LIBTCODDIR=D:/foe/libtcod-svn
SRCDIR=./src
INCDIR=$(LIBTCODDIR)/include
LIBDIR=$(LIBTCODDIR)/lib 
TEMP=./obj

# I. loads .h in the directory
CFLAGS=-I$(INCDIR) -I$(SRCDIR) -Wall -std=c++11 
CC=gcc
CPP=g++
UPX=$(LIBTCODDIR)/dependencies/upx/win/upx.exe
.SUFFIXES: .o .h .c .hpp .cpp

$(TEMP)/%.o : $(SRCDIR)/%.cpp
	$(CPP) $(CFLAGS) -o $@ -c $< 
$(TEMP)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $< 

CPP_OBJS=$(TEMP)/main.o $(TEMP)/fileops.o $(TEMP)/map16.o $(TEMP)/rng.o $(TEMP)/loader.o $(TEMP)/debug.o $(TEMP)/game.o $(TEMP)/liventities.o $(TEMP)/inventory.o $(TEMP)/gui.o $(TEMP)/chargen.o 

all : main  

main : $(CPP_OBJS) 
	$(CPP) $(CPP_OBJS) -o $@ -L${LIBDIR} -ltcod-mingw -static
	$(UPX) main.exe
clean :
	rm -f obj/*.o *.exe

