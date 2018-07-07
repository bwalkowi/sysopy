all: static dynamic

static:
	gcc -Wall -std=c11 -c list.c sort.c
	ar crs liblist1.a list.o sort.o

dynamic:
	gcc -Wall -std=c11 -c -fPIC list.c sort.c
	#-shared -fPIC - for dynamic library
	#-o - library file and .o files
	gcc -Wall -std=c11 -fPIC -shared -o liblist2.so list.o sort.o

clean:
	rm -f *.o *.a *.so
