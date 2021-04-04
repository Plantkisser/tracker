#include "control_server.h"
#include <unistd.h>

int main()
{
	ControlServer server;

	server.start_server("open.txt","exec.txt");
	sleep(10);
	server.stop_server();
}