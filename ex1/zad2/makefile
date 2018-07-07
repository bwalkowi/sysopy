static:
	#compile main file with library
	#-L - in which directory library is
	#-l - which library add
	gcc -Wall main.c -o main -I../zad1 -L../zad1 -llist1

dynamic:
	gcc main.c -Wall -o main -I../zad1 -L../zad1 -llist2 -Wl,-rpath=`pwd`/../zad1

dll:
	#-ldl for dll library
	#-D DLL - for #ifdef DLL
	gcc -Wall main.c -o main -I../zad1 -L../zad1 -ldl -D DLL_LIB

clean:
	rm -f *.o *.a *.so main

static_tests: static
	./main gen.txt

shared_tests: dynamic
	./main gen.txt

dll_tests: dll
	./main gen.txt


