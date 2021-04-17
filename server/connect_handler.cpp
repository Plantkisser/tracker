#define SLEEP_TIME_NS 100000000
#define POLL_TIMEOUT_MS 100

#include "connect_handler.h"
#include "event.h"
#include "parser.h"
#include "debug.h"

#include <algorithm>

#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

/*POLLOUT -----------------------------*/

/*supporting structures for convenient working with sockets*/

struct Message_for_sending
{
	int socket;
	tracer::event::inet::Message mesg;
	unsigned offset;

	Message_for_sending(int sck, tracer::event::inet::Message message, unsigned off): socket(sck), 
	mesg(message), offset(off) {}
};





void Handler:: add_socket(int socket)
{
	std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
	client_sockets_.push_back(socket);
	modified_ = true;
}

void Handler:: run_proxy(void* ptr)
{
	static_cast<Handler*>(ptr)->run_handler_routine();
}

void Handler:: run_handler_routine()
{
	std::vector<pollfd> arr_sck; 	// this used by poll

	std::list<Message_for_sending> send_list; 	// contain messages which will be sent 
									// when poll confirm that it is available

	nfds_t nfds = 0; 				// amount of sockets which we are 'polling'

	while(1)
	{
		{
			std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
			if (shutdown_)
			{
				break;
			}

			if (modified_)
			{
				if (arr_sck.size())
				{
					arr_sck.clear();
				}

				int i = 0;
				for (auto it = client_sockets_.begin(); it != client_sockets_.end(); ++i, ++it)
				{
					struct pollfd tmp;
					tmp.fd = *it;
					tmp.events = POLLIN;
					arr_sck.push_back(tmp);
				}
				nfds = arr_sck.size();

				modified_ = false;
			}
		}

		if (nfds == 0)
		{
			timespec req;
			req.tv_sec = 0;
			req.tv_nsec = SLEEP_TIME_NS;
			nanosleep(&req, NULL);
			continue;
		}

		int timeout = POLL_TIMEOUT_MS; // ms
		poll(arr_sck.data(), nfds, timeout);

		for(unsigned int i = 0; i < nfds; ++i)
		{
			switch (arr_sck[i].revents)
			{
				case POLLRDHUP:
				{
					std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
					client_sockets_.erase(find(client_sockets_.begin(), client_sockets_.end(), arr_sck[i].fd));
					shutdown(arr_sck[i].fd, SHUT_RDWR);
					modified_ = true;
					break;
				}
				case POLLIN:
				{
					tracer::event::inet::Message mesg;
					ssize_t res = recv(arr_sck[i].fd, &mesg, sizeof(mesg), MSG_DONTWAIT);
					if (res == 0)
					{
						std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
						client_sockets_.erase(find(client_sockets_.begin(), client_sockets_.end(), arr_sck[i].fd));
						shutdown(arr_sck[i].fd, SHUT_RDWR);
						modified_ = true;
						continue;
					}
					if (res < 0 && (errno == EWOULDBLOCK || errno == EAGAIN))
					{
						IPRINTF("NOMSG");
						continue;
					}

					if (res < 0)
					{
						if (errno == EBADF)
						{
							std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
							client_sockets_.erase(find(client_sockets_.begin(), client_sockets_.end(), arr_sck[i].fd));
							shutdown(arr_sck[i].fd, SHUT_RDWR);
							modified_ = true;
						}
						else
						{
							perror("Handler recv");
						}
					}

					if (res != sizeof(mesg))
					{
						IPRINTF("Strange message\nPath:%s %d\n", mesg.path, (int)res);
					}


					switch (mesg.event)
					{
						case tracer::event::Type::OPENAT:
						case tracer::event::Type::OPEN:
							IPRINTF("OPEN");
							mesg.decision = open_cntlr_.is_allowed(mesg.path);
							break;
						case tracer::event::Type::EXEC:
							IPRINTF("EXEC");
							mesg.decision = exec_cntlr_.is_allowed(mesg.path);
							break;
						default:
							mesg.decision = true;
							break;
					}
					send_list.push_back(Message_for_sending(arr_sck[i].fd, mesg, 0));
					
					//sleep(5);
					//send(arr_sck[i].fd, &mesg, sizeof(tracer::event::inet::Message), MSG_DONTWAIT | MSG_NOSIGNAL);
					arr_sck[i].events = POLLOUT;

					break;
				}
				case POLLOUT:
				{
					for (auto it = send_list.begin(); it != send_list.end(); )
					{
						if (it->socket != arr_sck[i].fd)
						{
							++it;
							continue;
						}

						int res = send(arr_sck[i].fd, &(it->mesg) + it->offset, sizeof(tracer::event::inet::Message)-it->offset, MSG_DONTWAIT | MSG_NOSIGNAL);

						if (res < 0  && (errno == EWOULDBLOCK || errno == EAGAIN || errno == ENOMEM)) // find another cases when server don't need to close socket
						{
							perror("send");
							break;
						}
						if (res < 0)
						{
							perror("send to client");
							// correctly close connection
						}
						it->offset += res;

						if (it->offset == sizeof(tracer::event::inet::Message))
						{
							it = send_list.erase(it);
							arr_sck[i].events = POLLIN;
						}
						else
							++it;
					}
					break;
				}
			}
		}
	}
	return;
}

Handler:: Handler(const char* open_blocked_paths, const char* exec_blocked_paths):
open_cntlr_(file_parse(open_blocked_paths)),
exec_cntlr_(file_parse(exec_blocked_paths)),
handler_(run_proxy, this)
{}

Handler:: ~Handler()
{
	{
		std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
		shutdown_ = true;
	}
	handler_.join();
}


