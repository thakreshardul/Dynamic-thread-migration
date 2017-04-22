#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ucontext.h>
#include <signal.h>
#include <stdlib.h>
#include "read_header.h"

void checkpoint_handler(int signo){
	if(signo==SIGUSR2){
		// open /proc/self/maps file in read-mode
	int fd = open("/proc/self/maps",O_RDONLY);
	
	// open file myckpt in same location as this .so file in write-only, append, create file mode, with RWX permissions for the owner
	int checkpoint = open("myckpt",O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
	
	// open file to maintain log
	FILE *log = fopen("log.myckpt","a");
	
	// keep count of line;
	int line_number=0;
	
	//execute the following till you hit an exit loop call
	while(1){
		// keep count of line to be read
		line_number++;
		
		// instance of struct MemoryRegion - to be populated on reading the file line by line
		struct MemoryRegion mr;
		
		int line_read_success = populate_struct(fd,&mr,log);
		
		// line read done
		if(line_read_success == 0){
			break;
		}else if(line_read_success == -1){
			break;
		}

		// write the memory region data in checkpoint file
		if((mr.permflags && (1<<3)) && strstr(mr.name,"vsyscall")==NULL){
		
			// write the memory region header in checkpoint file
			int header_write_success = write(checkpoint,&mr,sizeof(struct MemoryRegion));
			if(header_write_success == -1){
				break;
			}else if(header_write_success < sizeof(struct MemoryRegion)){
			}
		
			int data_write_success = write(checkpoint,mr.startAddr,mr.size);
			if(data_write_success == -1){
				break;
			}else if(data_write_success < mr.size){
			}
		}
	}
	
	int pid = getpid();
	
	// pass pointer to a variable of type ucontext_t
	ucontext_t ucon, *ucon_ptr = &ucon;
	if(getcontext(ucon_ptr)==-1){
	}
	
	if(getpid() != pid)		return;
	
	// write context to checkpoint file
	int context_write_success = write(checkpoint,ucon_ptr,sizeof(ucontext_t));
	if(context_write_success == -1){
		fprintf(1,"\n\nThere was a problem writing the context to the checkpoint.\n\n");
	}else if(context_write_success < sizeof(ucontext_t)){
		fprintf(1,"\n\nOnly some of the context for the process was written to the checkpoint file.\n\n");
	}
	
	fclose(log);
	close(fd);
	close(checkpoint);	

		printf("\n");
		exit(0);
	}
}

__attribute__((constructor))
void checkpoint_constructor(){
	signal(SIGUSR2,checkpoint_handler);
}

