#sudo apt-get install libncurses5 libncurses5-dev

HEADERS = emu8051.h  ui.h
CORE_OBJ = core.o  disasm.o  opcodes.o
UI_OBJ = ui_emu.o  ui_logicboard.o  ui_mainview.o  ui_memeditor.o  ui_options.o  ui_popups.o
LIB = libem8051.a


CC = gcc
CCPP = g++
CFLAGS = -Wall -Wextra -Wno-unused-parameter -Wno-unused -O2

%.o: %.c $(HEADERS)
	$(CC) $(CLFLAGS)-c -o $@ $< $(CFLAGS) $(CPPFLAGS)

# %.o: %.cpp $(HEADERS)
# 	$(CCPP) $(CLFLAGS)-c -o $@ $< $(CFLAGS)

all: emu

$(LIB): $(CORE_OBJ)
	$(AR) r $@ $^

emu: $(LIB) $(UI_OBJ)
	$(CC) $(CFLAGS) $^ -o emu -lcurses

clean:
	-rm -rf *.o emu $(LIB)
