#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

void printMenu(int* answer);
void addNewRecord(char * name, char * surname, char * address, char * addition);
void findRecord(char * name, char * surname);
void readIndexFile();
void writeIndexFile();
void writeFlagtoIndexDataFile();
void sortIndexFile();
void listRecords(char c);
void deleteRecord(char * name, char * surname);
void updateRecord(char * name, char * surname, int answer);
int isSynchronized();
void synchronization();

struct _Person { // Person struct uses to write records to data.dat. 250 bytes
	char name[30];
	char surname[30];
	char address[100];
	char addition[90];
};

typedef struct _Person Person;

typedef struct _IndexData
{
	char key[60];
	int index;
	
} IndexData; // IndexData struct uses to write pk and index to index.dat. 64 bytes 

IndexData indexFile[1000]; // IndexData struct array for keep index files in RAM.
int counterForIndexFile = 1; // counter for indexFile[1000] array and this counter uses for delete and add operations.
int dirty4bit = 1; // dirty flag to keep synchronization between data.dat and index.dat.

int main(){
	
	if(isSynchronized() == 1){ // if returns 1 this mean files are not synchronized.
		synchronization();
		sortIndexFile();
	}
	else if(isSynchronized() == 0){ // if returns 0 this mean files are synchronized.
		readIndexFile();
	}
	
	int enOfProgram = 0; // if user press exit this flag will be 1 and program will be ended.
	writeFlagtoIndexDataFile();

	while(!enOfProgram){
	    char inputC[1];
	    char name[30]; //for getting name from scanf
	    char surname[30]; // for getting surname from scanf
	    char address[100]; // for getting address from scanf
		char addition[90]; // for getting addition note from scanf
		int answer = 0; // for main menu
		int answerForUpdate = 0; // for update menu
		
		//print the user menu and read user answer
		printMenu(&answer);
	
		if(answer < 7 && answer > 0){ // control wrong inputs
		
			switch(answer){
				case 1:		
					
					printf("\nNew Record\n");
					printf("------------------\n");
					printf("Please Firstly Enter Just Your Name of New Record (Ex: Ozgur): \n");
					scanf("%s", name);
					printf("\nSecondly Enter Just Your Surname of New Record (Ex: Hepsag): \n");
					scanf("%s", surname);
					printf("\nThirdly Enter Just Your Address of New Record \n");
					printf("(But if your address have multiple word, please enter \"-\" between your words such as \"Buca-Adatepe-Mahallesi\". If there are not multi word, you can enter just one word such as \"Buca\") \n");
					printf("Address:\n");
					scanf("%s", address);
					printf("\nFinally Enter Just Your Additional Note of New Record \n");
					printf("(But if your note have multiple word, please enter \"-\" between your words such as \"Computer-Engineer\". If there are not multi word, you can enter just one word such as \"Engineer\") \n");
					printf("Additional Note:\n");
					scanf("%s", addition);
					addNewRecord(name, surname, address, addition);
					break;
					
				case 2: 
					printf("\nPlease Firstly Enter Single Character to List Records: \n");
					scanf("%s", inputC);
					listRecords(inputC[0]);
					printf("\nThen Enter Only Name and Surname That You Want to Update (Ex: Ozgur Hepsag):\n");
					scanf("%s %s", name, surname);
					printf("\nWhich Part of Record That You Want to Change: \n");
					printf("1- Name \n");
					printf("2- Surname \n");
					printf("3- Address \n");
					printf("4- Addition Note \n");
					printf("Please Enter Your Choise: \n");
					scanf("%d", &answerForUpdate);
					
					if(answer < 5 && answer > 0)	
						updateRecord(name, surname, answerForUpdate); 
					else
						printf("Wrong Input!!\n");
									
					break;
					
				case 3: 
					printf("\nPlease Enter Only Name and Surname That You Want to Delete (Ex: Ozgur Hepsag):\n");
					scanf("%s %s", name, surname);
					deleteRecord(name, surname);				
					break;
				
				case 4: 
					printf("\nPlease Firstly Enter Single Character to List Records: \n");
					scanf("%s", inputC);
					listRecords(inputC[0]);   
					printf("\nThen Enter Only Name and Surname That You Want to Search (Ex: Ozgur Hepsag):\n");
					scanf("%s %s", name, surname);
					findRecord(name, surname);
					break;	
					
				case 5:
					printf("\nPlease Enter Single Character to List Records: \n");
					scanf("%s", inputC);
					listRecords(inputC[0]);
					break;
			
				case 6:
					dirty4bit = 0;
					writeFlagtoIndexDataFile();
					writeIndexFile();
					enOfProgram = 1;
					printf("\nProgram is terminating \n");
					break;
			}
			
			printf("\n");
			
		}
		else{
		
			printf("\nEnter a valid choice by pressing ENTER key again");
			printMenu(&answer);
		
		}
	}

}

