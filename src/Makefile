
all:	clean-ckpt libckpt.so hello restart server

default: all
	
clean-ckpt:
	rm -rf myckpt 

clean: clean-ckpt
	rm -rf server restart hello ckpt ckpt.o libckpt.so hello.o

ckpt.o: ckpt.c
	gcc -fno-stack-protector -c -fpic -o ckpt.o ckpt.c

libckpt.so: ckpt.o
	gcc -fno-stack-protector -shared -o libckpt.so ckpt.o

hello.o: hello.c
	gcc -c -fno-stack-protector -Wall -Werror -fpic -o hello.o hello.c

hello:	hello.o
	gcc -g -fno-stack-protector -o hello hello.o

restart: myrestart.c
	gcc -fno-stack-protector -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -g -o restart myrestart.c

server: restart server.c
	gcc -o server server.c

ckpt:   libckpt.so hello
	(sleep 3 && kill -12 `pgrep -n hello` && sleep 2 && pkill -9 hello) & 
	LD_PRELOAD=`pwd`/libckpt.so ./hello
	(sleep 6 &&  pkill -9 restart) &

res: 	server
	./server 8555

gdb:
	gdb --args ./restart myckpt

dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
