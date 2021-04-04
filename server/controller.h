#include <vector>
#include <string>


class Controller
{
private:
	std::vector<std::string> blocked_path_;
public:
	bool is_allowed(const char* path);
	Controller(const std::vector<std::string>&);
	~Controller() = default;
};		