void printMenu(int * answer){
	//print the user menu
	printf("\nMY ADDRESS BOOK APPLICATION\n\n");
	printf("Main Menu\n");
	printf("==================================\n");
	printf("1- Add A New Record \n");
	printf("2- Update A Record \n");
	printf("3- Delete A Record \n");
	printf("4- Find A Record \n");
	printf("5- List Records \n");
	printf("6- Exit \n\n");
	printf("Please Enter Your Choise: \n");
	scanf("%d",answer);
	
}

void addNewRecord(char * name, char * surname, char * address, char * addition){

	FILE* fp;
	fp = fopen("data.dat", "rb+");
	
	if(!fp){ // if file is not exist, new data.dat file will be created.
		fp = fopen("data.dat", "wb"); 
		fclose(fp);
		fp = fopen("data.dat", "rb+");
	}
		
	Person temp;
	
	strcpy(temp.name, name); // new record is being created.
	strcpy(temp.surname, surname);
	strcpy(temp.address, address);
	strcpy(temp.addition, addition);
	
	char * primarykey = strcat(name, " ");
	primarykey = strcat(primarykey, surname); // pk is being created.
	
	if(isThere(primarykey) == 0) { // if return 0 , this mean record's name and surname are not same with previous records.
	
		int len;
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		
		strcpy(indexFile[counterForIndexFile].key, primarykey);
		indexFile[counterForIndexFile].index = (len / sizeof(Person)) + 1; // index is being created
	
		counterForIndexFile++;
		sortIndexFile(); // sort the index array in RAM.
		
		fseek(fp, 0, SEEK_END);
		fwrite(&temp, sizeof(Person), 1, fp);
		printf("\nYour record was succesfully added.\n");
	
	}
	else if(isThere(primarykey) == 1){ // if return 1 , this mean record's name and surname are same with previous records. This record will not be added.
		printf("You can not enter same record!!\n");
	}
	
	fclose(fp);

}

int isThere(char * primarykey){ // control if pk of record that user want to add is same with previous records. 
	
	int flagisThere = 0;
	int i = 1;
	char temp[60];
	strcpy(temp, primarykey);
	
	while(i < counterForIndexFile){
		if(strcmp(indexFile[i].key, temp) == 0){
			flagisThere = 1;
			break;
		}
		i++;
	}
	
	return flagisThere;

}

void readIndexFile(){ // read index file if exist when program starts

	FILE* fp;
	fp = fopen("index.dat", "rb+");	
	
	if(fp == NULL){
		return;
	}
	
	int len;
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	len = len - 4; // first bit is dirty bit. so, we substract 4.
		
	IndexData temp;
	
	int i = 0;
	for(i = 0; i < (len / sizeof(IndexData)); i++){
		fseek(fp, 4 + (i * sizeof(IndexData)), SEEK_SET); // first bit is dirty bit. so, we add 4.
		fread(&temp, sizeof(IndexData), 1, fp);
		strcpy(indexFile[counterForIndexFile].key, temp.key);
		indexFile[counterForIndexFile].index = temp.index; // get all information to RAM.
		counterForIndexFile++;
	}
	
	fclose(fp);

}

void writeFlagtoIndexDataFile(){ // write dirty bit to index.dat.

	FILE* fp;
	fp = fopen("index.dat", "wb");
	
	if(!fp){
		return;
	}
	
	fseek(fp, 0, SEEK_SET);
	fwrite(&dirty4bit, sizeof(int), 1, fp);

	fclose(fp);

}

