OBJS = main.o parsing.o minishell.o is_executable.o child_handler.o
SRCS = $(OBJS:.o=.c)

minishell: $(OBJS)
	$(CC) -o $@ $(OBJS)

.c.o: $(SRCS)
	$(CC) -c $<

clean:
	rm -f $(OBJS) minishell