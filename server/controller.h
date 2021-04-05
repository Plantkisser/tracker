#include <vector>
#include <string>


//this class contain blocked paths and compare requested path with blocked
class Controller
{
private:
	std::vector<std::string> blocked_path_;
public:
	bool is_allowed(const char* path);
	Controller(const std::vector<std::string>&);
	~Controller() = default;
};		