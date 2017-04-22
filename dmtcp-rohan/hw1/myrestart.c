#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ucontext.h>
#include <unistd.h>
#include <stdlib.h>
#include <ucontext.h>
#include "read_header.h"

char checkpoint_image[1024];
ucontext_t ucon,*ucon_ptr;

void restore_memory(struct MemoryRegion *mr_myrestart){
	FILE *loglog = fopen("log.restorememory","w");
	
	if(munmap(mr_myrestart->startAddr,mr_myrestart->size)==-1){
		fprintf(loglog,"Stack memory unmap unsuccessful.\n");
	}else{
		fputs("Stack memory unmap successfull.\n",loglog);
	}
	
	// open file myckpt in same location as this .so file in write-only, append, create file mode, with RWX permissions for the owner
	int checkpoint = open(checkpoint_image,O_RDONLY);
	struct MemoryRegion mr;
	
	int header_number = 0;
	VA old_stack_pointer;
	
	while(1){
	
		header_number++;
		
		// read the memory region header from checkpoint file
		int header_read_success = read(checkpoint,&mr,sizeof(struct MemoryRegion));
		fprintf(loglog,"Header read for %s.\n",mr.name);
		if(header_read_success == 0){
			fprintf(loglog,"There was an error reading header number %d from checkpoint file. Terminating read operation.\n",header_number);
			break;
		}else{
		
			char *data;
			char *remap_success;
			data = (char*)malloc(mr.size);
		
			int data_read_success = read(checkpoint,data,mr.size);
			
			fprintf(loglog,"Data read.\n");
			if(data_read_success == 0){
				fprintf(loglog,"There was an error reading data corresponding to header number %d from checkpoint file. Terminating read operation.\n",header_number);
				break;
			}
			if(strstr(mr.name,"[stack]")==NULL){
				remap_success = mmap(mr.startAddr, mr.size , PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, mr.offset);
			}else{
				remap_success = mmap(mr.startAddr, mr.size , PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS | MAP_GROWSDOWN, -1, mr.offset);
			}
		
			if(remap_success == MAP_FAILED){
				fprintf(loglog,"Error in remapping %s. Terminating restore operation.\n",mr.name);
				return;
			}
			
			int memcpy_success = memcpy(mr.startAddr,data,mr.size);
			
		}
		
		// exit loop if vsyscall header has been read
		if(strstr(mr.name,"vdso")!=NULL){
			break;
		}
	}
	
	ucon_ptr = &ucon;
	int context_read_success = read(checkpoint,ucon_ptr,sizeof(ucontext_t));

	if(context_read_success == 0){
		fprintf(loglog,"There was an error reading the context registers from checkpoint file. Terminating read operation.\n");
		return;
	}
	
	fclose(loglog);
	if(setcontext(ucon_ptr)==-1){
		fprintf(loglog,"Error setting context.\n");
	}
	fclose(loglog);
	exit(0);
}

void main(int argc, char const *argv[]){
	
	VA memory_map_success,new_stack_pointer = 0x700000;
		
	// open file to maintain log
	FILE *log = fopen("log.restart","a+");
	
	strcpy(checkpoint_image,argv[1]);
	
	// open /proc/self/maps file in read-mode
	int fd = open("/proc/self/maps",O_RDONLY);
	
	// keep count of line;
	int line_number=0;
	
	//execute the following till you hit an exit loop call
	while(1){
		// keep count of line to be read
		line_number++;
		
		fprintf(log,"\nOn line %d.\n",line_number);
		
		// instance of struct MemoryRegion - to be populated on reading the file line by line
		struct MemoryRegion mr;
		
		int line_read_success = populate_struct(fd,&mr,log);
		
		// line read done
		if(line_read_success == 0){
			fprintf(log,"Error reading line %d. Terminating file read operation.\n",line_number);
			break;
		}else if(line_read_success == -1){
			fprintf(log,"End of file reached on line %d\n",line_number);
			break;
		}
		
		if(strstr(mr.name,"[stack]")!=NULL){
			memory_map_success = mmap(new_stack_pointer, mr.size , PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_GROWSDOWN, -1, 0);
			
			if(memory_map_success == MAP_FAILED){
				fprintf(log,"Error in mapping stack memory. Terminating restore operation.\n");
				return;
			}
			
			memcpy(memory_map_success,mr.startAddr,mr.size);
			
			fclose(log);
			asm volatile ("mov %0,%%esp;"
						 :
						 : "g" (memory_map_success) 
						 : "%rsp");
			restore_memory(&mr);
		}
	}
	fclose(log);
	close(fd);
}
