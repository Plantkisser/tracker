#include "event.h"


/*
	Class send requests about syscalls to the server and return its decision 
*/
class ClientController
{
private:
	int server_socket_;
public:
	bool is_allowed(tracer::event::Type syscall_type, const char* path);

	/*
		config_file is path to the config file which contains ip address and port of server 
		look at directory controller to see example of such file
	*/
	ClientController(const char* config_file); 
	~ClientController();
};