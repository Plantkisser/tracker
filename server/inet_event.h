#pragma once
#define PATH_SIZE 256

namespace inet_event
{
	enum class Type
	{
		EXEC,
		OPEN
	};

	struct Message
	{
		Type event;
		char path[PATH_SIZE];
		bool decision;
	};
}