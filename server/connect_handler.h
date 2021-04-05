#include <mutex>
#include <list>
#include <thread>
#include <iostream>

#include "controller.h"
#include "event.h"


/*
	treat requsets of clients and send decision of class Controller
*/

class Handler
{
private:
	Controller open_cntlr_, exec_cntlr_;
	std::list<int> client_sockets_; // list is used because sockets must be removed when connection ends or interrupted
	mutable std::recursive_mutex mutex_;
	bool shutdown_ = false;
	bool modified_ = false; // list modifying

	std::thread handler_;
	static void run_proxy(void*);
	void run_handler_routine();
public:
	Handler(const char* open_blocked_paths, const char* exec_blocked_paths);
	~Handler();
	void add_socket(int sck);

};

