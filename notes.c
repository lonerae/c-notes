#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "objects.h"
#include "file.h"

#define MAX_OPTIONS 6

bool load();
void add_note_info();
void add_note(char due_date[], char descr[]);
void edit_note();
void delete_note();
void delete_all_notes();

struct note *head = NULL;

int main()
{
	printf("LOADING...\n");
	bool suc = load();
	if (!suc)
	{
		FILE *fptr = fopen(".notes.dat","wb");
		fclose(fptr);
		printf("Save file created!\n");
	}

	printf("\n");	
	printf("********************************************\n");
	printf("* #####      #   # ##### ##### ##### ##### *\n");
	printf("* #          ##  # #   #   #   #     #     *\n");
	printf("* #     ###  # # # #   #   #   ####  ##### *\n");
	printf("* #          #  ## #   #   #   #         # *\n");
	printf("* #####      #   # #####   #   ##### ##### *\n");
	printf("********************************************\n");
	printf("\n");
	
	int option = -1;
	do
	{
		printf("----- MENU -----\n");
		printf("1. ADD A NOTE\n");
		printf("2. PRINT ALL NOTES\n");
		printf("3. EDIT A NOTE\n");
		printf("4. DELETE A NOTE\n");
		printf("5. DELETE ALL NOTES\n");
		printf("6. QUIT\n");
		printf("----------------\n\n");
		
		printf("What do you want to do? ");
		scanf("%i", &option);
		
		switch(option)
		{
			case 1:
				add_note_info();
				break;
			case 2:
				load_from_file();
				break;
			case 3:
				edit_note();
				break;
			case 4:
				delete_note();
				break;
			case 5:
				delete_all_notes();
				break;
			case 6:
				printf("Bye bye!\n");			
				printf("Shutting down C-NOTES...\n");
				sleep(1);
				break;
			default:
				printf("Give an option between 1 and %i.\n\n", MAX_OPTIONS);
			
		}
	} while (option != MAX_OPTIONS);	
}

bool load()
{
	FILE *fptr = fopen(".notes.dat","r");
	if (fptr == NULL)
	{
		return false;
	}
	
	struct note *temp;
	temp = (struct note*) malloc(sizeof(struct note));
	while (fread(temp, sizeof(struct note), 1, fptr))
	{
		add_note(temp->due_date, temp->descr);
	}
	fclose(fptr);
	return true;
}

void add_note_info()
{
	char due_date[11];
	char descr[100];
	
	printf("\n");
	printf("Give a description.\n");
	scanf("\n");
	fgets(descr, 100, stdin);
	printf("When is it due? (DD/MM/YYYY)\n");
	scanf("\n");
	fgets(due_date, 11, stdin);
		
	add_note(due_date, descr);
}

void add_note(char due_date[], char descr[])
{
	struct note *ptr;
	ptr = (struct note*) malloc(sizeof(struct note));

	strcpy(ptr->due_date, due_date);
	strcpy(ptr->descr, descr);
	
	ptr->next = head;
	head = ptr;
	
	save_to_file(head);
}

void edit_note()
{
	int count = load_from_file();
	printf("Which note do you want to edit?\n");
	int choice;
	scanf("%i", &choice);
	
	if (choice > 0 && choice <= count)
	{
		int count = 0;
		FILE *fptr = fopen(".notes.dat","r");
		FILE *fptw = fopen("notes2.dat","wb");
		struct note *temp = (struct note*) malloc(sizeof(struct note));
		while (fread(temp, sizeof(struct note), 1, fptr))
		{
			count ++;
			if (count == choice)
			{
				int choice = -1;
				printf("1. Due date\n");
				printf("2. Description\n");
				printf("3. Nothing\n");
				printf("What do you want to edit? ");
				do
				{
					scanf("%i", &choice);
				} while (choice < 1 || choice > 3);
				
				struct note n;	
				switch (choice)
				{
					case 1:
						char due_date[11];						
						printf("Give new due date. (DD/MM/YYYY)\n");
						scanf("\n");
						fgets(due_date, 11, stdin);	
						strcpy(n.due_date, due_date);
						strcpy(n.descr, temp->descr);
						break;
					case 2:
						char descr[100];
						printf("Give new description.\n");
						scanf("\n");
						fgets(descr, 100, stdin);
						strcpy(n.due_date, temp->due_date);
						strcpy(n.descr, descr);
						break;
					case 3:
						printf("Reverting...\n");
						return;
					default:
				}								
				fwrite(&n, sizeof(struct note), 1, fptw);
			}
			else
			{
				fwrite(temp, sizeof(struct note), 1, fptw);
			}
		}
		fclose(fptr);
		fclose(fptw);
		rename("notes2.dat",".notes.dat");
		printf("\n");
	}
	else
	{
		printf("Invalid choice.\n");
	}
}

void delete_note()
{
	int count = load_from_file();
	if (count != 0)
	{
		printf("Which note do you want to delete?\n");
		int choice;
		scanf("%i", &choice);
		if (choice > 0 && choice <= count)
		{
			FILE *fptrr = fopen(".notes.dat","r");
			FILE *fptrw = fopen("notes2.dat","wb");
			int i = 0;

			struct note *temp;
			temp = (struct note*) malloc(sizeof(struct note));
			while (fread(temp, sizeof(struct note), 1, fptrr))
			{
				i++;
				if (i != choice)
				{
					fwrite(temp, sizeof(struct note), 1, fptrw);
				}
			}
			
			fclose(fptrr);
			fclose(fptrw);
			rename("notes2.dat",".notes.dat");
			
			int j = 0;
			struct note *mem_temp = head;
			while (mem_temp != NULL)
			{
				j++;
				if (choice == 1)
				{
					head = head->next;
				}
				else if (j == choice - 1)
				{
					mem_temp->next = mem_temp->next->next;
				}	
				mem_temp = mem_temp->next;	
			}
		}
		else
		{
			printf("Invalid choice.\n");
		}
	}
}

void delete_all_notes()
{
	head = NULL;
	FILE *fptr = fopen(".notes.dat","wb");
	fclose(fptr);
	printf("All notes deleted!\n");
}



















