Producer & Consumer
	Anthony See
	as2242@scarletmail.rutgers.edu
	157004252

_______________________________________
This program will demonstrate several topics including shared memory, File IO, and named pipes. PA1 includes two c files called “producer.c” and “consumer.c”, a makefile, and a few text files for the input and output.

- Producer.c : Will produce all the chunks of code for the consumer to grab. 
Feeds in chunks into shared memory then waits for consumer to finish grabbing the data. 
Keeps going untill EOF is reached.

- Consumer.c : Will consume/grab chunks of data inside the shared memory. 
Consumer will also unscramble data then put into a seperate file. It will grab the data, 
tell producer that its finished, and act again if the shared memory gets updated.

- makefile 
These are all the commands in the make file
	all: makes object files for producer + consumer
	pro: makes object file for producer
	con: make object file for consumer
	rpro: runs producer
	rcon: runs consumer

Print variations : These are the same thing as above but include more print statements to see what is happening inside.
	allPrint: Object files for producer + consumer + additional print statements/details 
	proP: Object files for producer + additional print statements/details 
	conP: Object files for consumer +  additional print statements/details 
	rproP: Runs producer
	rconP: Runs consumer

	clean: Cleans up.

- input.txt : A scrambled txt file 
- output.txt : The result unscrambled

  



















