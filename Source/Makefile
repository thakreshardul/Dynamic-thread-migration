
CC=gcc
SRC=src
BUILD=bin
INCLUDE=include
LIBS=lib
DMTCP_ROOT=`pwd`/dmtcp
DMTCP_INCLUDE=${DMTCP_ROOT}/include
DMTCP_LIB=${DMTCP_ROOT}/lib
DMTCP_BIN=${DMTCP_ROOT}/bin


CFLAGS= -fPIC -I${INCLUDE} -I${DMTCP_INCLUDE}
LD_FLAGS= $(DMTCP_LIB)/libdmtcp_alloc.so $(DMTCP_LIB)/libdmtcp_dl.so $(DMTCP_LIB)/libdmtcp_ipc.so $(DMTCP_LIB)/libdmtcp_svipc.so $(DMTCP_LIB)/libdmtcp_timer.so $(DMTCP_LIB)/libdmtcp.so $(DMTCP_LIB)/libdmtcp_pid.so


build-dirs:
	mkdir -p $(BUILD) 


hello : build-dirs
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(SRC)/$@.c


clean: 
	rm -rf $(BUILD)


all: 	build-dirs hello
	LD_PRELOAD=$(LD_FLAGS) $(DMTCP_BIN)/dmtcp_launch --coord-port 7779 $(BUILD)/hello
