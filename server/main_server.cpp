#include "control_server.h"
#include "debug.h"
#include <unistd.h>

int main()
{
	int status = 0;
	try
	{
		ControlServer server;
		server.start_server("../../controller/open_blocked","../../controller/exec_blocked");
		sleep(100);
		server.stop_server();
	}
	catch(std::exception exc)
	{
		EPRINTF("exception: %s", exc.what());
		status = -1;
	}
	catch(...)
	{
		EPRINTF("unexpexcted exception");
		status = -1;
	}
	return status;
}