#include <wiringPi.h>
#include <string.h>
#include <stdio.h>

#define SW	5
#define CDS	0
#define LED	1

void cds_function(char *arg)
{
	int i;
	wiringPiSetup();

	pinMode(SW, INPUT);
	pinMode(CDS, INPUT);
	pinMode(LED, OUTPUT);

	if (strcmp(arg, "ON") == 0) {
		printf("CDS Control On..\n");

		while (1) {
			if (digitalRead(CDS) == HIGH) {
				digitalWrite(LED, HIGH);
				delay(1000);
				digitalWrite(LED, LOW);
			}
			delay(500);
		}
	}
	else if (strcmp(arg, "OFF") == 0) {
		printf("CDS Control Off..\n");
	}
}
