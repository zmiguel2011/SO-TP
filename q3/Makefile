CC = gcc
CFLAGS = -Wall
OBJS = q3.o
EXEC = tokenring

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

.PHONY : clean
clean :
	-rm $(OBJS) $(EXEC)
	-rm -rf pipe*