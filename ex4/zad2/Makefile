SRCDIR     :=       src
OBJDIR     :=       obj
BUILD       :=       bin

SOURCES1a     :=      $(SRCDIR)/ver_a/sender.c
SOURCES1a     +=     $(SRCDIR)/sig_env.c

SOURCES2a     :=      $(SRCDIR)/ver_a/catcher.c
SOURCES2a     +=     $(SRCDIR)/sig_env.c

OBJECTS1a     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES1a)))
OBJECTS2a     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES2a)))

PROG1a       :=          $(BUILD)/ver_a/sender
PROG2a       :=          $(BUILD)/ver_a/catcher


SOURCES1b     :=      $(SRCDIR)/ver_b/sender.c
SOURCES1b     +=     $(SRCDIR)/sig_env.c

SOURCES2b     :=      $(SRCDIR)/ver_b/catcher.c
SOURCES2b     +=     $(SRCDIR)/sig_env.c

OBJECTS1b     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES1b)))
OBJECTS2b     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES2b)))

PROG1b       :=          $(BUILD)/ver_b/sender
PROG2b       :=          $(BUILD)/ver_b/catcher


SOURCES1c     :=      $(SRCDIR)/ver_c/sender.c
SOURCES1c     +=     $(SRCDIR)/sig_env.c

SOURCES2c     :=      $(SRCDIR)/ver_c/catcher.c
SOURCES2c     +=     $(SRCDIR)/sig_env.c

OBJECTS1c     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES1c)))
OBJECTS2c     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES2c)))

PROG1c       :=          $(BUILD)/ver_c/sender
PROG2c       :=          $(BUILD)/ver_c/catcher


CC               :=           gcc
CFLAGS      +=          -std=c11
CFLAGS      +=          -pedantic
CFLAGS      +=          -Wall

CFLAGS      +=          -I./inc

all : $(PROG1a) $(PROG2a) $(PROG1b) $(PROG2b) $(PROG1c) $(PROG2c)

$(PROG1a) : $(OBJECTS1a)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^

$(PROG2a) : $(OBJECTS2a)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^

$(PROG1b) : $(OBJECTS1b)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^

$(PROG2b) : $(OBJECTS2b)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^

$(PROG1c) : $(OBJECTS1c)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^

$(PROG2c) : $(OBJECTS2c)
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

$(shell mkdir -p $(OBJDIR)/ver_a 2>/dev/null)
$(shell mkdir -p $(OBJDIR)/ver_b 2>/dev/null)
$(shell mkdir -p $(OBJDIR)/ver_c 2>/dev/null)

$(shell mkdir -p $(BUILD) 2>/dev/null)
$(shell mkdir -p $(BUILD)/ver_a 2>/dev/null)
$(shell mkdir -p $(BUILD)/ver_b 2>/dev/null)
$(shell mkdir -p $(BUILD)/ver_c 2>/dev/null)

