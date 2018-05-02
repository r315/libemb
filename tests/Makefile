CC = gcc
GCC =gcc
#####################################################
TARGET =EmulatorTest
SRCPATH =../button ../display ../lcdemulator ../misc
INCPATH =../include
LIBPATH =../lcdemulator
SRCS=emulatorTest.c font8x16.c lcdsdl2.c emulator.c display.c fonts.c lcd.c debug.c
OBJS =$(SRCS:.c=.o)
#####################################################
LIBTARGET =emu
LIBTARGETPATH ="../../lib"
LIBOBJS =lcdsdl.o emulator.o lcd.o fonts.o display.o
#####################################################
VPATH +=$(SRCPATH)

ifeq ($(shell uname -s),Linux)
CFLAGS =-Wall -D__EMU__ -D_SINGLE_FONT
LDFLAGS =-lSDL2 -lSDL2main
else
CFLAGS =-Wall -D__EMU__
INCPATH +="C:/tools/SDL2/include"
LIBPATH +="C:/tools/SDL2/lib" "C:/tools/MinGw/lib"
LDFLAGS = -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2
endif
#####################################################
all : $(TARGET)
	./$(TARGET)	

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(addprefix -I, $(INCPATH)) $(addprefix -L, $(LIBPATH)) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe $(LIBTARGETPATH)/lib$(LIBTARGET).a

.c.o:
	$(CC) $(addprefix -I, $(INCPATH)) $(CFLAGS) -c $< -o ./$@

lib: $(LIBOBJS)
	ar rcs $(LIBTARGETPATH)/lib$(LIBTARGET).a $(LIBOBJS)
	rm -f *.o
#####################################################
	
