compare: compare.o
	gcc -g -std=c99 -Wvla -Wall -o compare compare.o -pthread -lm

compare.o: compare.c wsdstruct.h
	gcc -c -g -std=c99 -Wvla -Wall compare.c -o compare.o -pthread -lm

clean:
	rm -f *.o compare