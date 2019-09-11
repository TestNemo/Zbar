-include Rules.make
#CC        = gcc 
CFLAGS    = --thumb --cpu ARM7EJ-S --littleend   --split_sections --c99
CXXFLAGS  = 
INCLUDE   = -I ../include/zbar/include -I ../include/zbar/include/decoder -I ../include/ -I ../include/zbar
TARGET    = scan_image.exe
LIBPATH   = ./lib/
OBJ = ./obj

vpath %.h ../include

SRCS       = $(wildcard *.c)
OBJS       = $(SRCS:.c=.o)
$(OBJS):$(SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -c $^
	$(AR) -r $(TARGET) *.o          
	mv $(TARGET) $(LIBPATH)
	mv *.o $(OBJ)
clean:
	rm -f $(OBJ)/*
	rm -f $(LIBPATH)*
	rm -f *.o
