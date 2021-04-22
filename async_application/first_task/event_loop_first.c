#include <event2/event.h>
#include <stdio.h>
#include <unistd.h>

void event_callback(evutil_socket_t fd, short event, void* data)
{
	char buf[100];
	read(fd, buf, 100);
	printf("%s\n", buf);
}


/*
*	Parent send to the pipe "Hello"
*	Child use event.h to wait until reading become available
*/

int main()
{
	int pipe_fds[2];
	pipe(pipe_fds);
	if (fork())
	{
		sleep(4);
		close(pipe_fds[0]);
		write(pipe_fds[1], "Hello!\n", 8);
		return 0;
	}


	
	close(pipe_fds[1]);

	struct event_base* ev_base = event_base_new();
	struct event* ev = event_new(ev_base, pipe_fds[0], EV_READ, event_callback, NULL);
	event_add(ev, NULL);
	event_base_loop(ev_base, EVLOOP_ONCE); //waiting until all events happened and stop loop

	event_free(ev);
	event_base_free(ev_base);
	return 0;
}