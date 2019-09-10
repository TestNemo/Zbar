#-include Rules.make
CC       = gcc
CFLAGS   = -Wall -O -g 
CXXFLAGS = 
INCLUDE  = -I ../include/zbar/include -I ../include/zbar
TARGET   = TestZbar
LIBVAR   = -lzbar -lpthread  #链接 opencv zbar
LIBPATH  = -L ../src/lib
#
vpath %.cpp ../qrdecode/src
vpath %.hpp ../qrdecode/include

OBJS     = main.o 
main.o:main.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $^
	$(CC) $(CFLAGS) $(INCLUDE) -o $(TARGET) $(OBJS) $(LIBPATH) $(LIBVAR)
all:$(OBJS) $(LIB)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(TARGET) $(OBJS) $(LIBPATH) $(LIBVAR)

clean:
	rm -f *.o
	rm -f $(TARGET)
