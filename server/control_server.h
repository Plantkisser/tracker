#include "listener.h"
#include <memory>

/*
	this is main class which turn on all server
*/

class ControlServer
{
private:
	std::unique_ptr <Listener> listener_ptr_;
public:
	ControlServer();
	~ControlServer();

	void start_server(const char* open_blocked_paths, const char* exec_blocked_paths);
	void stop_server();
};


