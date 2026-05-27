/*
 * blink.c:
 *	Standard "blink" program in wiringPi. Blinks an LED connected
 *	to the first GPIO pin.
 *
 * Copyright (c) 2012-2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *      https://github.com/WiringPi/WiringPi
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.

void * led_thread(void *arg) {
  printf("[THREAD] ENTRY POINT\n");

  void *handle;
  void (*function_pointer)(char *);
  char *error;

  handle = dlopen("./libcontrol.so", RTLD_LAZY);
  if(!handle){
    fprintf(stderr, "%s\n", dlerror());
    exit(EXIT_FAILURE);
  }
  dlerror(); // clear error

  char * func_name = "led_function";
  function_pointer = dlsym(handle, func_name);
  error = dlerror();
  if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
  }    
  printf("[CALLING LED FUNCTION]\n");
  function_pointer((char*)arg);
	dlclose(handle);

  return NULL;

}

int main (int argc, char * argv[])
{
  if (argc!=2) {
    fprintf(stderr, "%d, [ON | OFF] \n", argc);
  }

  printf ("Raspberry Pi MAIN\n");

  pthread_t tid;
  if (pthread_create(&tid, NULL, led_thread, argv[1]) != 0) {
    perror("pthread_create");
    exit(1);
  }

  pthread_join(tid, NULL);  
  return 0 ;
}
