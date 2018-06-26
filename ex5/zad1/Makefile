OPT           :=       2
SRCDIR     :=       src
OBJDIR     :=       obj
BUILD       :=       bin

SOURCES     :=      $(SRCDIR)/main.c

PROG          :=        main
OBJECTS     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES)))


CC               :=           gcc
CFLAGS      +=          -O$(OPT)
CFLAGS      +=          -std=c11
CFLAGS      +=          -pedantic
CFLAGS      +=          -Wall


$(BUILD)/$(PROG) : $(OBJECTS)
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

