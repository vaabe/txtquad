#ifndef WATCH_H
#define WATCH_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>

#define EVENT_SIZE ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN ( 1024 * ( EVENT_SIZE + 16 ) )

int modded; 
pthread_t watch; 
int watchthread; 

void *watch_file(void *arg)
{
	int fd, wd, event;  
	char buffer[EVENT_BUF_LEN]; 

	fd = inotify_init(); 
	if (fd < 0) {
		perror("inotify_init"); 
	}

	wd = inotify_add_watch(fd, "./examples/prose", IN_MODIFY); 
	event = read(fd, buffer, EVENT_BUF_LEN); 
	modded = 1; 
	pthread_exit(NULL); 
}

#endif
