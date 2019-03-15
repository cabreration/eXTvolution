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

typedef struct SuperBloque {
    int systemType;
    int inodesCount;
    int blocksCount;
    int freeBlocksCount;
    int freeInodesCount;
    char mTime[16];
    char unTime[16];
    int mountCount;
    int magic;
    int inodeSize;
    int blockSize;
    int firstInode;
    int firstBlock;
    int bmInodeStart;
    int bmBlockStart;
    int inodeStart;
    int blockStart;
} SUPERBLOQUE;

typedef struct Journal {
    int operationType;
    int type;
    char name[16];
    int content;
    char date[16];
    char owner[11];
    int permissions;
} JOURNAL;

typedef struct InodesTable{
    int uid;
    int gid;
    int size;
    char readDate[16];
    char createDate[16];
    char modDate[16];
    int block[15];
    char type;
    int perm;
} INODE;

typedef struct content {
    char name[13];
    int inode;
} CONTENT;

typedef struct BloqueCarpetas {
    CONTENT content[4];
} DIRECTORYBLOCK;

typedef struct filesBlock {
    char content[64];
} FILESBLOCK;

typedef struct pointersBlock {
    int pointers[16];
} POINTERSBLOCK;

#endif
