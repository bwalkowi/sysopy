OPT           :=       2
SRCDIR     :=       src
OBJDIR     :=       obj
BUILD       :=       bin

SOURCES1     :=      $(SRCDIR)/client.c $(SRCDIR)/ssi.c
OBJECTS1     :=      $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES1)))
PROG1        :=      client

SOURCES2     :=      $(SRCDIR)/server.c $(SRCDIR)/ssi.c
OBJECTS2     :=      $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES2)))
PROG2        :=      server

CC               :=           gcc
CFLAGS      =          -O$(OPT)
CFLAGS      +=          -std=c11
CFLAGS      +=          -pedantic
CFLAGS      +=          -Wall
CFLAGS      +=          -Iinc/ssi.h

LFLAGS      :=          -lpthread

all : $(BUILD)/$(PROG1) $(BUILD)/$(PROG2)


$(BUILD)/$(PROG1) : $(OBJECTS1)
	@echo
	@echo Building executable: $<
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)


$(BUILD)/$(PROG2) : $(OBJECTS2)
	@echo
	@echo Building executable: $<
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)


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

