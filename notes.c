#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>

#include "constants.h"
#include "objects.h"
#include "file.h"
#include "schedule.h"

bool load();
void load_categories(char category[]);
void update_categories(char category[]);
void add_note_info();
void add_note(char due_date[], char descr[], char category[]);
void add_category(char* category);
void delete_category();
void edit_note();
void delete_note();
void delete_all_notes();


note *head = NULL;
char categories[MAX_CATEGORIES][20];
int notes_num = 0;
int categories_num = 0;

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
		printf("6. DELETE A CATEGORY\n");
		printf("7. EXPORT SCHEDULE\n");
		printf("8. QUIT\n");
		printf("----------------\n\n");
		
		printf("What do you want to do? ");
		scanf("%i", &option);
		
		switch(option)
		{
			case 1:
				system("clear");
				add_note_info();
				system("clear");  
				printf("Note added succesfully.\n\n");
				break;
			case 2:
				system("clear");
				load_from_file();
				printf("\nPress Y to continue.\n");
				scanf("\n");
				int c;
				while ((c = getchar()) != 'y');
				system("clear");
				break;
			case 3:
				system("clear");
				edit_note();
				break;
			case 4:
				system("clear");
				delete_note();
				break;
			case 5:
				system("clear");
				delete_all_notes();
				printf("All notes deleted succesfully.\n\n");
				break;
			case 6:
				system("clear");
				// problematic
				delete_category();
				break;
			case 7:
				calculate_graph_parameters(notes_num, head);
				break;
			case 8:
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
	
	note *temp;
	temp = (note*) malloc(sizeof(note));
	while (fread(temp, sizeof(note), 1, fptr))
	{
		add_note(temp->due_date, temp->descr, temp->category);
		load_categories(temp->category);
		notes_num++;
	}
	fclose(fptr);
	return true;
}

void load_categories(char category[])
{
	int i = 0;
	if (i == 0)
		{
			strcpy(categories[i], category);
		}
		else
		{
			bool flag = true;
			for (int j = 0; j < i; j++)
			{
				if (strcmp(category, categories[i]) == 0)
				{
					flag = false;
					break;
				}
			}
			if (flag)
			{
				strcpy(categories[i], category);
			}
		}
		categories_num++;
		i++;
}

void update_categories(char category[])
{
	if (categories_num > MAX_CATEGORIES)	
	{
		printf("No more categories can be added.\n");
		printf("Reverting...\n");
		return;
	}
	bool flag = true;
	for (int j = 0; j < categories_num; j++)
	{
		if (strcmp(category, categories[j]) == 0)
		{
			flag = false;
			break;
		}
	}
	if (flag)
	{
		strcpy(categories[categories_num], category);
		categories_num++;
	}
}

void add_note_info()
{
	char due_date[11];
	char descr[100];
	char category[20];
	
	printf("\n");
	printf("Give a description.\n");
	scanf("\n");
	fgets(descr, 100, stdin);
	printf("When is it due? (DD/MM/YYYY)\n");
	scanf("\n");
	fgets(due_date, 11, stdin);
	add_category(category);
	add_note(due_date, descr, category);
}

void add_note(char due_date[], char descr[], char category[])
{
	struct note *ptr;
	ptr = (note*) malloc(sizeof(note));

	strcpy(ptr->due_date, due_date);
	strcpy(ptr->descr, descr);
	strcpy(ptr->category, category);
	
	ptr->next = head;
	head = ptr;
	
	save_to_file(head);
}

void add_category(char* category)
{
	printf("Specify its category.\n");
	int i = 1;
	for (i; i < categories_num + 1; i++)	
	{
		printf("%d. %s", i, categories[i-1]);
	}
	printf("%d. Other\n", i);
	int option;
	scanf("%d", &option);
	if (option < 1 || option > i)
	{
		printf("Invalid choice.\n");
	}
	else if (option < i)
	{
		strcpy(category, categories[option-1]);
	}
	else
	{
		printf("Give the category's name.\n");
		scanf("\n");
		fgets(category, 20, stdin);
		update_categories(category);
	}
} 

void delete_category()
{
	for (int i = 0; i < categories_num; i++)
	{
		printf("%d. %s\n", i+1, categories[i]);
	}
	printf("Which category do you want to delete?\n");
	int option;
	scanf("%d", &option);

	for (int i = option - 1; i < categories_num - 1; i++)
	{
		strcpy(categories[i], categories[i+1]);
	}
	strcpy(categories[categories_num-1], "\0");
}

void edit_note()
{
	int count = load_from_file();
	if (count == 0) return;
	printf("Which note do you want to edit?\n");
	int choice;
	scanf("%i", &choice);
	
	if (choice > 0 && choice <= count)
	{
		int count = 0;
		FILE *fptr = fopen(".notes.dat","r");
		FILE *fptw = fopen("notes2.dat","wb");
		note *temp = (note*) malloc(sizeof(note));
		while (fread(temp, sizeof(note), 1, fptr))
		{
			count ++;
			if (count == choice)
			{
				int choice = -1;
				printf("1. Due date\n");
				printf("2. Description\n");
				printf("3. Category\n");
				printf("4. Nothing\n");
				printf("What do you want to edit? ");
				do
				{
					scanf("%i", &choice);
				} while (choice < 1 || choice > 4);
				
				note n;	
				switch (choice)
				{
					case 1:
						char due_date[11];						
						printf("Give new due date. (DD/MM/YYYY)\n");
						scanf("\n");
						fgets(due_date, 11, stdin);	
						strcpy(n.due_date, due_date);
						strcpy(n.descr, temp->descr);
						strcpy(n.category, temp->category);
						break;
					case 2:
						char descr[100];
						printf("Give new description.\n");
						scanf("\n");
						fgets(descr, 100, stdin);
						strcpy(n.due_date, temp->due_date);
						strcpy(n.descr, descr);
						strcpy(n.category, temp->category);
						break;
					case 3:
						char category[20];
						add_category(category);
						strcpy(n.due_date, temp->due_date);
						strcpy(n.descr, temp->descr);
						strcpy(n.category, category);
						break;
					case 4:
						system("clear");
						printf("Reverting...\n\n");
						return;
					default:
				}								
				fwrite(&n, sizeof(note), 1, fptw);
			}
			else
			{
				fwrite(temp, sizeof(note), 1, fptw);
			}
		}
		fclose(fptr);
		fclose(fptw);
		rename("notes2.dat",".notes.dat");
		system("clear");
		printf("Note edited successfully.\n\n");
	}
	else
	{
		system("clear");
		printf("Invalid choice.\n\n");
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

			note *temp;
			temp = (note*) malloc(sizeof(note));
			while (fread(temp, sizeof(note), 1, fptrr))
			{
				i++;
				if (i != choice)
				{
					fwrite(temp, sizeof(note), 1, fptrw);
				}
			}
			
			fclose(fptrr);
			fclose(fptrw);
			rename("notes2.dat",".notes.dat");
			
			int j = 0;
			note *mem_temp = head;
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

			system("clear");
			printf("Note deleted successfully.\n\n");
		}
		else
		{
			system("clear");
			printf("Invalid choice.\n\n");
		}
	}
}

void delete_all_notes()
{
	head = NULL;
	FILE *fptr = fopen(".notes.dat","wb");
	fclose(fptr);
}