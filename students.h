#ifndef STUDENTS_H
#define STUDENTS_H

//Entity student
struct _student {
	int nID; // AUTO_INCREMENT - exists in any sql database
	char sName[51];
	int d, m, y;
	int group;
	//int iterationForFound;
};

typedef struct _student Student;

void initStudents();
void closeStudents();
void truncateStudents();

void LoadIndexID();
void StoreIndexID();

// let's CRUD

// add student to database
int addStudent(Student*); // 0 - ok
int updateStudent(Student*);
int removeStudent(int);
Student *loadStudentIDWithIndex(int);
Student *loadStudentID(int);
Student *loadStudentName(char[51]);
Student *loadStudentGroup(int);

void freeStudent(Student*);

#endif
