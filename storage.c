#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/*
definition of storage cell structure ----
members :
int building : building number of the destination
int room : room number of the destination
int cnt : number of packages in the cell. �� ���� ��Ű�� ��. 0�̸� ���.
char passwd[] : password setting (4 characters)
char *contents : package context (message string)
*/
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN + 1];
	char *context;
} storage_t;


static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied ���ִ� ĭ��
static int systemSize[2] = { 0, 0 };  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN + 1];	//master password


											// ------- inner functions ---------------

											//print the inside context of a specific cell
											//int x, int y : cell to print the context
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized

static void initStorage(int x, int y) {//������ �ϳ��� �ʱ�ȭ
	
	free(deliverySystem[x]+y);//free
	deliverySystem[x][y].cnt = 0;
	deliverySystem[x][y].building = 0;
	deliverySystem[x][y].room = 0;
	deliverySystem[x][y].context = NULL;
	*deliverySystem[x][y].passwd = 0; //�迭 ���� ������ 0�� �ǰ�.


}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching,/ -1 - password is not matching

static int inputPasswd(int x, int y) {
	printf(" - input password for (%d, %d) storage : ", x, y);
	char inputPasswd[PASSWD_LEN + 1];
	scanf("%s", inputPasswd);
	if ((strcmp(deliverySystem[x][y].passwd, inputPasswd) == 0) || (strcmp(inputPasswd, masterPassword) == 0))
		return 0;
	
	else return -1;
}



// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup

int str_backupSystem(char* filepath) { //���� �����Ե��� ���� �� ���� ������ ���Ͽ� ����
	//���Ͽ� x y building room ��� �޼���
	FILE *fp;
	fp = fopen(filepath, "w"); //for�� ������ �ù� ���ִ°� �ϴ� ��������.
	if (fp == NULL) 
		return -1;
	
	else {
		int i; int j;
		for (i = 0;i < systemSize[0];i++) {
			for (j = 0;j < systemSize[1];j++) {
				if (deliverySystem[i][j].cnt == 1) {
					fputc(i,fp);
					fputc(j, fp);
					fputc((char)deliverySystem[i][j].building, fp);
					fputc((char)deliverySystem[i][j].room, fp);
					fputs(deliverySystem[i][j].passwd, fp);
					fputs(deliverySystem[i][j].context, fp);
				}
			}
		}

		return 0;
	}
	fclose(fp);
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) { //�ù躸���� ����ü �ڷᱸ�� ����
	
	//char buffer[1000];
	int line = 0;
	FILE *fp;
	fp = fopen(filepath, "r"); //file read. fgets?
	
	if (fp == NULL)
		return -1;
	else {
		//file���� ��,��,�����ͺ��, �繰�� ��Ȳ�� read�ؾ���.
		systemSize[0] = 4;
		systemSize[1] = 6;

		//*masterPassword = "1234";

		//��°�ٺ���
		int x; int y; int building; int room; char passwd[10]; char context[100];
		fscanf(fp, "%d %d %d %d %s %s", &x, &y, &building, &room, passwd, context);
		deliverySystem[x][y].cnt=1;
		deliverySystem[x][y].building = building;
		deliverySystem[x][y].room = room;
		deliverySystem[x][y].context = context;
		*deliverySystem[x][y].passwd = passwd;

		storage_t ** deliverySystem;
		deliverySystem = (storage_t**)malloc(24 * sizeof(struct deliverySystem*)); //���� ����
		int i; int j;
		for (i = 0;i < systemSize[0];i++) {
			for (j = 0;j < systemSize[1];j++) {
				initStorage(i, j);
			}
		}
		return 0;
	}
}

//free the memory of the deliverySystem 
void str_freeSystem(void) {
	int i;
	for (i = 0;i < systemSize[0];i++) 
		free(deliverySystem[i]);
	
	free(deliverySystem);
}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0] * systemSize[1]);

	printf("\t");
	for (j = 0;j<systemSize[1];j++)
	{
		printf(" %i\t\t", j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");

	for (i = 0;i<systemSize[0];i++)
	{
		printf("%i|\t", i);
		for (j = 0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}



//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}

	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	return deliverySystem[x][y].cnt;
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE + 1], char passwd[PASSWD_LEN + 1]) {
	//xy�� ������ 
	if (x > 3 || x < 0 || y < 0 || y>5)
		return -1;
	else {
		deliverySystem[x][y].building = nBuilding;
		deliverySystem[x][y].room = nRoom;
		deliverySystem[x][y].cnt = 1;
		strcpy(deliverySystem[x][y].passwd, passwd);
		strcpy(deliverySystem[x][y].context, msg);
		return 0;
	}
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	//char input_passwd[PASSWD_LEN + 1];
	
	if (inputPasswd(x,y) == 0) {
		printf(" -----------> extracting the storage (%d, %d)...", x, y);
		printf("\n------------------------------------------------------------------------\n");
		printf("\n------------------------------------------------------------------------\n");
		printf("\n<<<<<<<<<< %s >>>>>>>>>>\n", deliverySystem[x][y].context);
		printf("\n------------------------------------------------------------------------\n");
		printf("\n------------------------------------------------------------------------\n\n");

		initStorage(x, y);
		return 0;
	}
	else {
		printf("\n -----------> password is wrong!!\n");
		return -1;
	}
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	int returnCnt=0;
	int i; int j;
	for (i = 0;i < systemSize[0];i++) {
		for (j = 0;j < systemSize[1];j++) {
			if (deliverySystem[i][j].building == nBuilding) {
				if (deliverySystem[i][j].room = nRoom) {
					printf("-----------> Found a package in (%d, %d)", i, j);
					returnCnt++;
				}
			}
			return returnCnt;
		}
	}
	return 0;
	
}