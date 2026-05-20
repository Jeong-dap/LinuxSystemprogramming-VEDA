OBJS = main.o parsing.o isbuiltin.o isexecutable.o child_handler.o
SRCS = $(OBJS:.o=.c)
myshell: $(OBJS)
	$(CC) -o $@ $(OBJS)
%.o: %.c
	$(CC) -c $<
clean:
	rm -f $(OBJS) myshell
