LIBTCODDIR=D:/foe/libtcod-svn
SRCDIR=.
INCDIR=$(LIBTCODDIR)/include
LIBDIR=$(LIBTCODDIR)/lib 
TEMP=./temp

# I. loads .h in the directory
CFLAGS=-I$(INCDIR) -I$(SRCDIR) -Wall -std=c++11 
CC=mingw32-gcc
CPP=mingw32-g++
.SUFFIXES: .o .h .c .hpp .cpp

$(TEMP)/%.o : $(SRCDIR)/%.cpp
	$(CPP) $(CFLAGS) -o $@ -c $< 
$(TEMP)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $< 

CPP_OBJS=$(TEMP)/main.o $(TEMP)/fileops.o $(TEMP)/map16.o $(TEMP)/rng.o

all : main  

main : $(CPP_OBJS) 
	$(CPP) $(CPP_OBJS) -o $@ -L${LIBDIR} -ltcod-mingw -static  

clean :
	del .\temp\*.o *.exe

