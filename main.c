#include <stdio.h>
#include <string.h>
#include "students.h"
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));
    // need to open all entities
    initStudents();

//    Student* s = loadStudentID(200000);
//    printf("%d, %d, %d, %d, %s, %d\n\n", s->nID, s->d, s->m, s->y, s->sName, s->group);
//    free(s);
//    s = loadStudentIDWithIndex(200000);
//    printf("%d, %d, %d, %d, %s, %d\n\n", s->nID, s->d, s->m, s->y, s->sName, s->group);
//    free(s);

    Student s;
    for (int i = 0; i < 100000; i++) {
        s.d = rand() % 30 + 1;
        s.m = rand() % 11 + 1;
        s.y = rand() % 2000 + 1;
        memset(s.sName, 0, 51);
        strcpy(s.sName, "Alexander");
        s.group = rand() % 99 + 1;
        addStudent(&s);
    }

    //Student s;
    //s.d = 7;
    //s.m = 6;
    //s.y = 1983;
    //memset(s.sName, 0, 51);
    //strcpy(s.sName, "Vitalii");
    //s.group = 32;
    //addStudent(&s);




    // need to close all entities
    closeStudents();
    return 0;
}


