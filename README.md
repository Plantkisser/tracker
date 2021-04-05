# tracker

This is 'process tracker' project.


# Formatting

This file will be primarily viewed in primitive Linux text console.
Rendering in browser is secondary.


# Task overview

I don't like some daemons (for example at-spi2-registry, gvfsd-http,
udisksd). Unfortunately I can't just uninstall them because some
applications which I use depend on those daemons. Usually I periodically
manually kill those daemons. I can setup 'cron job' to automatically kill
them. But I want special tool with some specific features...

We will need Linux kernel module which will track 'fork', 'exec' and
'exit' events in Linux kernel.

We will need user space application which will react on events reported
by kernel module and will decide when and what to do with different
processes.

We will need configuration file which will specify which daemon shall be
killed and how soon it shall happen.


# Future plans

Our tool can be used by 'parents' to control how much time 'childern' may
spend playing computer games.


# How to use

If you want to use application:
0) Go to controller directory and write ip address and port of server
1) Go to service directory and run "make"
2) Go to BUILD_DIR and open "service" directory
3) Run "service"
May be you will need to run it as superuser to be permitted to trace process

To run server:
0) Go to controller directory and write paths you want to block for open and execve
1) Go to server directory and run "make"
2) Go to BUILD_DIR and open "server" directory
3) Run "server"
