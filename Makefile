#Author: Xiroudakis Georgios - csd5067

CC = gcc
CFLAGS = -pthread -Wall -pedantic -g

all: studyRoom

studyRoom: src/studyRoom.o src/colors.o src/colors.h
	$(CC) $(CFLAGS) $^ -o $@

studyRoom.o: src/studyRoom.c src/colors.h
	$(CC)  $(CFLAGS) -c $^ 
	
colors.o: src/colors.c src/colors.h
	$(CC) $(CFLAGS) -c $^

clean:
	-rm -f studyRoom src/studyRoom.o src/colors.o src/*.gch
