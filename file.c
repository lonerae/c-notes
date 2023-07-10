#include <stdio.h>
#include <stdlib.h>

#include "objects.h"

void save_to_file(note *head)
{
	note *temp = head;
	if (temp == NULL)
	{
		printf("No data to save.\n");
		return;
	}

	FILE *fptr = fopen(".notes.dat","wb");
	if (fptr == NULL)
	{
		printf("Error opening file.\n");
		return;
	}


	while (temp != NULL)
	{
		fwrite(temp, sizeof(note), 1, fptr);
		temp = temp->next;
	}
	fclose(fptr);
}

int load_from_file()
{	
	FILE *fptr = fopen(".notes.dat","r");
	if (fptr == NULL)
	{
		printf("Error opening file.\n");
		return 0;
	}
	
	int count = 0;
	note *s = (note*) malloc(sizeof(note));

	printf("\n----- NOTES -----\n");
	printf("\n");
	while (fread(s, sizeof(note), 1, fptr))
	{
		count ++;
		printf("%i.\n", count);
		printf("DUE: %s\nDESC: %s\nCATEGORY: %s\n", s->due_date, s->descr, s->category);
	}
	fclose(fptr);
	
	if (count == 0)
	{
		printf("No notes saved.\n");	
	}
	return count;
}

