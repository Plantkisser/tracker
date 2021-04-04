#define BUF_SIZE 100
#include <stdexcept>

#include "client.h"

#include <stdio.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>

bool ClientController:: is_allowed(tracer::event::Type syscall_type, const char* path)
{
	tracer::event::inet::Message request;
	request.event = syscall_type;
	if (path)
		sprintf(request.path, "%s", path);
	else
		memset(request.path, 0, sizeof(request.path)); 

	request.decision = false;
	if (send(server_socket_, &request, sizeof(request), 0) <= 0 || recv(server_socket_, &request, sizeof(request), 0) <= 0) // what if count of bytes doesn't equal size of struct Message
	{
		perror("send recv");
		throw(std::runtime_error("problem with client-server communication"));
	}
	return request.decision;
}

ClientController:: ClientController(const char* config_file)
{
	int fd = open(config_file, O_RDONLY);
	char buf[BUF_SIZE];
	struct in_addr in;
	int size = read(fd, buf, BUF_SIZE);
	char* ptr = (char*) memchr(buf, '\n', size);
	*ptr = '\0';
	char* addr = strdup(buf);
	inet_aton(addr, &in);
	free(addr);

	ptr++;
	size = size - (ptr - buf);
	char* old_ptr = ptr;
	ptr = (char*) memchr(old_ptr, '\n', size);
	if (!ptr)
	{
		ptr = (char*) memchr(old_ptr, '\0', size);
		if (!ptr)
		{
			throw(std::runtime_error("wrong config file"));
		}

	}
	*ptr = '\0';
	unsigned int port = atoi(old_ptr);


	struct sockaddr_in s_addr;
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(port);
	s_addr.sin_addr = in;

	server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(server_socket_, (struct sockaddr*) &s_addr, sizeof(s_addr)) < 0)
		throw (std::runtime_error("connection to server failed"));
}

ClientController:: ~ClientController()
{
	close(server_socket_);
}
