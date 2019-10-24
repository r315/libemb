GCC =gcc
#####################################################
#TARGET =EmulatorTest
SRCPATH =. ../button ../display ../drv/tft
INCPATH =. ../include $(APP_PATH)
#LIBPATH =../../lib
#SRCS=emulatorTest.c font8x16.c lcdsdl2.c emulator.c #lcd.c
#OBJS =$(SRCS:.c=.o)
#####################################################
LIBTARGET =emu
LIBTARGETPATH =$(PWD)
OBJS =lcdsdl2.o emulator.o lcd.o font.o display.o $(EXT_OBJS)
#####################################################
VPATH +=$(SRCPATH) $(APP_PATH)

ifeq ($(shell uname -s),Linux)
CFLAGS =-Wall -D__EMU__ $(addprefix -I ,$(INCPATH)) $(shell sdl2-config --cflags)
LDFLAGS =$(shell sdl2-config --libs) -lSDL2main
else
CFLAGS =-Wall -D__EMU__
INCPATH +="C:/tools/SDL2/include"
LIBPATH +=-L"C:/tools/SDL2/lib" -L"C:/tools/MinGw/lib"
LDFLAGS = -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2
endif
#####################################################
lib: $(OBJS)
	ar rcs $(LIBTARGETPATH)/lib$(LIBTARGET).a $(OBJS)
	rm -f *.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LIBPATH) -o $(TARGET) $(LDFLAGS)
	./$(TARGET) 

clean:
	rm -f *.o $(TARGET) $(TARGET).exe $(LIBTARGETPATH)/lib$(LIBTARGET).a

.c.o:
	$(CC) $(CFLAGS) -c $< -o ./$@

test:
	$(MAKE) EXT_OBJS="font8x16.o emulatorTest.o util.o" APP_PATH=". ../misc ../display" TARGET=emuTest emuTest
#####################################################
	
