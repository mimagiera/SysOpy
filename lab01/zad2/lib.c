#include "lib.h"

char *pathGlobal;
char *filenameGlobal;
char **arrayOfPointers;
int sizeGlobal;
const char* tmpFileNameGlobal;

/*
tworzenie polecenia do znalezienia pliku i zapisania wyniku do pliku tymczasowego
*/
char* getInstruction()
{
    char* find = "find ";
    char* name = " -name ";
    char* toFile = " > ";
    char* searchInstruction;
    int length = strlen(find)+strlen(name)+strlen(pathGlobal)+strlen(filenameGlobal)+strlen(toFile)+strlen(tmpFileNameGlobal);
    searchInstruction = calloc(length,sizeof(char));
    strcpy(searchInstruction,find);
    strcat(searchInstruction,pathGlobal);
    strcat(searchInstruction,name);
    strcat(searchInstruction,"\"");
    strcat(searchInstruction,filenameGlobal);
    strcat(searchInstruction,"\"");
    strcat(searchInstruction,toFile);
    strcat(searchInstruction,tmpFileNameGlobal);
    return searchInstruction;
}
/* utworzenie tablicy wskaźników w której będą przechowywane wskaźniki na bloki pamięci zawierające wyniki przeszukiwań
*/
void createArrayOfPointers(int size)
{
    sizeGlobal=size;
    arrayOfPointers = (char**)calloc(size,sizeof(char*));
}
/* ustawienie aktualnie przeszukiwanego katalogu oraz poszukiwanego pliku
*/
void setCurrentPathAndFileName(char *path, char *fileName,char *tmpFileName)
{
    pathGlobal=path;
    filenameGlobal=fileName;
    tmpFileNameGlobal=tmpFileName;
}
/* wykonanie instrukcji
*/
void findFileAndSaveToTmp()
{
    char* instruction = getInstruction();
    system(instruction);
}

//zapisuje blok pod danym indeksem
void allocateAtIndex(int index, char *block)
{
    if(index>=sizeGlobal || index<0)
    {
        return;
    }
    arrayOfPointers[index] = calloc(strlen(block),sizeof(char));
    strcpy(arrayOfPointers[index],block);
}
/* zapisanie danych z pliku do bloku w tablicy
*/
int saveTmpToBlock()
{
    int i =0;
    while(i<sizeGlobal && arrayOfPointers[i]!=NULL)
    {
        i++;
    }
    if(i<sizeGlobal)
    {
        FILE *f = fopen(tmpFileNameGlobal,"r");
        fseek(f,0L,SEEK_END);
        long length  = ftell(f);
        fseek(f,0L,SEEK_SET);
        char* a = calloc(length,sizeof(char));
        fread(a,sizeof(char),length,f);
        allocateAtIndex(i,a);
        return i;
    }
    else
    {
        return -1;
    }
}
/* usunięcie z pamięci bloku o zadanym indeksie
*/
void deleteBlockAtIndex(int index)
{
    if(index>=0 && index<sizeGlobal && arrayOfPointers!=NULL && arrayOfPointers[index]!=NULL)
    {
        free(arrayOfPointers[index]);
        arrayOfPointers[index]=NULL;
    }
}
//usuniecie calej tablicy
void deleteArray()
{
    if(arrayOfPointers!=NULL)
    {
        for(int i =0;i<sizeGlobal;i++)
        {
            deleteBlockAtIndex(i);
        }
        //free(arrayOfPointers);
    }
}