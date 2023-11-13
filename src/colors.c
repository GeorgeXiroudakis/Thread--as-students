//Author: Xiroudakis Georgios - csd5067

#include <stdio.h>
#include "colors.h"


void printColorInstuctions(){
    printf("\ncolor instractions:\n\n");
    cyanOutput();
    printf("a student is born\n");
    blueOutput();
    printf("a student wants to go in and gets a priority number\n");
    greenOutput();
    printf("a student is going in the study room\n");
	redOutput();
    printf("a student is leaving the study room\n");
	purpleOutput();
    printf("studyroom and hall update after a student entered form the hall to the studyroom\n");
	yellowOutput();
    printf("studyroom and hall update after a student exited the studyroom\n\n");

    resetOutput();
}

void greenOutput(){printf("\033[0;32m");}

void redOutput(){printf("\033[0;31m");}

void purpleOutput(){printf("\033[0;35m");}

void yellowOutput(){printf("\033[0;33m");}

void cyanOutput(){printf("\033[0;36m");}

void blueOutput(){printf("\033[0;34m");}

void resetOutput(){printf("\033[0m");}
