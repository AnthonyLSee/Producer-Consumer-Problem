/*
 *	Anthony See
 *	producer.c
 *   157004252
 */


#include        <sys/types.h>
#include 		<sys/stat.h>
#include 		<sys/ipc.h>
#include 		<sys/shm.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <unistd.h>
#include        <errno.h>
#include        <string.h>
#include 		<fcntl.h>

#define chunkPartition 512
#define MAX_BUF 1024
#define recvMessage "RECV"
#define EOFMessage "EOF"

/* Directory PATHS depending on machine! */
//#define PATHvar	"/mnt/c/Users/Anthony See/Desktop/Programming/OS 2018/PA1/ftokFile.txt"
	// String Var for the local machine path

#define PATHvar "/var/autofs/rutgers/clam/u3/students/as2242/OS/ftokFile.txt"
	// String Var for clamshell path

//#define PATHmorbius "/var/autofs/rutgers/crab/u12/facstaff/morbius"
	// String Var for Russel's given path
	
/* Directory PATHS depending on machine! */


void foundError(char* s)
{
	perror(s);
	exit(1);
}

int fileOffset = 0;
int readFromFile(char * buffer, char* inputFile)
{
	/* Reads FROM file 
	 * returns :
	 * 		0 : Sucessful
	 *		1 : EOF
	 *
	 */
	
	FILE *		file;
	int 		i,size = 0;
	size_t 		nread;
	
	file = fopen(inputFile, "r");
	fseek(file,fileOffset,SEEK_CUR);
	fread(buffer,chunkPartition,1,file);
	fileOffset+=chunkPartition;
	
	if (feof(file))
	{	
		fclose(file);
		return 1;
	}
	fclose(file);
	return 0;
}

int main(int argc, char ** argv)
{
	if ( argc < 2 )
	{
		fprintf( stderr, "USAGE : 'EXAMPLE.txt' File %s line %d.\x1b[0m\n", __FILE__, __LINE__ );
		exit( 1 );
	}
	
	
	printf("\n\n ===================== STARTING PRODUCER! =====================\n\n");
	/* === VARS! ======================================	*/
	key_t		key;
	int			shmid;
	char*		p;
	char*		buffer;
	int			size = 393216;							// Referring to shared segment
	int 		keepGoing = 1;
	
	int 		fd;										// For FIFO / named pipes
    char * 		myfifo = "/tmp/myfifo";					// For FIFO / named pipes
    char 		recvBuf[MAX_BUF];						// For FIFO / named pipes
	/* === VARS! ======================================	*/
	
	
	buffer = (char*) malloc(sizeof(char)*chunkPartition+1);
	readFromFile(buffer,argv[1]);
	printf("First Chunk Grab: %s",buffer);
	printf("\n");
    
	
	/* === SHARED MEM! =============================== 	*/
	
	if ( errno = 0, (key = ftok( "ftokFile.txt", 42 )) == -1 )									// get key!
	{
		printf( "ftok() failed  errno :  %s\n", strerror( errno ) );
		exit( 1 );
	}
	else if (errno = 0, (shmid = shmget( key, size, 0666 | IPC_CREAT | IPC_EXCL )) != -1 )		// CREATE ok?
	{
		printf("			 - shmget : CREATE - \n");
		errno = 0;
		p = shmat( shmid, 0, 0 );
		if ( p == (void *)-1 )				/* Did shmat work? 	*/
		{									/* 		~no~		*/
			printf( "shmat() failed  errno :  %s\n", strerror( errno ) );
			exit( 1 );
		}
		else
		{									/* 		~Yes~		*/
			printf( "Process %d gets message from shared memory segment attached at address %#x.\n", getpid(), p );
			sprintf( p, "%s", buffer );		/* Put into sharedMem */
			printf("\n	=== STARTING RECV LOOP | START UP COMMUNICATION ===\n");
			while(1)
			{
				printf("----\n Waiting for consumer...\n");
				fd = open(myfifo, O_RDONLY);						/* Named Pipes used to communicate! */
				sleep(1);
				read(fd, recvBuf, MAX_BUF);							/*  Did Consumer recv data yet? 	*/
				if( strcmp(recvBuf,recvMessage) == 0)				/* 			 Yes! Update			*/ 
				{
					printf("\n	- GOT RECV! -> Clear Buffer! -> Update Buffer!\n");
					if( readFromFile(buffer, argv[1]) == 1)			/*	 Updated data buffer, is EOF?	*/
					{
																	/* Yes, but still need VERY LAST chunk! */
						sprintf( p, "%s", buffer );
						memset( p , 0 , chunkPartition);			/* Make shared mem empty -> ends con*/
					}
					else
					{
						sprintf( p, "%s", buffer );					/* 	Not EOF , Updated shared MEM 	*/
						memset(recvBuf,0,MAX_BUF);					/* 		Reset recvBuf				*/
						printf("\n next chunk :%s\n\n", buffer);
					}
				}
				else if (strcmp(recvBuf,EOFMessage) == 0)			/* Consumer is 100% done, end 		*/
				{
					printf("\n\n			 === EOF! ===\n");
					break;	
				}
			}
		}			
	
	}
	else if (errno = 0, (shmid = shmget( key, 0, 0 )) != -1 )					// FIND ok?
	{
		printf("			 - shmget : FIND - \n");
		errno = 0;
		p = shmat( shmid, 0, 0 );
		if ( p == (void *)-1 )				/* Did shmat work?	*/
		{									/* 		~no~		*/
			printf( "shmat() failed  errno :  %s\n", strerror( errno ) );
			exit( 1 );
		}
		else								/* 		~Yes~		*/
		{
			printf( "Process %d gets message from shared memory segment attached at address %#x.\n", getpid(), p );
			sprintf( p, "%s", buffer );		/* Put into sharedMem */
			printf("\n	=== STARTING RECV LOOP | START UP COMMUNICATION ===\n");
			while(1)
			{
				printf("----\n Waiting for consumer...\n");
				fd = open(myfifo, O_RDONLY);						/* Named Pipes used to communicate! */
				sleep(1);
				read(fd, recvBuf, MAX_BUF);							/*  Did Consumer recv data yet? 	*/
				if( strcmp(recvBuf,recvMessage) == 0)				/* 			 Yes! Update			*/ 
				{
					printf("\n	- Consumer Ready for Next Chunk! -> Clear Buffer! -> Update Buffer!\n");
					if( readFromFile(buffer, argv[1]) == 1)			/*	 Updated data buffer, is EOF?	*/
					{
																	/* Yes, but still need VERY LAST chunk! */
						sprintf( p, "%s", buffer );
						memset( p , 0 , chunkPartition);			/* Make shared mem empty -> ends con*/
					}
					else
					{
						sprintf( p, "%s", buffer );					/* 	Not EOF , Updated shared MEM 	*/
						memset(recvBuf,0,MAX_BUF);					/* 	Reset recvBuf					*/
						printf("\n next chunk :%s\n\n", buffer);
					}
				}
				else if (strcmp(recvBuf,EOFMessage) == 0)			/* Consumer is 100% done, end 		*/
				{
					printf("\n	- Consumer Finished!\n");
					printf("\n\n			 === EOF! ===\n");
					break;	
				}
			}
		}
	}
	else																		// no create, no find
	{
		printf( "\x1b[2;31mBKR shmget() failed  errno :  %s\x1b[0m\n", strerror( errno ) );
		exit( 1 );
	}
	/* === SHARED MEM! =============================== 	*/

	sleep(1);
	close(fd);
	free(buffer);
	shmdt( p );
	printf( "\n\nnormal end. + shmdt(p) worked\n" );
	return 0;
}

