#include "event.h"

class ClientController
{
private:
	int server_socket_;
public:
	bool is_allowed(tracer::event::Type syscall_type, const char* path);

	ClientController(const char* config_file);
	~ClientController();
};