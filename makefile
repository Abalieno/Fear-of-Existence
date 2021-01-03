LIBTCODDIR=G:\msys2\home\Abalieno\foe\libtcod-new
SRCDIR=./src
INCDIR=$(LIBTCODDIR)/src
LIBDIR=$(LIBTCODDIR) 
TEMP=./obj

# I. loads .h in the directory
CFLAGS=-I$(INCDIR) -I$(SRCDIR) -Wall -std=c++11  
CC=gcc
CPP=g++
# wall of errors in the linker were due to using gcc instead of g++
UPX=$(LIBTCODDIR)/dependencies/upx/win/upx.exe
.SUFFIXES: .o .h .c .hpp .cpp

$(TEMP)/%.o : $(SRCDIR)/%.cpp
	$(CPP) $(CFLAGS) -o $@ -c $< 
$(TEMP)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $< 

CPP_OBJS=$(TEMP)/main.o $(TEMP)/fileops.o $(TEMP)/map16.o $(TEMP)/rng.o $(TEMP)/loader.o $(TEMP)/debug.o $(TEMP)/game.o $(TEMP)/liventities.o $(TEMP)/inventory.o $(TEMP)/gui.o $(TEMP)/chargen.o $(TEMP)/screens.o $(TEMP)/armor.o $(TEMP)/busywork.o 

all : main  

main : $(CPP_OBJS) 
	$(CPP) $(CPP_OBJS) -o $@ -L${LIBDIR} -ltcod -static
	#disabling UPX to avoid virus heuristics
	#$(UPX) main.exe
clean :
	rm -f obj/*.o *.exe

