#include "control_server.h"
#include <unistd.h>

int main()
{
	ControlServer server;

	server.start_server("../../controller/open_blocked","../../controller/exec_blocked");
	sleep(100);
	server.stop_server();
}