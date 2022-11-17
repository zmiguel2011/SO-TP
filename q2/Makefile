CC = gcc
CFLAGS = -Wall
OBJS = q2.o
EXEC = txt2epub

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OBJS) $(EXEC)