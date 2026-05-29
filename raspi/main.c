#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <dlfcn.h>

void* led_thread(void* arg)
{
	void *handle = NULL;
	void (*led_func)(char *);
	char* err_msg;

	handle = dlopen("./libled.so", RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "dlopen error\n");
		goto err;
	}

	dlerror();

	led_func = (void (*)(char *))dlsym(handle, "led_function");

	err_msg = dlerror();

	if (err_msg != NULL) {
		fprintf(stderr, "dlsym error: %s\n", err_msg);
		goto err;
	}

	led_func((char *)arg);

	dlclose(handle);

	return NULL;
err:
	if (handle) {
		dlclose(handle);
	}

	return NULL;
}

void* cds_thread(void* arg)
{
	void *handle = NULL;
	void (*cds_func)(char *);
	char* err_msg;

	handle = dlopen("./libled.so", RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "dlopen error\n");
		goto err;
	}

	dlerror();

	cds_func = (void (*)(char *))dlsym(handle, "cds_function");

	err_msg = dlerror();

	if (err_msg != NULL) {
		fprintf(stderr, "dlsym error: %s\n", err_msg);
		goto err;
	}

	cds_func((char *)arg);

	dlclose(handle);

	return NULL;
err:
	if (handle) {
		dlclose(handle);
	}

	return NULL;
}

void print_menu(void)
{
	printf("\n===============================\n");
	printf("1. LED\n");
	printf("2. CDS\n");
	printf("3. EXIT\n");
	printf("===============================\n");
	printf("Input > ");
}

int main (int argc, char **argv)
{
	int menu;
	char buf[80];
	pthread_t tid, tid2;

	while (1) {
		print_menu();	
		scanf("%d", &menu);

		if (menu == 1) {
			while (1) {
				printf("Usage: [ON | OFF | BLINK | Q]\n");

				printf("LED Input> ");
				memset(buf, 0, sizeof(buf));
				scanf("%s", buf);

				if (strcmp(buf, "Q") == 0 || strcmp(buf, "q") == 0) {
					break;
				}

				if (pthread_create(&tid, NULL, led_thread, buf)) {
					perror("pthread_create");
					exit(-1);
				}

				pthread_join(tid, (void **)NULL);
			}
		}
		else if (menu == 2) {
			printf("Usage: [ON | OFF]\n");

			printf("CDS Input> ");
			memset(buf, 0, sizeof(buf));
			scanf("%s", buf);

			if (strcmp(buf, "OFF") == 0) {
				pthread_cancel(tid2);
			}
			else {
				if (pthread_create(&tid2, NULL, cds_thread, buf)) {
					perror("pthread_create");
					exit(-1);
				}

				pthread_detach(tid2);
			}
		}
		else if (menu == 3) {
			printf("Exit...\n");
			exit(0);
		}
		else {
			printf("\nSelect mode only [1 | 2 | 3]\n");
		}
	}

	return 0;
}
