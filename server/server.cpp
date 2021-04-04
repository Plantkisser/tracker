


class Handler
{
private:
	Controller open_cntlr_, exec_cntlr_;
	std::list<int> client_sockets_;
	mutable std::recursive_mutex mutex_;
	bool shutdown_ = false;

	std::thread handler_;
	static void run_proxy(void*);
	void run_handler_routine();
public:
	Handler(const char* open_blocked_paths, const char* exec_blocked_paths);
	~Handler();
	void add_socket(int sck);

}

Handler:: Handler(const char* open_blocked_paths, const char* exec_blocked_paths):
open_cntlr_(open_blocked_paths),
exec_cntlr_(exec_blocked_paths),
handler_(run_proxy(this))
{}






class Listener
{
private:
	mutable std::recursive_mutex mutex_;
	bool shutdown_ = false;

	std::thread listener_;
	std::unique_ptr <ConnectionHandler> handler_ptr_;
public:
	Listener(const char* open_blocked_paths, const char* exec_blocked_paths);
	~Listener();

	static void run_proxy();
	void run_listener();
};

Listener:: Listener(const char* open_blocked_paths, const char* exec_blocked_paths):
listener_(run_proxy, this)
{
	//listener starts
	handler_.reset(new Handler(open_blocked_paths, exec_blocked_paths));
	//handler starts
}















class Control_server
{
private:
	std::unique_ptr <Listener> listener_ptr_;
public:
	Control_server();
	~Control_server();

	void start_server(const char* open_blocked_paths, const char* exec_blocked_paths);
	void stop_server();
}


void Control_server:: start_server()
{
	listener_.reset(new Listener(const char* open_blocked_paths, const char* exec_blocked_paths));
}