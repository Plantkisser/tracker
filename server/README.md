# General description 
Server part is responsible for decision-making. It take all paths listed in 'controller' directory, for each syscall its own blocked paths (to find example look in 'controller' directory).
# Features of work
1) Only Controller is responsible for comparison logic.
2) Now only open and exec syscalls are controlled and can be blocked, but you can add your own by adding another Controller in Handler class.
 