void writeIndexFile(){ // write indexFile[1000] array to index.dat when program ends.

	FILE* fp;
	fp = fopen("index.dat", "rb+");
	
	if(!fp){
		return;
	}
	
	int i = 1;
	
	while(i < counterForIndexFile){
		fseek(fp, 0, SEEK_END);
		fwrite(&indexFile[i], sizeof(IndexData), 1, fp);
		i++;
	}
	
	fclose(fp);
	
}

void sortIndexFile(){ // sort all primarykeys in RAM.
	
	int i, j;
	IndexData temp;
	
	for(i = 1; i < counterForIndexFile; i++){
		for(j = i + 1; j < counterForIndexFile; j++){
        	if(strcmp(indexFile[i].key ,indexFile[j].key)>0){
        	    temp = indexFile[i];
        	    indexFile[i] = indexFile[j];
        	    indexFile[j] = temp;
        	 }
      	}
	}
	
}

void listRecords(char c){ // list records all fields.
	
	int i = 1;
	Person temp;
	
	FILE* fp;
	fp = fopen("data.dat", "rb+");	
	
	if(fp == NULL){
		return;
	}
	
	int isThere = 0;
	
	printf("\nRecords\n");
	printf("------------------\n");
	
	while(i < counterForIndexFile){
		if(indexFile[i].key[0] == c){
			isThere = 1;
			fseek(fp, (indexFile[i].index - 1) * sizeof(Person), SEEK_SET);
			fread(&temp, sizeof(Person), 1, fp);
			printf("Name: %s | Surname: %s | Address: %s | Addition note: %s \n", temp.name, temp.surname, temp.address, temp.addition);
		}
		i++;
	}
	
	if(isThere == 0)
		printf("There is not any record that starts with \"%c\".\n", c);
	
	fclose(fp);	
}

void findRecord(char * name, char * surname){ // finding record that user want to search. and list all information about this record.

	char * primarykey = strcat(name, " ");
	primarykey = strcat(primarykey, surname);

	int i = 1;
	Person temp;
	
	FILE* fp;
	fp = fopen("data.dat", "rb+");	
	
	if(fp == NULL){
		return;
	}
	
	int isThere = 0;
	
	printf("\nRecords\n");
	printf("------------------\n");
	
	while(i < counterForIndexFile){
		if(strcmp(indexFile[i].key, primarykey) == 0){ // control all priamrykeys
			isThere = 1;
			fseek(fp, (indexFile[i].index - 1) * sizeof(Person), SEEK_SET); // take index from index array from RAM and fseek goes to that record in data.dat
			fread(&temp, sizeof(Person), 1, fp);
			printf("Name: %s | Surname: %s | Address: %s | Addition note: %s \n", temp.name, temp.surname, temp.address, temp.addition);
			break;
		}
		i++;
	}
	
	if(isThere == 0)
		printf("There is not any record that his name is \"%s\".\n", primarykey);
	
	fclose(fp);
		
}

void deleteRecord(char * name, char * surname){ // delete record 

	char * primarykey = strcat(name, " ");
	primarykey = strcat(primarykey, surname);
	IndexData temp;

	int i = 1;
	int isThere = 0;
	
	FILE* fp;
	fp = fopen("data.dat", "rb+");	
	
	if(fp == NULL){
		return;
	}
	
	while(i < counterForIndexFile){
		if(strcmp(indexFile[i].key, primarykey) == 0){
			isThere = 1;
			char deleteSymbol[30] = "*";
			fseek(fp, (indexFile[i].index - 1) * sizeof(Person), SEEK_SET); // take index from index array from RAM and fseek goes to that record in data.dat and we write name field "*".
			fwrite(deleteSymbol, sizeof(deleteSymbol), 1, fp);
			printf("Your record succesfully deleted\n");
			temp = indexFile[i];
        	indexFile[i] = indexFile[counterForIndexFile - 1]; // we put that IndexData to end of the array and we substract 1 from counter. This means we remove that IndexData also from RAM.
       	    indexFile[counterForIndexFile - 1] = temp;
       	    counterForIndexFile--;
       	    sortIndexFile(); // re-sort
			break;
		}
		i++;
	}
	
	if(isThere == 0)
		printf("\nThere is not any record that his name is \"%s\".\n", primarykey);	
	
	fclose(fp);

}

