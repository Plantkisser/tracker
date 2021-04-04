#include "controller.h"

#include <string.h>
#include <stdlib.h>

/*static std::string env_variables_substitution(const std::string& original_string)
{
	std::string result_str = "";
	std::string::size_type old_pos = 0, pos = 0;
	while (1)
	{
		old_pos = pos;
		pos = original_string.find("$", pos);
		if (pos == std::string::npos)
		{
			result_str += original_string.substr(old_pos);
			return result_str;
		}
		result_str += original_string.substr(old_pos, pos - old_pos);

		std::string::size_type left_bracket = original_string.find("{", pos);
		std::string::size_type right_bracket = original_string.find("}", pos);
		std::string env_var = original_string.substr(left_bracket + 1, right_bracket - left_bracket - 1);

		std::string substitution = getenv(env_var.c_str());
		result_str += substitution;
		pos = right_bracket + 1;
	}
}*/



bool Controller:: is_allowed(const char* path)
{
	int size = blocked_path_.size();
	for (int i = 0; i < size; ++i)
	{
		if (!strcmp(path, blocked_path_[i].c_str()))
			return false;
	}

	return true;
}

Controller:: Controller(const std::vector<std::string>& blocked_path):
blocked_path_(blocked_path)
{}
