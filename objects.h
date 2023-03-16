#ifndef OBJECTS_H
#define OBJECTS_H

typedef struct note
{
	struct note *next;	
	
	char due_date[11];
	char descr[100];
}
note;

#endif
