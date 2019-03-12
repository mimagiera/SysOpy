#ifndef zad1_LIB_H
#define zad1_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void createArrayOfPointers(int size);
extern void setCurrentPathAndFileName(char *path, char *fileName,char *tmpFileName);
extern void findFileAndSaveToTmp();
extern void allocateAtIndex(int index, char *block);
extern int saveTmpToBlock();
extern void deleteBlockAtIndex(int index);
extern void deleteArray();
#endif