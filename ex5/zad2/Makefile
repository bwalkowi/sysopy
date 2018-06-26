SRCDIR     :=       src
OBJDIR     :=       obj
BUILD       :=       bin

SOURCES1     :=      $(SRCDIR)/rdfifo.c
SOURCES2     :=      $(SRCDIR)/wrfifo.c

OBJECTS1     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES1)))
OBJECTS2     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES2)))

PROG1       :=          $(BUILD)/rdfifo
PROG2       :=          $(BUILD)/wrfifo


CC               :=           gcc
CFLAGS      +=          -std=c11
CFLAGS      +=          -pedantic
CFLAGS      +=          -Wall

all : $(PROG1) $(PROG2)

$(PROG1) : $(OBJECTS1)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^

$(PROG2) : $(OBJECTS2)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@echo
	@echo Compiling C: $<
	$(CC) -c $(CFLAGS) $< -o $@ 


.PHONY : clean
clean:
	@echo Cleaning project:
	rm -rf $(OBJDIR)
	rm -rf $(BUILD)

$(shell mkdir -p $(OBJDIR) 2>/dev/null)
$(shell mkdir -p $(BUILD) 2>/dev/null)


