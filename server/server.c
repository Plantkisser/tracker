#define BUF_SIZE 100

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


int main()
{
	int listen_sck = socket(AF_INET, SOCK_STREAM, 0);
	if (listen < 0)
	{
		perror("listen socket");
		return 0;
	}
	listen(listen_sck, 3);
	struct sockaddr_in s_in;
	socklen_t len;
	getsockname(listen_sck, (struct sockaddr*) &s_in, &len);
	if (len != sizeof(struct sockaddr_in))
	{
		printf("Another type\n");
		return 0;
	}

	int fd = open("config", O_WRONLY);
	char* addr = strdup(inet_ntoa(s_in.sin_addr));
	write(fd, addr, strlen(addr));
	free (addr);
	write(fd, "\n", 1);
	unsigned int port = ntohs(s_in.sin_port);
	char port_buf[20];
	sprintf(port_buf, "%u%c", port, '\0');
	write(fd, port_buf, strlen(port_buf));
	write(fd, "\0", 1);


	int sck_fd = accept(listen_sck, NULL, NULL);

	char buf[BUF_SIZE];
	recv(sck_fd, buf, BUF_SIZE, 0);

	printf("%s", buf);
	return 0;
}	