SRCDIR     :=       src
OBJDIR     :=       obj
BUILD       :=       bin

SOURCES1    :=      $(SRCDIR)/prod.c
SOURCES1    +=      $(SRCDIR)/svsem.c
SOURCES1    +=      $(SRCDIR)/svshm.c

SOURCES2    :=      $(SRCDIR)/cons.c
SOURCES2    +=      $(SRCDIR)/svsem.c
SOURCES2    +=      $(SRCDIR)/svshm.c

SOURCES3    :=      $(SRCDIR)/main.c
SOURCES3    +=      $(SRCDIR)/svsem.c
SOURCES3    +=      $(SRCDIR)/svshm.c

OBJECTS1     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES1)))
OBJECTS2     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES2)))
OBJECTS3     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES3)))

PROG1       :=          $(BUILD)/prod
PROG2       :=          $(BUILD)/cons
PROG3       :=          $(BUILD)/main



CC          :=           gcc
CFLAGS      +=          -std=c11
CFLAGS      +=          -Wall

CFLAGS      +=          -I./inc


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


