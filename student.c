#include "students.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE_OF_ARRAY(array) (sizeof(array)/sizeof(array[0]))

#define ITEMS_COUNT_POSITION sizeof(char) + sizeof(int)
#define AUTO_INCREMENT_POSITION sizeof(char)
#define READFILE_CAP 10

const char *g_sStudent_file_name = "students.bdb";
const char *g_sStudent_index_id_name = "students.id.index";

FILE * g_fDBF = NULL;
int *indexID;
int indexIDLen = 0;

void LoadIndexID() {
	indexID = NULL;
	FILE *f = fopen(g_sStudent_index_id_name, "rb");
	if (f == NULL) {
        return;
	}
	int cap = READFILE_CAP;
	int len = 0;
	size_t blocksRead = 0;
	indexIDLen = 0;
	indexID = (int*)malloc(sizeof(int) * cap);
	while (!feof(f))
	{
		if (len == cap) {
			cap += READFILE_CAP;
			int* tmp = (int*)malloc(sizeof(int) * cap);
			memcpy(tmp, indexID, sizeof(int) * len);
			free(indexID);
			indexID = tmp;
		}
		blocksRead = fread(indexID + len, sizeof(int), 1, f);
		if (blocksRead == 0) {
			break;
		}
		len++;
		indexIDLen++;
	}
	fclose(f);
}

void StoreIndexID() {
	FILE *f = fopen(g_sStudent_index_id_name, "wb");
	if (f == NULL) {
		return;
	}
	fwrite(indexID, sizeof(int), indexIDLen, f);
	fclose(f);
}

void initStudents() {
	g_fDBF = fopen(g_sStudent_file_name, "r+b");
	if (g_fDBF == NULL) {
		g_fDBF = fopen(g_sStudent_file_name, "wb");
		fclose(g_fDBF);
		g_fDBF = fopen(g_sStudent_file_name, "r+b");
		if (g_fDBF == NULL) {
			printf("STUDENTS DB FILE OPEN ERROR!!!");
			exit(EXIT_FAILURE);
		}
	}
	char s = '\0';
	fseek(g_fDBF, 0, SEEK_SET);
	
	size_t r = fread(&s, sizeof(char), 1, g_fDBF);
	if (r == 0) {
		// new file! fill its header!
        // db header is 100 bytes length
        // first - char 's' symbol - just for fun
        // second - int AutoIncrement field
        // third - int elements count
        // other 91 bytes is reserved
		s = 's';
		fwrite(&s, sizeof(char), 1, g_fDBF);
		int ai = 1;
		fwrite(&ai, sizeof(int), 1, g_fDBF);
		int cnt = 0;
		fwrite(&cnt, sizeof(int), 1, g_fDBF);
		char reserv[91] = { 0 };
		fwrite(reserv, sizeof(char), 91, g_fDBF);
	}
	else {
		if (s == 's') {
			fseek(g_fDBF, 100, SEEK_SET);
		}
	}
	LoadIndexID();
}

void closeStudents() {
	if (g_fDBF != NULL) {
		fclose(g_fDBF);
	}
	StoreIndexID();
}

void truncateStudents() {
	closeStudents();
	g_fDBF = fopen(g_sStudent_file_name, "wb");
	closeStudents();
	initStudents();
}

int autoIncrement() {
	if (g_fDBF == NULL) {
		return 1;
	}
	int r = fseek(g_fDBF, AUTO_INCREMENT_POSITION, SEEK_SET);
	if (r != 0) {
		printf("students autoIncrement fseek error!!!");
		exit(EXIT_FAILURE);
	}
	int i = 0;
	fread(&i, sizeof(int), 1, g_fDBF);
	r = fseek(g_fDBF, AUTO_INCREMENT_POSITION, SEEK_SET);
	if (r != 0) {
		printf("students autoIncrement fseek error!!!");
		exit(EXIT_FAILURE);
	}
	int ni = i + 1;
	size_t result = fwrite(&ni, sizeof(int), 1, g_fDBF);
	if (result != 1) {
		printf("students autoIncrement FWRITE error!!!");
		exit(EXIT_FAILURE);
	}
	return i;
}

int addItemsCount() {
	if (g_fDBF == NULL) {
		return 1;
	}
	int r = fseek(g_fDBF, ITEMS_COUNT_POSITION, SEEK_SET);
	if (r != 0) {
		printf("students addItemsCount fseek1 error!!!");
		exit(EXIT_FAILURE);
	}
	int cnt = 0;
	fread(&cnt, sizeof(cnt), 1, g_fDBF);
	cnt++;
	r = fseek(g_fDBF, ITEMS_COUNT_POSITION, SEEK_SET);
	if (r != 0) {
		printf("students addItemsCount fseek2 error!!!");
		exit(EXIT_FAILURE);
	}
	fwrite(&cnt, sizeof(cnt), 1, g_fDBF);
	return 0;
}

void addIndexID(int studentID, int position) {
	int *tmp = (int*)malloc(sizeof(int) * (indexIDLen + 1));
	memcpy(tmp, indexID, sizeof(int) * (indexIDLen));
	free(indexID);
	indexID = tmp;
	indexID[studentID - 1] = position;
	indexIDLen++;
}

int addStudent(Student* s) {
	s->nID = autoIncrement();
	int result = fseek(g_fDBF, 0, SEEK_END);
	if (result != 0) {
		return 1;
	}
	long curTell = ftell(g_fDBF);
	result = fwrite(s, sizeof(*s), 1, g_fDBF);
	if (result != 1) {
		return 1;
	}
	// add items_count 
	addItemsCount();
	curTell = (curTell - 100) / sizeof(Student);
	addIndexID(s->nID, curTell);
	return 0;
}



void freeStudent(Student *s) {
	if (s == NULL) {
		return;
	}
	free(s);
}

Student *loadStudentIDWithIndex(int nID) {
	int position = indexID[nID - 1];
	Student *s = (Student*)malloc(sizeof(Student));
	fseek(g_fDBF, 100 + (sizeof(Student) * position), SEEK_SET);
	fread(s, sizeof(Student), 1, g_fDBF);
	return s;
}

Student *loadStudentID(int nID) {
	int iterationForFound = 0;
	Student *s = (Student*)malloc(sizeof(Student));
	fseek(g_fDBF, 100, SEEK_SET);
	int bRead = 0;
	while (!feof(g_fDBF)) {
		iterationForFound++;
		bRead = fread(s, sizeof(Student), 1, g_fDBF);
		if (bRead == 0) {
			break;
		}
		if (s->nID != nID) {
			continue;
		}
		// student found!!!
		printf("DEBUG: iterationForFound: %d\n", iterationForFound);
		return s;
	}
	return NULL;
}
