# General description
This is client part. It is responsible for tracing processes on machine and control they activity (syscalls).

# Features of work
1) 	Program trace all processe using ptrace().
2) 	This program will attach all processes which pid is greater than pid of running program.
3) 	If you want to change selection rules you should change function 'check_pid()' in file 'tracer.cpp' 
4) 	Every syscall which was called by traced process will be checked
	and if it is known syscall_type it will be send to server.
	Server will decide is it allowed or not, and if it is prohibitted
	process will get error EPERM as return value of syscall.

5)	Server will need syscall type and path. 
6)	Now only exec open and openat can be blocked and decision is based on path.
7)	'tracer.' responsible for work with ptrace and syscall treating
	'client.' responsible for getting decision from server
	'timer.' is simple timer for sending signal to tread which call PeriodicTimer class 
	'service.' is main class which turn on all parts of client part program

8) 	If you need some more information about ptrace check Ptrace_description or see 'man ptrace' 
9) 	A lot of syscalls have only number and no name you can change it if you want by adding new in 'utils/event.h' 
	but remember server also use this file so you need to recompile it too
	and take care number of syscall must be equal to its real number.