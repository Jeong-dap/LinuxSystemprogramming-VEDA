CC = aarch64-linux-gnu-gcc

SRCS = led.c wiring.c
LIB_TARGET = libled.so
MAIN_TARGET = main

all: $(LIB_TARGET) $(MAIN_TARGET)

$(LIB_TARGET): $(SRCS)
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $^ -lwiringPi -lcrypt

$(MAIN_TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $< -ldl

clean:
	rm -f $(LIB_TARGET) $(MAIN_TARGET)

.PHONY: all clean
