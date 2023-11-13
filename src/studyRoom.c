//Author: Xiroudakis Georgios - csd5067

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include "colors.h"

#define STUDY_ROOM_CAPACITY 8
#define MIN_NUM_OF_STUDENTS 20
#define MAX_NUM_OF_STUDENTS 40
#define MIN_TIME_IN_STUDY_ROOM 5
#define MAX_TIME_IN_STUDY_ROOM 15


void printfStatus();
void *hallOfStudyroom(void *_i);
void cleanUp();

//enum for the diferent states(places) a student can be in
enum studentState {someWhereWeDontCareAbout = 0, hall = 1, studyroom = 2};

struct student{
    pthread_t thread;
    int rc;
    enum studentState state;
    unsigned short int timeHeNeeds;
    int priorityTicket;
    int AM;

}typedef student_t;


//for saving the diferent student(threads)
student_t *students;
int numOfStudents;

//for kepping truck of the students
int NumOfstudensInRoom = 0;
int NumOfstudensInHall = 0;

//for priority
int priorityThatShouldEnter = 0;
pthread_mutex_t priorityMutex;
pthread_cond_t priority_condition;

//for only entering after empty
sem_t emptySem;

//to maniputate the valriables for the room status
pthread_mutex_t roomMutex;

//to make sure the prints dont get interamted mid way
pthread_mutex_t printMutex;


int main(int argc, char **argv){
    //initalize the pthread_cont and the accompanying mutex
    pthread_mutex_init(&priorityMutex, NULL);
    pthread_cond_init(&priority_condition, NULL); 

    sem_init(&emptySem, 0, 0);

    pthread_mutex_init(&roomMutex, NULL);

    pthread_mutex_init(&printMutex, NULL);
    
	
	//initalize the rand to get different values each run
	srand(time(NULL));

	printColorInstuctions();
    printf("press enter to continue"); 
    getchar();
    printf("\n");

    do{
        printf("Plese give the number of students interested in studying in the study room (between %d,%d) > ", MIN_NUM_OF_STUDENTS, MAX_NUM_OF_STUDENTS);
        scanf("%d", &numOfStudents);
    }while( numOfStudents < MIN_NUM_OF_STUDENTS || numOfStudents > MAX_NUM_OF_STUDENTS );
	printf("\n");

    //malloc memmory for students
    students = malloc(sizeof(student_t) * numOfStudents);
    
    //init the students
    for(int i = 0; i < numOfStudents; i++){
        student_t temp;
 
        temp.state = someWhereWeDontCareAbout;
        temp.timeHeNeeds = (rand() % (MAX_TIME_IN_STUDY_ROOM - MIN_TIME_IN_STUDY_ROOM + 1)) + MIN_TIME_IN_STUDY_ROOM;
        temp.AM = (rand() % (9999 - 1000 + 1)) + 1000; 

        students[i] = temp; 
		
		//consol update that a student was born
        cyanOutput();
        pthread_mutex_lock(&printMutex);
        printf("student %d was born\n", students[i].AM);
        pthread_mutex_unlock(&printMutex);
        //resetOutput();
    }
    printf("\n");

    //create the threads(sent students to study room)
    for(int i = 0; i < numOfStudents; i++){
        //set the priority of each student as they go to the study room
        students[i].priorityTicket = i;

		//console update about the student going at the hall to try to get in
        pthread_mutex_lock(&printMutex);
        blueOutput();
        printf("student %d wants to go in the study room and get the priorityTicket %d \n\n", students[i].AM, students[i].priorityTicket);
        //resetOutput();
        pthread_mutex_unlock(&printMutex);

        students[i].state = hall;
        NumOfstudensInHall++;
		//going to hall to try to get in the studyroom
        students[i].rc = pthread_create(&(students[i].thread), NULL, hallOfStudyroom, &students[i].priorityTicket);
    }

    //wait for all threads to finish
    for(int i = 0; i < numOfStudents; i++){
        pthread_join(students[i].thread, NULL);
    }

	cleanUp();
        resetOutput();
    
    return 0;
}


