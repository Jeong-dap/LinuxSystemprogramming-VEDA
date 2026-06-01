#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <wiringPi.h>

#define	LED	1

static void led_on()
{
	digitalWrite(LED, HIGH);
}

static void led_off()
{
	digitalWrite(LED, LOW);
}

static void led_blink()
{
	digitalWrite(LED, HIGH);
	delay(500);
	digitalWrite(LED, LOW);
	delay(500);
}

struct command_map {
	const char *command;
	void (*action)(void);
};

const static struct command_map lut[] = {
	{"ON", led_on},
	{"OFF", led_off},
	{"BLINK", led_blink}
};

const static int lut_size = sizeof(lut) / sizeof(lut[0]);

void led_function(char* arg)
{
	wiringPiSetup();
	pinMode(LED, OUTPUT);

	for (int i = 0; i < lut_size; i++) {
		if (strcmp(arg, lut[i].command) == 0) {
			lut[i].action();
			return;
		}
	}
}
