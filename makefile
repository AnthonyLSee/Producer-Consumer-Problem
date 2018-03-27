CC = gcc
CFLAGS = -w -o

all: producer.c consumer.c
	$(CC) $(CFLAGS) producer producer.c
	$(CC) $(CFLAGS) consumer consumer.c
pro: producer.c
	$(CC) $(CFLAGS) producer producer.c
con: consumer.c
	$(CC) $(CFLAGS) consumer consumer.c
rpro: producer
	./producer input.txt
rcon: consumer
	./consumer

allPrint: producerPRINT.c consumerPRINT.c
	$(CC) $(CFLAGS) pPRINT producerPRINT.c
	$(CC) $(CFLAGS) cPRINT consumerPRINT.c
proP: producerPRINT.c
	$(CC) $(CFLAGS) pPRINT producerPRINT.c
conP: consumerPRINT.c
	$(CC) $(CFLAGS) cPRINT consumerPRINT.c

rproP: pPRINT
	./pPRINT input.txt
rconP: cPRINT
	./cPRINT

clean:
	rm -i producer consumer cPRINT pPRINT
