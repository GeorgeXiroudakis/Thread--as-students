//Author: Xiroudakis Georgios - csd5067

### Thread synchronization usning a studyroom.

## students
the stuct student contains all the info we need about a student the thread he belongs to (and after we create it we safe it in his rc),
his state (whether he is in the hall in the stady room of somewhere else), the time he needs to study for,
his prioriry, and his AM(intentifier)

## main
in main we initialize all the Semaphores, the mutexes and the sead for the rand then we print the instractions for the colors and  
the the prompt for the #students, we initalize each of the students with the the starter values and random AM and time time they need to study.
Then we sent each student to study with the according priority(like a priority ticket). Lastly we wait for all the threads we created to finish and we clean up

## hallOfStudyroom function
each tread(student) starts in this function.
firstly ussing a conditional thread we pause each tread that doesn't have the correct priority ticket(afrer a student enters the priority
changes so we broatcast a signal to wake them up and chek again). Then if we have reached the capasity of the study room we have to wait untill it emptys, 
so we sleep with a sem_wait untill the number of the students in reaches 0 again so we sem_post to get treads in until it reaches capasity again.
after we pass those two test and we are in we study for the time each thread needs and then we leave.
