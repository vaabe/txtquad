#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/inotify.h>

#define EVENT_SIZE ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN ( 1024 * ( EVENT_SIZE + 16 ) )

void watcher()
{
	int fd; 
	int wd; 
	int i = 0; 
	char buffer[EVENT_BUF_LEN]; 
	int length; 

	fd = inotify_init(); 
	if (fd < 0) {
		perror("inotify_init"); 
	}

	wd = inotify_add_watch(fd, "./", IN_MODIFY); 
	length = read(fd, buffer, EVENT_BUF_LEN); 

	while(i < length) {
		struct inotify_event *event = (struct inotify_event *) &buffer[i];
		if (event->mask & IN_MODIFY) {
			printf("file %s changed!\n", event->name); 
		}
		i += EVENT_SIZE + event->len; 
	}

	(void) inotify_rm_watch(fd, wd); 
	(void) close(fd); 
}

