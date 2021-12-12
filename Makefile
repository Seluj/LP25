CC=gcc
CFLAGS=-O2 -Wall

all: main

main: main.c database.o utils.o sql.o check.o table.o expand.o query_exec.o record_list.o
	$(CC) $(CFLAGS) -o main $^

%o: %c %h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o main