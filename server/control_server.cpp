#include "control_server.h"

void ControlServer:: start_server(const char* open_blocked_paths, const char* exec_blocked_paths)
{
	if (listener_ptr_)
	{
		throw(std::runtime_error("Server has already started"));
	}
	listener_ptr_.reset(new Listener(open_blocked_paths, exec_blocked_paths));
}

void ControlServer:: stop_server()
{
	if (!listener_ptr_)
	{
		throw(std::runtime_error("Server has already stopped"));
	}
	listener_ptr_.reset();

}

ControlServer:: ControlServer():
listener_ptr_()	
{}

ControlServer:: ~ControlServer()
{
	if (listener_ptr_)
		listener_ptr_.reset();
}