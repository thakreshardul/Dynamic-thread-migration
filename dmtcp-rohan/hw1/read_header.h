typedef char* VA;

struct MemoryRegion{
	VA startAddr;
	VA endAddr;
	unsigned long long size;
	off_t offset;
	int permflags;
	unsigned long devmajor;
	unsigned long devminor;
	unsigned long inodenumber;
	char name[1024];
};

/* Read non-null character, return null if EOF */
char mtcp_readchar (int fd){
	char c;
	int rc;
	do {
		rc = read(fd, &c, 1);
	} while ( rc == -1 );
	if (rc < 0) return (0);
	if (rc==0)	return -1;
	return (c);
}

/* Read decimal number, return value and terminating character */
char mtcp_readdec (int fd, VA *value){
	char c;
	unsigned long int v;
	v = 0;
	while (1) {
	c = mtcp_readchar (fd);
	if ((c >= '0') && (c <= '9')) c -= '0';
	else break;
	v = v * 10 + c;
	}
	*value = (VA)v;
	return (c);
}
 
/* Read decimal number, return value and terminating character */
char mtcp_readhex (int fd, VA *value){
	char c;
	unsigned long int v;
	v = 0;
	while (1) {
		c = mtcp_readchar(fd);
		if ((c >= '0') && (c <= '9')) c -= '0';
		else if ((c >= 'a') && (c <= 'f')) c -= 'a' - 10;
		else if ((c >= 'A') && (c <= 'F')) c -= 'A' - 10;
		else break;
		v = v * 16 + c;
	}
	*value = (VA)v;
	return (c);
}

/* Throw error for bad file format.*/
int bad_format_error(FILE *log){
	fprintf(log,"Bad file format.\n");
	return 0;
}

/* Populate an instance of struct MemoryRegion by reading /proc/self/maps line by line.*/
int populate_struct(int fd, struct MemoryRegion *mr,FILE *log){
	char c,rf,wf,xf,pf;
	int i;
	off_t offset;
	unsigned long devmajor,devminor,inodenumber;
	VA startAddress,endAddress;
	
	c=mtcp_readhex(fd,&startAddress);
	if(c==-1){
		return c;
	}
	if(c!='-'){
		if((c==0) && (startAddress == 0)) return 0;
		return bad_format_error(log);
	}
	
	c=mtcp_readhex(fd,&endAddress);
	if(c!=' ' || endAddress - startAddress<0) return bad_format_error(log);
	
	rf = mtcp_readchar(fd);
	if(rf!='r' && rf!='-') return bad_format_error(log);
	
	wf = mtcp_readchar(fd);
	if(wf!='w' && wf!='-') return bad_format_error(log);
	
	xf = mtcp_readchar(fd);
	if(xf!='x' && xf!='-') return bad_format_error(log);
	
	pf = mtcp_readchar(fd);
	if(pf!='p' && pf!='s') return bad_format_error(log);
	
	c = mtcp_readchar(fd);
	if(c!=' ') return bad_format_error(log);
	
	c = mtcp_readhex(fd,(VA*)&offset);
	if(c!=' ') return bad_format_error(log);
	
	c = mtcp_readhex(fd,(VA*)&devmajor);
	if(c!=':') return bad_format_error(log);

	c = mtcp_readhex(fd,(VA*)&devminor);
	if(c!=' ') return bad_format_error(log);
	
	c = mtcp_readdec(fd,(VA*)&inodenumber);
	
	mr->name[0] = '\0';
	while(c==' ')	c=mtcp_readchar(fd);
	if(c=='/' || c=='['){
		i=0;
		do{
			mr->name[i++] = c;
			c = mtcp_readchar(fd);
		}while(c!='\n');
		mr->name[i]='\0';
	}
	
	mr->startAddr = startAddress;
	mr->endAddr = endAddress;
	mr->size = endAddress-startAddress;
	mr->offset = offset;
	mr->permflags = 0;
	if(rf=='r') mr->permflags |= (1<<3);
//	if(rf=='r') printf("1");
//	else		printf("0");
	if(wf=='w') mr->permflags |= (1<<2);
//	if(wf=='w') printf("1");
//	else		printf("0");
	if(xf=='x') mr->permflags |= (1<<1);
//	if(xf=='x')	printf("1");
//	else		printf("0");
	if(pf=='s') mr->permflags |= 1;
//	if(pf=='s') printf("1");
//	else		printf("0");
//	printf("\n");
	mr->devmajor = devmajor;
	mr->devminor = devminor;
	mr->inodenumber = inodenumber;
/*
	fprintf(log,"%x\t",mr->startAddr);
	fprintf(log,"%x\t",mr->endAddr);
	fprintf(log,"%x\t",mr->size);
	fprintf(log,"%x\t",mr->offset);
	fprintf(log,"%x\t",mr->permflags);
	fprintf(log,"%x\t",mr->devmajor);
	fprintf(log,"%x\t",mr->devminor);
	fprintf(log,"%x\t",mr->inodenumber);
	fprintf(log,"%s\n",mr->name);
*/
	return 1;
}
