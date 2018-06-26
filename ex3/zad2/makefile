CC 			:=		gcc
CFLAGS 		:= 		-Wall -pedantic -std=c11

PROG    	:=		fcounter
SRC 		:=		main.c
OBJ         :=		$(patsubst %.c, %.o, $(SRC))


all: $(PROG)
	
$(PROG) : $(OBJ)
	$(CC) -o $@ $^

$(OBJ) : $(SRC)
	$(CC) $(CFLAGS) -c $^


.PHONY : clean
clean:
	rm -f *.o $(PROG)

