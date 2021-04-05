#define SLEEP_TIME_NS 100000000

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
	struct pollfd* arr_sck = NULL;
	nfds_t nfds = 0;

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
				if (arr_sck)
				{
					delete[] arr_sck;
				}

				nfds = client_sockets_.size();
				arr_sck = new pollfd[nfds]; 

				int i = 0;
				for (auto it = client_sockets_.begin(); it != client_sockets_.end(); ++i, ++it)
				{
					arr_sck[i].fd = *it;
					arr_sck[i].events = POLLIN;
				}

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

		int timeout = 1000; // ms
		poll(arr_sck, nfds, timeout);

		for(unsigned int i = 0; i < nfds; ++i)
		{
			if (arr_sck[i].revents == POLLRDHUP)
			{
				std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
				client_sockets_.erase(find(client_sockets_.begin(), client_sockets_.end(), arr_sck[i].fd));
				close(arr_sck[i].fd);
				modified_ = true;
			}

			if (arr_sck[i].revents == POLLIN)
			{
				tracer::event::inet::Message mesg;
				ssize_t res = recv(arr_sck[i].fd, &mesg, sizeof(mesg), 0);

				if (res == 0)
				{
					std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
					client_sockets_.erase(find(client_sockets_.begin(), client_sockets_.end(), arr_sck[i].fd));
					close(arr_sck[i].fd);
					modified_ = true;
					continue;
				}

				if (res < 0)
				{
					if (errno == EBADF)
					{
						std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
						client_sockets_.erase(find(client_sockets_.begin(), client_sockets_.end(), arr_sck[i].fd));
						close(arr_sck[i].fd);
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
					//case tracer::event::Type::OPENAT
					case tracer::event::Type::OPEN:
						IPRINTF("OPEN");
						mesg.decision = open_cntlr_.is_allowed(mesg.path);
						send(arr_sck[i].fd, &mesg, sizeof(mesg), 0);
						break;
					case tracer::event::Type::EXEC:
						IPRINTF("EXEC");
						mesg.decision = exec_cntlr_.is_allowed(mesg.path);
						send(arr_sck[i].fd, &mesg, sizeof(mesg), 0);
						break;
					default:
						mesg.decision = true;
						send(arr_sck[i].fd, &mesg, sizeof(mesg), 0);
						break;
				}
			}
		}


	}
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


