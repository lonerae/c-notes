#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "objects.h"
#include "constants.h"

const int month_days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

int calculate_available_segments(struct tm *today, int notes_num);
void update_note_fields(struct tm *today, note *temp);
void make_graph_draft(char schedule[][101], note all_notes[], int segments_per_note, int notes_num);
void make_graph_second_draft(char schedule[][101], note all_notes[], int all_segments, int notes_num);
void make_graph_final(char schedule[][101], note all_notes[], int all_segments, int notes_num);
note* find_note(note all_notes[], int notes_num, char descr[]);
int desc_avail(const void * a, const void * b);
int desc_actual(const void * a, const void * b);

void calculate_graph_parameters(int notes_num, note* head)
{
	if (notes_num == 0)
	{
		system("clear");
		printf("Please add a note first.\n\n");
		return;
	}

    time_t t = time(NULL);
	struct tm today = *localtime(&t);
	
    int all_segments = calculate_available_segments(&today, notes_num) ;
    int segments_per_note = all_segments / notes_num;
	note all_notes[notes_num];
	note *temp = head;
	int i = 0;
	while (temp != NULL)
	{
		update_note_fields(&today, temp);
		all_notes[i] = *temp;
		i++;
		temp = temp->next;
	}

	qsort(all_notes, notes_num, sizeof(all_notes[0]), desc_avail);
	char schedule[all_segments][101];
    for (int i = 0; i < all_segments; i++)
	{
		strcpy(schedule[i]," ");
	}

	make_graph_draft(schedule, all_notes, segments_per_note, notes_num);
    make_graph_second_draft(schedule, all_notes, all_segments, notes_num);
	make_graph_final(schedule, all_notes, all_segments, notes_num);

	FILE *fp = fopen("schedule.txt", "w");
    if (fp) {
        for (int i = 0; i < all_segments; i++)
		{
			fputs(schedule[i],fp);
		}
        fclose(fp);
    }
}

int calculate_available_segments(struct tm *today, int notes_num)
{
   
	int remaining_days = month_days[today->tm_mon] - today->tm_mday;
	for (int i = today->tm_mon + 1; i < 12; i++)
	{
		remaining_days += month_days[i];
	}
	int segments_per_day = 4;
	return remaining_days * segments_per_day;
}

void update_note_fields(struct tm *today, note *temp)
{
	char days[3];
    strncpy(days, temp->due_date, 2);
    char month[3];
    strncpy(month, temp->due_date+3, 2);
    int segments_left = atoi(days);
    segments_left += month_days[today->tm_mon] - today->tm_mday;
    for (int i = today->tm_mon + 1; i < atoi(month) - 1; i++)
    {
        segments_left += month_days[i];
    }
    segments_left *= 4;
    temp->available_segments = segments_left;
    temp->actual_segments = 0;
}

void make_graph_draft(char schedule[][101], note all_notes[], int segments_per_note, int notes_num)
{
	int start_position = 0;
	for (int i = 0; i < notes_num; i++)
	{
		int day_counter = start_position;
		double step = all_notes[i].available_segments / segments_per_note;
		if (step < 1.0) step = 1.0;
		int counter = segments_per_note;
		
		bool flag = false;
		while (counter > 0 && day_counter < all_notes[i].available_segments)
		{	
			if (counter != segments_per_note) // not first loop
			{
				day_counter += step;
				if (!flag)
				{
					for (int j = day_counter - step; j < day_counter; j++)
					{
						if (strcmp(schedule[j]," ") == 0)
						{
							start_position = j;
							flag = true;
							break;
						}
					}	
				}
			}
			if (strcmp(schedule[day_counter]," ") != 0) 
			{
				bool found = false;
				for (int j = day_counter - step; j < day_counter; j++)
				{
					if (strcmp(schedule[j]," ") == 0) 
					{
						strcpy(schedule[j], all_notes[i].descr);
						counter--;
						all_notes[i].actual_segments++;
						found = true;
						break;
					}
				}	
				while (!found && day_counter < all_notes[i].available_segments)
				{
					day_counter++;
					if (strcmp(schedule[day_counter]," ") == 0) 
					{
						strcpy(schedule[day_counter], all_notes[i].descr);
						counter--;
						all_notes[i].actual_segments++;
						break;
					}
				}
			}
			else 
			{
				strcpy(schedule[day_counter], all_notes[i].descr);
				counter--;
				all_notes[i].actual_segments++;
			}
		}
	}
}

void make_graph_second_draft(char schedule[][101], note all_notes[], int all_segments, int notes_num)
{
    qsort(all_notes, notes_num, sizeof(all_notes[0]), desc_actual);
    for (int i = 0; i < all_segments; i++)
	{
		if (strcmp(schedule[i]," ") == 0)
		{
			for (int j = 0; j < notes_num; j++)
			{
				if (i < all_notes[j].available_segments)
				{
					strcpy(schedule[i], all_notes[j].descr);
					all_notes[j].actual_segments++;
					qsort(all_notes, notes_num, sizeof(all_notes[0]), desc_actual);
					break;
				}
			}
		}
	}

}

void make_graph_final(char schedule[][101], note all_notes[], int all_segments, int notes_num)
{
    for (int i = 0; i < notes_num - 1; i++)
    {
        int target = (all_notes[i+1].actual_segments - all_notes[i].actual_segments) / 2;
        printf("%d\n", target);
        for (int j = 0; j < all_notes[i].available_segments; j++)
        {
            bool flag = true;
            for (int k = 0; k <= i; k++)
            {
                if (strcmp(schedule[j], all_notes[k].descr) == 0) flag = false;
            }
            if (flag)
            {
                find_note(all_notes, notes_num, schedule[j])->actual_segments--;
                strcpy(schedule[j], all_notes[i].descr);
                all_notes[i].actual_segments++;
                target--;
            }
            if (target == 0 ) break;
        }
    }
}

note* find_note(note all_notes[], int notes_num, char descr[])
{
    for (int i = 0; i < notes_num; i++)
    {
        if (strcmp(all_notes[i].descr, descr) == 0)
        {
            return &all_notes[i];
        }
    }
    return NULL;
}

int desc_avail(const void * a, const void * b)
{
	return (((note*)a)->available_segments - ((note*)b)->available_segments);
}

int desc_actual(const void * a, const void * b)
{
	return (((note*)a)->actual_segments - ((note*)b)->actual_segments);
}