CC 			:=		gcc
CFLAGS 		:= 		-Wall -pedantic -std=c11

PROG1	:=		main1
SRC1		:=		main1.c
OBJ1		:=		$(patsubst %.c, %.o, $(SRC1))

PROG2	:=		main2
SRC2		:=		main2.c 
OBJ2		:=		$(patsubst %.c, %.o, $(SRC2))

all: $(PROG1) $(PROG2)
	
$(PROG1) : $(OBJ1)
	$(CC) -o $@ $^

$(PROG2) : $(OBJ2)
	$(CC) -o $@ $^

$(OBJ1) : $(SRC1)
	$(CC) $(CFLAGS) -c $^

$(OBJ2) : $(SRC2)
	$(CC) $(CFLAGS) -c $^


.PHONY : clean
clean:
	rm -f *.o $(PROG1) $(PROG2)
