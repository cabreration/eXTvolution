#include "structures.h"

int createHardFile(char* path);

int fileExists(char* path);

int createDisk(char* path, int size, char unit, char fit);

int removeDisk(char* path);

int createPart(char * path, char tipo, int size, char * nombre, char unidad, char fit);

int modifyPart(char * path, char * name, int add, char unit);

int deletePart(char * path, char * mode, char * name);

int mountPart(char * path, char * name, IDLIST *lista);

int unmountPart(char * id, IDLIST *lista);

long int findBest(MBR mbr, long int size);

long int findWorst(MBR mbr, long int size);

long int findFirst(MBR mbr, long int size);

void ordenarMBR(MBR * mbr);

void printMBR(char *path);

void insertId(char *path, char *name, IDLIST *lista);

void printIdList(IDLIST *lista);
