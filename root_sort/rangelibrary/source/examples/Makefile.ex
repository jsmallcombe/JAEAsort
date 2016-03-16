
CCFLAGS = -g -O -Wall

test: clean passage.c
	gcc $(CCFLAGS) passage.c -lrange -lm -o passage

clean:
	rm -f *~ *.o passage