void updateRecord(char * name, char * surname, int answer){
	
	 int flagForNewPK = 0;
   	 char nameForUpdate[30];
     char surnameForUpdate[30];
     char addressForUpdate[100];
	 char additionForUpdate[90];
	 
	 if(answer == 1){ 	// which part that user want to change
	 	printf("\nPlease New Name: \n");
		scanf("%s", nameForUpdate);
		flagForNewPK = 1;
	 }
	 else if(answer == 2){
	 	printf("\nPlease New Surname: \n");
		scanf("%s", surnameForUpdate);
		flagForNewPK = 1;
	 }
	 else if(answer == 3){
	 	printf("\nPlease New Address(Please enter \"-\" between multiple words): \n");
		scanf("%s", addressForUpdate);
	 }
	 else if(answer == 4){
	 	printf("\nPlease New Adiition Note(Please enter \"-\" between multiple words): \n");
		scanf("%s", additionForUpdate);
	 }

	char * primarykey = strcat(name, " ");
	primarykey = strcat(primarykey, surname);

	int i = 1;
	Person temp;
	
	FILE* fp;
	fp = fopen("data.dat", "rb+");	
	
	if(fp == NULL){
		return;
	}
	
	while(i < counterForIndexFile){
		if(strcmp(indexFile[i].key, primarykey) == 0){
			fseek(fp, (indexFile[i].index - 1) * sizeof(Person), SEEK_SET);
			fread(&temp, sizeof(Person), 1, fp);
			
			if(answer == 1){ 	
	 			strcpy(temp.name, nameForUpdate);
	 		}
	 		else if(answer == 2){
	 			strcpy(temp.surname, surnameForUpdate);
	 		}
	 		else if(answer == 3){
	 			strcpy(temp.address, addressForUpdate);
			}
	 		else if(answer == 4){
	 			strcpy(temp.addition, additionForUpdate);
	 		}
			
			printf("\nNew Record\n");
			printf("Name: %s | Surname: %s | Address: %s | Addition note: %s\n", temp.name, temp.surname, temp.address, temp.addition);
			
			fseek(fp, (indexFile[i].index - 1) * sizeof(Person), SEEK_SET); // taking index from RAM and writing easily new record to upon old record in data.dat.
			fwrite(&temp, sizeof(Person), 1, fp);
			
			if(flagForNewPK == 1){ // change primary key if user change name or surname
				char * newKey = strcat(temp.name, " ");
				newKey = strcat(newKey, temp.surname);
				strcpy(indexFile[i].key, newKey);
				sortIndexFile();
			}	
			
			
			break;
		}
		i++;
	}
	
	fclose(fp);

}

int isSynchronized(){ // check the dirty bit
	
	FILE* fp;
	fp = fopen("index.dat", "rb+");	
	
	if(fp == NULL){
		return;
	}
	
	int temp;

	fseek(fp, 0, SEEK_SET);
	fread(&temp, sizeof(temp), 1, fp); // first 4 bit for int
	
	fclose(fp);
	
	return temp;
}

void synchronization(){	// synchronization is being succesfull done if data.dat and index.dat are not synchronized
	
	FILE* fpdata;
	fpdata = fopen("data.dat", "rb+");	
	
	if(fpdata == NULL){
		return;
	}
	
	char deleteSymbol[30] = "*";
	int len;
	fseek(fpdata, 0, SEEK_END);
	len = ftell(fpdata);
		
	Person tempRecord;
	
	int i = 0;
	for(i = 0; i < (len / sizeof(Person)); i++){
		fseek(fpdata, i * sizeof(Person), SEEK_SET); // take all records from data.dat to RAM again
		fread(&tempRecord, sizeof(Person), 1, fpdata);
		
		if(strcmp(tempRecord.name, deleteSymbol) == 0) // if first field is "*", this means this data was deleted.
			continue;
		
		char * newKey = strcat(tempRecord.name, " ");
		newKey = strcat(newKey, tempRecord.surname);
		
		strcpy(indexFile[counterForIndexFile].key, newKey); // create array in RAM.
		indexFile[counterForIndexFile].index =  i + 1;
		counterForIndexFile++;
		
	}
	
	fclose(fpdata);

}

