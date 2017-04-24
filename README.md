# Dynamic-thread-migration
Source code: 

https://github.com/thakreshardul/Dynamic-thread-migration.git

Source code for this implementation would be in Source directory of above specified Git repository.

Directory structure:

include: all headers of mthread library

lib: contains shared objects

src: source code of this implementation 

test: user program

Makefile: 
	make all:
	make run-server 1 : this will run the servers on ports 7777
	make run-server 2 : this will run the servers on ports 7778
	make run-server 3 : this will run the servers on ports 7779 
	make run-server 4 : this will run the servers on ports 7780
	make run-server 5 : this will run the servers on ports 7781
	make run-test : will start execution for the specified parameters, 
			currently set to 4 2 which will calculate e^4 unto 2 terms
