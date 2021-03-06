/*
 *	Anthony See
 *	consumer.c
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
	// String Var for my local machine path

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

void swap(char* s, int i)
{
    char tmp = s[i];
    s[i] = s[i-1];
    s[i-1] = tmp;
}

int writeToFile(char* fileString)
{	
	int i;
    
	
	for( i = 0 ; i < chunkPartition ;i++)
	{
		if( i % 2 == 1)
		{
		swap(fileString,i);
		}
	}
	
	FILE *fp;
	fp = fopen("output.txt","a");
	fprintf(fp,fileString,chunkPartition);
	fclose(fp);
	return 0;
}


int main(int argc, char ** argv)
{
	/* === VARS! ======================================	*/
	key_t		key;
	int			shmid;
	char *		p;
	int			size = 393216;							// Referring to shared segment
	
	int 		fd;										// For FIFO / named pipes
    char * 		myfifo = "/tmp/myfifo";					// For FIFO / named pipes
	/* === VARS! ======================================	*/

	
	/* Named Pipes used to communicate! */
	mkfifo(myfifo, 0666);
	fd = open(myfifo, O_WRONLY);
	/* Named Pipes used to communicate! */
	

	/* === SHARED MEM! =============================== 	*/
	
	if ( errno = 0, (key = ftok( "ftokFile.txt", 42 )) == -1 )										// get key!
	{
		printf( "ftok() failed  errno :  %s\n", strerror( errno ) );
		exit( 1 );
	}
	else if (errno = 0, (shmid = shmget( key, size, 0666 | IPC_CREAT | IPC_EXCL )) != -1 )		// create ok?
	{
		errno = 0;
		p = shmat( shmid, 0, 0 );
		if ( p == (void *)-1 )				/* Did shmat work? */
		{
			printf( "shmat() failed  errno :  %s\n", strerror( errno ) );
			exit( 1 );
		}
		else
		{
			printf( "Process %d gets message from shared memory segment attached at address %#x.\n", getpid(), p );
			while(1)
			{
				sleep(1);

				writeToFile(p);
				if (strlen(p) == 0)									/* Anything left in p? 		*/
				{
																	/* Nope! Tell Producer 		*/
					write(fd, EOFMessage , sizeof(recvMessage));
					break;
				}
				if(write(fd, recvMessage, sizeof(recvMessage)) < 0)	/* Ask for next chunk 		*/
				{
					foundError("write\n");
				}
			}
		}
	}
	else if (errno = 0, (shmid = shmget( key, 0, 0 )) != -1 )					// find ok?
	{
		errno = 0;
		p = shmat( shmid, 0, 0 );
		if ( p == (void *)-1 )				/* Did shmat work? */
		{
			printf( "shmat() failed  errno :  %s\n", strerror( errno ) );
			exit( 1 );
		}
		else
		{
			while(1)
			{
				printf("----\n Working...\n");
				sleep(1);
				writeToFile(p);
				if (strlen(p) == 0)									/* Anything left in p? 		*/
				{
																	/* Nope! Tell Producer 		*/
					write(fd, EOFMessage , sizeof(recvMessage));
					break;
				}
				if(write(fd, recvMessage, sizeof(recvMessage)) < 0)	/* Ask for next chunk 		*/
				{
					foundError("write\n");
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
	
	close(fd);
	unlink(myfifo);
	printf(p);
	shmdt( p );
	return 0;
}