#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct particion{
    char status;
    char type;
    char fit;
    int start;
    int size;
    char name[16];
} Particion;

typedef struct mbr {
    int size;
    int signature;
    char date[24];
    char fit;
    Particion particiones[4];
} MBR;

typedef struct ebr {
    char status;
    char fit;
    int start;
    int size;
    int next;
    char name[20];
} EBR;

typedef struct relation {
    char id[7];
    char path[256];
    char name[16];
} REL;

typedef struct idList {
    REL ids[400];
    char current;
    int active;
    int correlatives[26];
} IDLIST;


#endif
