SRCDIR     :=       src
OBJDIR     :=       obj
BUILD       :=       bin

SOURCES     :=      $(SRCDIR)/gen.c
OBJECTS      :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES)))
PROG           :=          $(BUILD)/gen

SOURCESa     :=      $(SRCDIR)/ver_a/main.c
OBJECTSa     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCESa)))
PROGa       :=          $(BUILD)/ver_a/main


SOURCESb     :=      $(SRCDIR)/ver_b/main.c
OBJECTSb     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCESb)))
PROGb       :=          $(BUILD)/ver_b/main


SOURCESc     :=      $(SRCDIR)/ver_c/main.c
OBJECTSc     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCESc)))
PROGc       :=          $(BUILD)/ver_c/main


CC               :=           gcc
CFLAGS      +=          -std=c11
CFLAGS      +=          -pedantic
CFLAGS      +=          -Wall

LFLAGS      +=          -lpthread

all : $(PROGa) $(PROGb) $(PROGc) $(PROG)

$(PROGa) : $(OBJECTSa)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^ $(LFLAGS)

$(PROGb) : $(OBJECTSb)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^ $(LFLAGS)

$(PROGc) : $(OBJECTSc)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^ $(LFLAGS)

$(PROG) : $(OBJECTS)
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
