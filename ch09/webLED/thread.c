#include <stdio.h>
#include <dlfcn.h>
#include "thread.h"

#define LIB_FILE "./libled.so"

void* led_thread(void* arg)
{
	void *handle = NULL;
	void (*led_func)(char *);
	char* err_msg;

	handle = dlopen(LIB_FILE, RTLD_LAZY);
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
	void (*cds_func)(int *);
	char* err_msg;

	handle = dlopen(LIB_FILE, RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "dlopen error\n");
		goto err;
	}

	dlerror();

	cds_func = (void (*)(int *))dlsym(handle, "cds_function");

	err_msg = dlerror();

	if (err_msg != NULL) {
		fprintf(stderr, "dlsym error: %s\n", err_msg);
		goto err;
	}

	cds_func((int *)arg);

	dlclose(handle);

	return NULL;
err:
	if (handle) {
		dlclose(handle);
	}

	return NULL;
}
