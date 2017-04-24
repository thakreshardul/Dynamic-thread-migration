CC=gcc
SRC=src
TEST=test
BUILD=bin
INCLUDE=include
LIBS=lib

CFLAGS= -I${INCLUDE}

build-dirs:
	mkdir -p $(BUILD) 
	mkdir -p $(LIBS) 

#lib-mThread: $(INCLUDE)/mThread.h $(SRC)/mThread.c
#	$(CC) $(CFLAGS) -fpic -c -o $(BUILD)/mThread.o $(SRC)/mThread.c
#	$(CC) $(CFLAGS) -shared -fpic -o $(LIBS)/libmThread.so $(BUILD)/mThread.o

mthread: $(INCLUDE)/mThread.h $(SRC)/mThread.c
	$(CC) $(CFLAGS) -c -o $(BUILD)/mThread.o $(SRC)/mThread.c

lib-ckpt: ckpt.o
	$(CC) -fno-stack-protector -shared -o $(LIBS)/libckpt.so $(BUILD)/ckpt.o

build-lib: lib-ckpt

ckpt.o: $(SRC)/ckpt.c
	$(CC) $(CFLAGS) -fno-stack-protector -c -fpic -o $(BUILD)/ckpt.o $(SRC)/ckpt.c


restart: $(SRC)/myrestart.c
	$(CC) -fno-stack-protector -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -g -o $(BUILD)/restart $(SRC)/myrestart.c

server: build-dirs restart $(SRC)/server.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(SRC)/$@.c

hello : build-dirs
	$(CC) $(CFLAGS) -I$(SRC) -c -o $(BUILD)/$@.o $(TEST)/$@.c 
	$(CC) $(CFLAGS) -I$(SRC) -o $(BUILD)/$@ $(BUILD)/$@.o

clean:  clean-lib
	rm -rf $(BUILD)

clean-lib: 
	rm $(LIBS)/libckpt.so


all: 	build-dirs mthread restart server build-lib hello
	
run-server1: 	
	$(BUILD)/server 8555

run-server2: 	
	$(BUILD)/server 8556

run-server3: 	
	$(BUILD)/server 8557

run-test:
	export LD_LIBRARY_PATH=`pwd`/$(LIBS):$LD_LIBRARY_PATH
	LD_PRELOAD=`pwd`/$(LIBS)/libckpt.so $(BUILD)/hello 4 1

