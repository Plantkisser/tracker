#define PORT 50000
#define SLEEP_TIME_NS 50000000
#include "listener.h"

#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>


void Listener:: run_proxy(void* ptr)
{
	static_cast<Listener*>(ptr)->run_listener_routine();
}

void Listener:: run_listener_routine()
{
	int listen_sck = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sck < 0)
	{
		perror("listen socket");
		return;
	}
	struct sockaddr_in s_in;
	s_in.sin_family = AF_INET;
	s_in.sin_port = htons(PORT);
	s_in.sin_addr.s_addr = INADDR_ANY;

	if (bind(listen_sck, (sockaddr*) &s_in, sizeof(s_in)))
	{
		perror("bind");
		return;
	}

	if (listen(listen_sck, 3) < 0)
	{
		perror("listen");
		return;
	}
	
	socklen_t len;
	getsockname(listen_sck, (struct sockaddr*) &s_in, &len);
	if (len != sizeof(struct sockaddr_in))
	{
		printf("Another type\n");
		return;
	}


	fcntl(listen_sck, F_SETFL, O_NONBLOCK);
	while(1)
	{
		{
			std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
			if (shutdown_)
				break;
		}

		int new_sck = accept(listen_sck, NULL, NULL);
		if (new_sck < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		{
			timespec req;
			req.tv_sec = 0;
			req.tv_nsec = SLEEP_TIME_NS;
			nanosleep(&req, NULL);
			continue;
		}

		handler_ptr_->add_socket(new_sck);
	}
}




Listener:: Listener(const char* open_blocked_paths, const char* exec_blocked_paths):
listener_(run_proxy, this)
{
	//listener starts
	handler_ptr_.reset(new Handler(open_blocked_paths, exec_blocked_paths));
	//handler starts
}

Listener:: ~Listener()
{
	{
		std::lock_guard<std::recursive_mutex> lock_guard(mutex_);
		shutdown_ = true;
	}
	listener_.join();
	
	handler_ptr_.reset();
}