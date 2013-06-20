LIBTCODDIR=D:/foe/libtcod-svn
SRCDIR=.
INCDIR=$(LIBTCODDIR)/include
LIBDIR=$(LIBTCODDIR)/lib 
TEMP=./temp

CFLAGS=-I$(INCDIR) -I$(SRCDIR) -Wall  
CC=mingw32-gcc
CPP=mingw32-g++
.SUFFIXES: .o .h .c .hpp .cpp

$(TEMP)/%.o : $(SRCDIR)/%.cpp
	$(CPP) $(CFLAGS) -o $@ -c $< 
$(TEMP)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $< 

CPP_OBJS=$(TEMP)/main9.o

all : main9

main9 : $(CPP_OBJS)
	$(CPP) $(CPP_OBJS) -o $@ -L${LIBDIR} -ltcod-mingw -static  

clean :
	del .\temp\*.o *.exe

