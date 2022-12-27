
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CIRCULAR_BUFFER_H
#define	CIRCULAR_BUFFER_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define CIRCULAR_BUFFER_SIZE 15

typedef struct {
    char buffer[CIRCULAR_BUFFER_SIZE];
    int readIndex;
    int writeIndex;
} circular_buffer_t;

void write_cb(volatile circular_buffer_t* cb, char byte);
void read_cb(volatile circular_buffer_t* cb, char* byte);
int avl_bytes(volatile circular_buffer_t* cb);

#endif	/* CIRCULAR_BUFFER_H */

