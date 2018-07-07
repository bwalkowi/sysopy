OPT         :=      2
SRCDIR      :=      src
OBJDIR      :=      obj
BUILD       :=      bin
CC          :=      gcc


CFLAGS      +=      -O$(OPT)
CFLAGS      +=      -std=c11
CFLAGS      +=      -pedantic
CFLAGS      +=      -Wall

CFLAGS      +=      -I./inc/


SOURCES1    :=      $(SRCDIR)/wsh.c $(SRCDIR)/shell.c $(SRCDIR)/ssi.c
OBJECTS1    :=      $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES1)))
TARGET1     :=      $(BUILD)/wsh

SOURCES2    :=      $(SRCDIR)/deam.c $(SRCDIR)/shell.c $(SRCDIR)/ssi.c
OBJECTS2    :=      $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES2)))
TARGET2     :=      $(BUILD)/deam


all : $(TARGET1) $(TARGET2)

$(TARGET1) : $(OBJECTS1)
	@echo Building executable: $<
	$(CC) -o $@ $^

$(TARGET2) : $(OBJECTS2)
	@echo Building executable: $<
	$(CC) -o $@ $^

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@echo Compiling C: $<
	$(CC) -c $(CFLAGS) $< -o $@ 


.PHONY : clean
clean:
	@echo Cleaning project:
	rm -rf $(OBJDIR)
	rm -rf $(BUILD)

$(shell mkdir -p $(OBJDIR) 2>/dev/null)
$(shell mkdir -p $(BUILD) 2>/dev/null)

