#include <stdio.h>

#include "UI.h"

void show_logo()
{
    printf(ANSI_COLOR_GREEN);
    printf("\n");	
	printf("██      ████████  ██████ ██████ \n");
	printf("██         ██    ██      ██   ██\n");
	printf("██         ██    ██      ██████ \n");
	printf("██         ██    ██      ██     \n");
	printf("███████    ██     ██████ ██     \n");
	printf("\n");
    printf(ANSI_COLOR_RESET);
}

void show_menu()
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
}