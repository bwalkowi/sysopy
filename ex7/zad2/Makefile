SRCDIR     :=       src
OBJDIR     :=       obj
BUILD       :=       bin

SOURCES1    :=      $(SRCDIR)/writer.c
SOURCES1    +=      $(SRCDIR)/lib.c

SOURCES2    :=      $(SRCDIR)/reader.c
SOURCES2    +=      $(SRCDIR)/lib.c

SOURCES3    :=      $(SRCDIR)/main.c
SOURCES3    +=      $(SRCDIR)/lib.c

OBJECTS1     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES1)))
OBJECTS2     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES2)))
OBJECTS3     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES3)))

PROG1       :=          $(BUILD)/writer
PROG2       :=          $(BUILD)/reader
PROG3       :=          $(BUILD)/main


CC          :=           gcc
CFLAGS      +=          -std=c11
CFLAGS      +=          -Wall

CFLAGS      +=          -I./inc

LDFLAGS      :=          -pthread
LDFLAGS      +=          -lrt


all : $(PROG1) $(PROG2) $(PROG3) 

$(PROG1) : $(OBJECTS1)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^ $(LDFLAGS)

$(PROG2) : $(OBJECTS2)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^ $(LDFLAGS)

$(PROG3) : $(OBJECTS3)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^ $(LDFLAGS)


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


