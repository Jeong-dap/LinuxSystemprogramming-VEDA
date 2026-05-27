CC = aarch64-linux-gnu-gcc
LDLIBS = -lwiringPi -lcrypt
LIBOBJS = led.o
SENDOBJS = main libcontrol.so

all: main send

%.o: %.c
	$(CC) -c -fPIC $<

libcontrol.so: $(LIBOBJS)
	$(CC) -shared -o libcontrol.so *.o $(LDLIBS)

main: main.c libcontrol.so
	$(CC) main.c -o main -ldl

send: $(SENDOBJS)
	scp $(SENDOBJS) njd990603@127.20.33.119:/home/njd990603/

clean:
	rm -f *.o *.so main