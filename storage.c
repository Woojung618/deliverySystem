#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/*
definition of storage cell structure ----
members :
int building : building number of the destination
int room : room number of the destination
int cnt : number of packages in the cell. 셀 안의 패키지 수. 0이면 빈거.
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
static int storedCnt = 0;					//number of cells occupied 차있는 칸수
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

static void initStorage(int x, int y) {//보관함 하나를 초기화
	
	free(deliverySystem[x]+y);//free
	deliverySystem[x][y].cnt = 0;
	deliverySystem[x][y].building = 0;
	deliverySystem[x][y].room = 0;
	deliverySystem[x][y].context = NULL;
	*deliverySystem[x][y].passwd = 0; //배열 안의 내용이 0이 되게.


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

int str_backupSystem(char* filepath) { //현재 보관함들의 상태 및 설정 값들을 파일에 저장
	//파일에 x y building room 비번 메세지
	FILE *fp;
	fp = fopen(filepath, "w"); //for문 돌려서 택배 차있는거 싹다 써지도록.
	if (fp == NULL) 
		return -1;
	
	else {
		int i; int j;
		for (i = 0;i < systemSize[0];i++) {
			for (j = 0;j < systemSize[1];j++) {
				if (deliverySystem[i][j].cnt == 1) {
					
					fprintf(fp,"%d %d %d %d %s %s",i,j,deliverySystem[i][j].building,deliverySystem[i][j].room,deliverySystem[i][j].passwd,deliverySystem[i][j].context);
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

int str_createSystem(char* filepath) { //택배보관함 구조체 자료구조 생성
	
	//make file NULL
	FILE *fp=NULL;
	int line = 0;
	char c;
	fp = fopen(filepath, "r"); 
	
	if (fp == NULL)
		return -1;
		
	//malloc
	
	//file에서 행,열,마스터비번, 사물함 상황들 read해야함.
	int i,j;
	int inputrow, inputcolumn;
		
	deliverySystem = (storage_t**)malloc(systemSize[0]*sizeof(storage_t*));
	
	fscanf(fp, "%d %d %s", &systemSize[0], &systemSize[1], masterPassword);
	
	for(i=0;i<systemSize[0];i++) {
		deliverySystem[i]= (storage_t*)malloc(systemSize[1]*sizeof(storage_t));
	}
	
	for(i=0;i<systemSize[0];i++) {
		for(j=0;j<systemSize[1];j++)
			deliverySystem[i][j].context = (char *)malloc(100 * sizeof(char));
	}
	
	while(fscanf(fp, "%d %d", &inputrow, &inputcolumn)==2){
		
		fscanf(fp, "%d %d %s %s", &deliverySystem[inputrow][inputcolumn].building, &deliverySystem[inputrow][inputcolumn].room,
		 &deliverySystem[inputrow][inputcolumn].room, deliverySystem[inputrow][inputcolumn].passwd, deliverySystem[inputrow][inputcolumn].context);
		
	}
	

	fclose(fp);
	return 0;	
		
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
	//xy값 따져서 
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