void *hallOfStudyroom(void *_i){
	//get the int from void pointer
    int i = *((int*)_i);

    //Lock the priority Mutex because the prhtread_cond_wait acceses the cond veriables.
    pthread_mutex_lock(&priorityMutex);

    //if it's not our turn to get in we wait.
    //the pthread_cond_wait is usually enclosded in a wille with the condition
    //because the thread somethimg wakes up from other sources other that the broadcast 
    while (students[i].priorityTicket != priorityThatShouldEnter){
        pthread_cond_wait(&priority_condition, &priorityMutex);
    }

    pthread_mutex_unlock(&priorityMutex);

    //if we reach capasity lock the room untill it get empty again and we are woken up(post)
    if(NumOfstudensInRoom == STUDY_ROOM_CAPACITY)sem_wait(&emptySem);

    //now we can enter the study room(our priority and we fill up from empty) 
    
	//lock the room mutex to alter the num of studenst and the priority veriable 
    pthread_mutex_lock(&roomMutex);

    students[i].state = studyroom;
    NumOfstudensInRoom++;
	
	//somone entered so now the priority goes to the next one
    priorityThatShouldEnter++;
	
	//because he left the hall to get in
    NumOfstudensInHall--;

	//conslole update about the student entering the room
    greenOutput();
    printf("Student %d(%d) has entered the studyroom and is going to study for %dsec\n\n", students[i].AM, students[i].priorityTicket, students[i].timeHeNeeds);
    //resetOutput();

    // now the priority has changed so we wake up those who are waiting for the priority
    pthread_cond_broadcast(&priority_condition);

    //print overall status
    purpleOutput();
    printfStatus();
    //resetOutput();
    
    pthread_mutex_unlock(&roomMutex);

    //start studing
    sleep(students[i].timeHeNeeds);

    //after we study
	
    pthread_mutex_lock(&roomMutex);
	
	//student leaves
    students[i].state = someWhereWeDontCareAbout;
    NumOfstudensInRoom--;
   
	//console update abput student leaving the studyroom
    redOutput();
    printf("Student %d(%d) has exited after studing for %dsec\n\n", students[i].AM, students[i].priorityTicket, students[i].timeHeNeeds);
    //resetOutput();

	//print overall status
    yellowOutput();
    printfStatus();
    //resetOutput();

    //if after student left, the room is empy new students can go in
    if(NumOfstudensInRoom == 0)sem_post(&emptySem);

    pthread_mutex_unlock(&roomMutex);

    return NULL;
}


void printfStatus(){
    pthread_mutex_lock(&printMutex);

    //print the status of the studyroom 
    printf("there are %d students in the room ", NumOfstudensInRoom);
    if(NumOfstudensInRoom != 0){
        int numOfPrints = 0;
        printf("and they are AM(priority):\n");
        for(int i = 0; i < numOfStudents; i++){
            if(students[i].state == studyroom){
                numOfPrints++;
                printf("| %d", students[i].AM);
                //if the number is one cahr print one extra spce to be uniform
                printf("(%d) ", students[i].priorityTicket);
                if(students[i].priorityTicket < 10) printf(" ");
            }
        }
        for ( ; numOfPrints < STUDY_ROOM_CAPACITY; numOfPrints++){
            printf("|  empty  ");
        }
        printf("|");
    }

    printf("\n\n");

    //print the status of the hall
    printf("there are %d students in the hall", NumOfstudensInHall);
    if(NumOfstudensInHall != 0){
        int numOfPrints = 0;
        printf("and they are AM(priority):\n");
        for(int i = 0; i < numOfStudents; i++){
            if(students[i].state == hall){
                numOfPrints++;
                printf("| %d", students[i].AM);
                //if the number is one cahr print one extra spce to be uniform
                printf("(%d) ", students[i].priorityTicket);
                if(students[i].priorityTicket < 10) printf(" ");
                if(numOfPrints % 8 == 0)printf("\n");
            }
        }
		printf("|");
    }
    printf("\n\n");
    
    pthread_mutex_unlock(&printMutex);

	return;
}

void cleanUp(){
    free(students);
    pthread_mutex_destroy(&priorityMutex);
    pthread_cond_destroy(&priority_condition); 
    sem_destroy(&emptySem);
    pthread_mutex_destroy(&roomMutex);
    pthread_mutex_destroy(&printMutex);

	return;
}
