SHELL		:= 		/bin/bash
CC 			:=		gcc
CFLAGS 		:= 		-Wall -pedantic -std=c11

PROG1		:=		gen
SRC1		:=		gen.c
OBJ1		:=		$(patsubst %.c, %.o, $(SRC1))

PROG2		:=		main
SRC2		:=		main.c 
OBJ2		:=		$(patsubst %.c, %.o, $(SRC2))

RESULTS		:=		wyniki.txt


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


.PHONY : test
test: $(PROG1) $(PROG2)
	@./$(PROG1) text.txt 10000 4
	@cp text.txt text1.txt
	@echo "10000 elems, 4 in size, sys" > $(RESULTS)
	@{ time -p ./$(PROG2) text.txt 4 sys ; } 2>> $(RESULTS)
	@echo "10000 elems, 4 in size, lib" >> $(RESULTS)
	@{ time -p ./$(PROG2) text1.txt 4 lib ; } 2>> $(RESULTS)

	@./$(PROG1) text.txt 20000 4
	@cp text.txt text1.txt
	@echo "20000 elems, 4 in size, sys" >> $(RESULTS)
	@{ time -p ./$(PROG2) text.txt 4 sys ; } 2>> $(RESULTS)
	@echo "20000 elems, 4 in size, lib" >> $(RESULTS)
	@{ time -p ./$(PROG2) text1.txt 4 lib ; } 2>> $(RESULTS)

	@./$(PROG1) text.txt 10000 512
	@cp text.txt text1.txt
	@echo "10000 elems, 512 in size, sys" >> $(RESULTS)
	@{ time -p ./$(PROG2) text.txt 512 sys ; } 2>> $(RESULTS)
	@echo "10000 elems, 512 in size, lib" >> $(RESULTS)
	@{ time -p ./$(PROG2) text1.txt 512 lib ; } 2>> $(RESULTS)

	@./$(PROG1) text.txt 20000 512
	@cp text.txt text1.txt
	@echo "20000 elems, 512 in size, sys" >> $(RESULTS)
	@{ time -p ./$(PROG2) text.txt 512 sys ; } 2>> $(RESULTS)
	@echo "20000 elems, 512 in size, lib" >> $(RESULTS)
	@{ time -p ./$(PROG2) text1.txt 512 lib ; } 2>> $(RESULTS)

	@./$(PROG1) text.txt 10000 4096
	@cp text.txt text1.txt
	@echo "10000 elems, 4096 in size, sys" >> $(RESULTS)
	@{ time -p ./$(PROG2) text.txt 4096 sys ; } 2>> $(RESULTS)
	@echo "10000 elems, 4096 in size, lib" >> $(RESULTS)
	@{ time -p ./$(PROG2) text1.txt 4096 lib ; } 2>> $(RESULTS)

	@./$(PROG1) text.txt 15000 4096
	@cp text.txt text1.txt
	@echo "15000 elems, 4096 in size, sys" >> $(RESULTS)
	@{ time -p ./$(PROG2) text.txt 4096 sys ; } 2>> $(RESULTS)
	@echo "15000 elems, 4096 in size, lib" >> $(RESULTS)
	@{ time -p ./$(PROG2) text1.txt 4096 lib ; } 2>> $(RESULTS)

	@./$(PROG1) text.txt 8000 8192
	@cp text.txt text1.txt
	@echo "8000 elems, 8192in size, sys" >> $(RESULTS)
	@{ time -p ./$(PROG2) text.txt 8192 sys ; } 2>> $(RESULTS)
	@echo "8000 elems, 8192 in size, lib" >> $(RESULTS)
	@{ time -p ./$(PROG2) text1.txt 8192 lib ; } 2>> $(RESULTS)

	@./$(PROG1) text.txt 12000 8192
	@cp text.txt text1.txt
	@echo "12000 elems, 8192in size, sys" >> $(RESULTS)
	@{ time -p ./$(PROG2) text.txt 8192 sys ; } 2>> $(RESULTS)
	@echo "12000 elems, 8192 in size, lib" >> $(RESULTS)
	@{ time -p ./$(PROG2) text1.txt 8192 lib ; } 2>> $(RESULTS)
	@rm -f text.txt text1.txt

