SRCDIR     :=       src
OBJDIR     :=       obj
BUILD       :=       bin

SOURCESa     :=      $(SRCDIR)/a1/main.c
OBJECTSa     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCESa)))
PROGa       :=          $(BUILD)/a1/main

SOURCESb     :=      $(SRCDIR)/a2/main.c
OBJECTSb     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCESb)))
PROGb       :=          $(BUILD)/a2/main


SOURCESc     :=      $(SRCDIR)/a3/main.c
OBJECTSc     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCESc)))
PROGc       :=          $(BUILD)/a3/main

SOURCESd     :=      $(SRCDIR)/a4/main.c
OBJECTSd     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCESd)))
PROGd       :=          $(BUILD)/a4/main

SOURCESe     :=      $(SRCDIR)/a5/main.c
OBJECTSe     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCESe)))
PROGe          :=          $(BUILD)/a5/main

SOURCESf     :=      $(SRCDIR)/b/main.c
OBJECTSf     :=         $(addprefix $(OBJDIR)/,$(patsubst $(SRCDIR)/%.c, %.o, $(SOURCESf)))
PROGf       :=          $(BUILD)/b/main


CC               :=           gcc
CFLAGS      +=          -std=c11
CFLAGS      +=          -pedantic
CFLAGS      +=          -Wall

LFLAGS      +=          -lpthread

all : $(PROGa) $(PROGb) $(PROGc) $(PROGd) $(PROGe) $(PROGf)

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

$(PROGd) : $(OBJECTSd)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^ $(LFLAGS)

$(PROGe) : $(OBJECTSe)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^ $(LFLAGS)

$(PROGf) : $(OBJECTSf)
	@echo
	@echo Building executable: $<
	$(CC) -o $@ $^ $(LFLAGS)

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

$(shell mkdir -p $(OBJDIR)/a1 2>/dev/null)
$(shell mkdir -p $(OBJDIR)/a2 2>/dev/null)
$(shell mkdir -p $(OBJDIR)/a3 2>/dev/null)
$(shell mkdir -p $(OBJDIR)/a4 2>/dev/null)
$(shell mkdir -p $(OBJDIR)/a5 2>/dev/null)
$(shell mkdir -p $(OBJDIR)/b 2>/dev/null)

$(shell mkdir -p $(BUILD) 2>/dev/null)
$(shell mkdir -p $(BUILD)/a1 2>/dev/null)
$(shell mkdir -p $(BUILD)/a2 2>/dev/null)
$(shell mkdir -p $(BUILD)/a3 2>/dev/null)
$(shell mkdir -p $(BUILD)/a4 2>/dev/null)
$(shell mkdir -p $(BUILD)/a5 2>/dev/null)
$(shell mkdir -p $(BUILD)/b 2>/dev/null)
