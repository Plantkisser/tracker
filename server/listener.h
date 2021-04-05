#include "connect_handler.h"

#include <memory>
#include <thread>
#include <mutex>

/*
	listen and add new clients to Handler
*/

class Listener
{
private:
	mutable std::recursive_mutex mutex_;
	bool shutdown_ = false;

	std::thread listener_;
	std::unique_ptr <Handler> handler_ptr_;
public:
	Listener(const char* open_blocked_paths, const char* exec_blocked_paths);
	~Listener();

	static void run_proxy(void*);
	void run_listener_routine();
};

