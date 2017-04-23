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

lib-mThread: $(INCLUDE)/mThread.h $(SRC)/mThread.c
	$(CC) -c -Wall -Werror -fpic -o $(BUILD)/mThread.o $(SRC)/mThread.c
	$(CC) -shared -o $(LIBS)/libmThread.so $(BUILD)/mThread.o

lib-ckpt: ckpt.o
	$(CC) -fno-stack-protector -shared -o $(LIBS)/libckpt.so $(BUILD)/ckpt.o

build-lib: lib-mThread lib-ckpt

ckpt.o: $(SRC)/ckpt.c
	$(CC) $(CFLAGS) -fno-stack-protector -c -fpic -o $(BUILD)/ckpt.o $(SRC)/ckpt.c


restart: $(SRC)/myrestart.c
	$(CC) -fno-stack-protector -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -g -o $(BUILD)/restart $(SRC)/myrestart.c

server: build-dirs restart $(SRC)/server.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(SRC)/$@.c

hello : build-dirs
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(TEST)/$@.c

clean: 
	rm -rf $(BUILD)

clean-lib: 
	rm $(LIBS)/libmThread.so
	rm $(LIBS)/libckpt.so


all: 	build-dirs restart server build-lib hello
	
run-server: 	
	$(BUILD)/server 8555

run-test:
	LD_PRELOAD=$(LIBS)/libckpt.so $(BUILD)/hello -lmThread
