SRCDIR     :=       src
OBJDIR     :=       obj
BUILD       :=       bin

SOURCES1    :=      $(SRCDIR)/server.c
SOURCES2    :=      $(SRCDIR)/client.c

OBJECTS1     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES1)))
OBJECTS2     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES2)))

PROG1       :=          $(BUILD)/server
PROG2       :=          $(BUILD)/client



CC               :=           gcc
CFLAGS      +=          -std=c11
CFLAGS      +=          -Wall

CFLAGS      +=          -I./inc

LDFLAGS      :=          -lrt
LDFLAGS      +=          -lm


all : $(PROG1) $(PROG2) 

$(PROG1) : $(OBJECTS1)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^ $(LDFLAGS)

$(PROG2) : $(OBJECTS2)
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


