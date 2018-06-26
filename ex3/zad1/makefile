CC 			    :=		    gcc
CFLAGS 		:= 		-Wall -pedantic -std=c11

PROG    		:=		main
SRC         		:=		main.c
OBJ         		:=		$(patsubst %.c, %.o, $(SRC))

RESULTS		:=		wyniki.txt


all: $(PROG)
	
$(PROG) : $(OBJ)
	$(CC) -o $@ $^

$(OBJ) : $(SRC)
	$(CC) $(CFLAGS) -c $^


.PHONY : clean
clean:
	rm -f *.o $(PROG)


.PHONY : test
test: $(PROG)
	@./$(PROG) 1000000 fork > $(RESULTS)
	@./$(PROG) 1000000 vfork >> $(RESULTS)
	@./$(PROG) 1000000 clone >> $(RESULTS)
	@./$(PROG) 1000000 vclone >> $(RESULTS)

	@./$(PROG) 1500000 fork >> $(RESULTS)
	@./$(PROG) 1500000 vfork >> $(RESULTS)
	@./$(PROG) 1500000 clone >> $(RESULTS)
	@./$(PROG) 1500000 vclone >> $(RESULTS)

	@./$(PROG) 2000000 fork >> $(RESULTS)
	@./$(PROG) 2000000 vfork >> $(RESULTS)
	@./$(PROG) 2000000 clone >> $(RESULTS)
	@./$(PROG) 2000000 vclone >> $(RESULTS)

	@./$(PROG) 3000000 fork >> $(RESULTS)
	@./$(PROG) 3000000 vfork >> $(RESULTS)
	@./$(PROG) 3000000 clone >> $(RESULTS)
	@./$(PROG) 3000000 vclone >> $(RESULTS)

