#define BUF_SIZE 100

#include "parser.h"

#include <stdexcept>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

static bool check_str(std::string str)
{
	if (str[0] == '#')
		return false;
	for (unsigned int i = 0; i < str.size(); ++i)
		if (isalnum(str[i]))
			return true;
	return false;
}

std::vector<std::string> file_parse(const char* file_name)
{
	int fd = open(file_name, O_RDONLY, 0666);
	if (fd < 0)
	{
		throw(std::runtime_error("Open failed"));
	}

	std::vector<std::string> result;	
	int read_res = -1;
	char buf[BUF_SIZE];
	std::string tmp_str = "";
	while (1)
	{
		read_res = read(fd, buf, BUF_SIZE-1);
		if (read_res < 0)
		{
			throw(std::runtime_error("read  failed"));
		}

		if (read_res == 0)
		{
			if (check_str(tmp_str))
			{
				result.push_back(tmp_str);
			}
			break;
		}

		buf[read_res] = '\0';

		char* ptr = buf;
		char* old_ptr = ptr;
		while(1)
		{
			ptr = strchr(ptr, '\n');

			if (ptr == NULL)
			{
				tmp_str += old_ptr;
				break;
			}
			else
			{
				*ptr = '\0';
				tmp_str += old_ptr;
				if (check_str(tmp_str))
				{
					result.push_back(tmp_str);
				}
				ptr++;
				tmp_str = "";
				old_ptr = ptr;
			}
		}
	}

	return result;
}