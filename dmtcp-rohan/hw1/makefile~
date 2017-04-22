all:	check

default: check
	
clean-logmyckpt:
	rm -rf log.myckpt
	
clean-logrestart:
	rm -rf log.restart
	
clean-logrestorememory:
	rm -rf log.restorememory

clean-libckpt:
	rm -rf myckpt

clean: clean-libckpt clean-logmyckpt clean-logrestart clean-logrestorememory
	rm -rf myrestart hello libckpt libckpt.o libckpt.so hello.o

libckpt.o: libckpt.c
	gcc -c -fno-stack-protector -fpic -o libckpt.o libckpt.c

libckpt.so: libckpt.o
	gcc -shared -fno-stack-protector -o libckpt.so libckpt.o

hello.o: hello.c
	gcc -c -fno-stack-protector -fpic -o hello.o hello.c

hello:	hello.o
	gcc -g -fno-stack-protector -o hello hello.o

myrestart: myrestart.c
	gcc -g -fno-stack-protector -static -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -o myrestart myrestart.c

res: 	myrestart
	./myrestart myckpt

gdb:
	gdb --args ./myrestart myckpt

check:	clean-libckpt clean-logmyckpt clean-logrestart clean-logrestorememory libckpt.so hello myrestart
	(sleep 4 && kill -12 `pgrep -n hello` && sleep 2 && pkill -9 hello) & 
	LD_PRELOAD=`pwd`/libckpt.so ./hello
	(sleep 4 && kill -12 `pgrep -n myrestart`) &
	make res

dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
