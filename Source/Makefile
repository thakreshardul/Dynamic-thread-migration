CC=gcc
SRC=src
TEST=test
BUILD=bin
INCLUDE=include
LIBS=lib

CFLAGS= -I${INCLUDE}

build-dirs:
	mkdir -p $(BUILD) 

build-lib: $(INCLUDE)/mThread.h $(SRC)/mThread.c
	$(CC) -c -Wall -Werror -fpic -o $(BUILD)/mThread.o $(SRC)/mThread.c
	$(CC) -shared -o $(LIBS)/libmThread.so $(BUILD)/mThread.o

hello : build-dirs
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(TEST)/$@.c

restart: $(SRC)/myrestart.c
	$(CC) -fno-stack-protector -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -g -o $(BUILD)/restart $(SRC)/myrestart.c

server: build-dirs restart $(SRC)/server.c
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(SRC)/$@.c

clean: 
	rm -rf $(BUILD)

all: 	build-dirs restart server build-lib hello
	
run-server: 	
	$(BUILD)/server 8555

run-test:
	$(BUILD)/hello -lmThread
