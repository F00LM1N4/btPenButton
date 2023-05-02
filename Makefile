# Info @ http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Application name
NAME	:= bt-pen-button

# Directories
IDIR 	:= ./
ODIR	:= obj
LDIR 	:= ./

# Compiler
CC	:= gcc
CFLAGS	:= -I$(IDIR) -Wall -g
LIBS	:= -lm -ludev

# Dependancies
_DEPS = 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS)) 

# Objectfiles
_OBJ = main.o  
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# Rules
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(NAME)
	
