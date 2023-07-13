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
    srand((unsigned) time(&t));
	make_graph_final(schedule, all_notes, all_segments, notes_num);

	FILE *fp = fopen("schedule.txt", "w");
    if (fp) {
        int d = today.tm_mday;
        int m = today.tm_mon;
        d++;
        if (d == month_days[m])
        {
            m++;
            d = 1;
        }
        fprintf(fp, "%d / %d\n\n", d, m + 1);
        for (int i = 0; i < all_segments; i++)
		{
            int rem = i % SEGMENTS_PER_DAY;
            fprintf(fp, "%d", DAY_START + 2 * rem );
            fputs(" - ", fp);
            fprintf(fp, "%d", DAY_START + 2 * (rem + 1));
            fputs(" : ", fp);
            fputs(schedule[i],fp);
            if (rem == 3)
            {
                d++;
                if (d > month_days[m])
                {
                    m++;
                    d = 1;
                }
                if (m < 12) fprintf(fp, "\n%d / %d\n\n", d, m + 1);
            }
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
	return remaining_days * SEGMENTS_PER_DAY;
}

void update_note_fields(struct tm *today, note *temp)
{
	char days[3];
    strncpy(days, temp->due_date, 2);
    char month[3];
    strncpy(month, temp->due_date+3, 2);
    int segments_left = 0;
    if (today->tm_mon == atoi(month) - 1)
    {
        segments_left = atoi(days) - today->tm_mday;
    }
    else
    {
        segments_left = atoi(days);
        segments_left += month_days[today->tm_mon] - today->tm_mday;
        for (int i = today->tm_mon + 1; i < atoi(month) - 1; i++)
        {
            segments_left += month_days[i];
        }
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
		double day_counter = start_position;
		double step = all_notes[i].available_segments / (double) segments_per_note;
		int counter = segments_per_note;
		bool flag = false;
		while (counter > 0 && (int)day_counter + step < all_notes[i].available_segments)
		{	
			if (counter != segments_per_note) // not first loop
			{
				day_counter += step;
				if (!flag)
				{
					for (int j = (int)day_counter - step; j < (int)day_counter; j++)
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
			if (strcmp(schedule[(int)day_counter]," ") != 0) 
			{
				bool found = false;
				for (int j = (int)day_counter - step; j < (int)day_counter; j++)
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
				while (!found && day_counter + 1 < all_notes[i].available_segments)
				{
					day_counter++;
					if (strcmp(schedule[(int)day_counter]," ") == 0) 
					{
						strcpy(schedule[(int)day_counter], all_notes[i].descr);
						counter--;
						all_notes[i].actual_segments++;
						break;
					}
				}
			}
			else 
			{
				strcpy(schedule[(int)day_counter], all_notes[i].descr);
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
    qsort(all_notes, notes_num, sizeof(all_notes[0]), desc_actual);
    int max_distance = (all_notes[notes_num - 1].actual_segments - all_notes[0].actual_segments);
    int guard = 0;
    while (max_distance > 1 && guard < notes_num + 1)
    {
        int goal = max_distance / 2;
        int j = 0;
        while (j < all_notes[0].available_segments && goal != 0)
        {
            if (strcmp(schedule[j], all_notes[notes_num - 1].descr) == 0)
            {
                goal--;
                find_note(all_notes, notes_num, schedule[j])->actual_segments--;
                strcpy(schedule[j], all_notes[0].descr);
                all_notes[0].actual_segments++;
            }            
            j++;
        }
        qsort(all_notes, notes_num, sizeof(all_notes[0]), desc_actual);
        max_distance = (all_notes[notes_num - 1].actual_segments - all_notes[0].actual_segments);
        guard++;
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