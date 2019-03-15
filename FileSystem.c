#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "structures.h"
#include <stdio.h>
#include "filesystem.h"
#include "cadenas.h"
#include <errno.h>
#include <math.h>

int createHardFile(char* path) {
    char comando[512] = {0};
    strcpy(comando, "mkdir -p ");
    strcat(comando, path);
    int retorno = system(comando);
    if (path) free(path);
    for (int i = 0; i < 512; i++)
        comando[i] = 0;
    return retorno;
}

int fileExists(char* path) {
    FILE *file;
    if ((file = fopen(path, "r"))) {
        fclose(file);
        return 1;
    }
    return 0;
}

int createDisk(char *path, int size, char unit, char fit) {
    FILE * disk;
    char buffer[1024] = {0};
    MBR mbr;
    timer_t hora;
    time(&hora);
    struct tm *local = localtime(&hora);
    char salida[16];
    strftime(salida, 16, "%d/%m/%y %H:%M", local);
    srand(time(0));
    int random = rand();
    quitarComillas(path);

    disk = fopen(path, "wb+");
    if (disk == NULL) {
        perror("El error fue: ");
        return 0;
    }

    if (unit == 'k') {
        for (int i = 0; i < size; i++)
            fwrite(&buffer, 1024, 1, disk);
        fflush(disk);
        fclose(disk);
        mbr.size = size*1024;
    }
    else {
        long int megas = size*1024;
        for (int i = 0; i < megas; i++)
            fwrite(&buffer, 1024, 1, disk);
        fflush(disk);
        fclose(disk);
        mbr.size = megas*1024;
    }
    strcpy(mbr.date, salida);
    mbr.signature = random;
    mbr.fit = fit;
    for(int i = 0; i < 4; i++) {
        mbr.particiones[i].status = '0';
        mbr.particiones[i].start = 0;
        mbr.particiones[i].size = 0;
        mbr.particiones[i].type = 'p';
        mbr.particiones[i].fit = 'w';
        for (int y = 0; y < 16; y++)
            mbr.particiones[i].name[y] = 0;
    }

    disk = fopen(path, "rb+");
    fseek(disk,0,SEEK_SET);
    fwrite(&mbr, sizeof(mbr), 1, disk);
    fflush(disk);
    fclose(disk);

    if (path) free(path);
    return 1;
}

int removeDisk(char* path) {
    char comando[256] = {0};
    strcpy(comando, "rm ");
    int tamanio = strlen(path) + 1;
    char* upath = (char*) malloc(sizeof(char)*tamanio + 1);
    strcpy(upath, path);
    strcat(comando, upath);
    int ret = 0;
    printf("Si elimina el disco no podra recuperarlo, desea continuar?\n");
    char buffer[10] = { 0 };
    fgets(buffer, sizeof(buffer), stdin);
    if (!(strcmp(buffer, "no")) || !(strcmp(buffer, "no\n"))) return -1;
    ret = system(comando);
    free(upath);
    upath = 0;
    return ret;
}

void printMBR(char *path) {
    MBR mbr;
    FILE *disk;
    quitarComillas(path);
    disk = fopen(path, "rb+");
    if (disk == NULL)
        return;

    fseek(disk, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, disk);
    fclose(disk);

    printf("Particion leida: \n");
    printf("Size: %d\n", mbr.size);
    printf("Signature: %d\n", mbr.signature);
    printf("Date: %s\n", mbr.date);
    printf("Fit: %c\n", mbr.fit);
    for (int i = 0; i < 4; i++) {
        printf("type: %c\n", mbr.particiones[i].type);
        printf("start: %d\n", mbr.particiones[i].start);
        printf("size: %d\n", mbr.particiones[i].size);
        printf("name: %s\n", mbr.particiones[i].name);
    }
}

int createPart(char * path, char tipo, int size, char * nombre, char unidad, char fit) {
    if (tipo == 'l') return -4;
    MBR mbr;
    FILE *disk;
    quitarComillas(path);
    disk = fopen(path, "rb+");
    if (disk == NULL)
        return -1;

    fseek(disk, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, disk);
    fclose(disk);

    for (int k = 0; k < 4; k++) {
        if (!(strcmp(nombre, mbr.particiones[k].name))) return -6;
    }

    long int finMbr = (long int) sizeof(mbr);
    if (unidad == 'k') {
        size *= 1024;
    }
    else if (unidad == 'm') {
        size *= 1024 * 1024;
    }

    if (mbr.particiones[0].start == 0
            && mbr.particiones[1].start == 0
            && mbr.particiones[2].start == 0
            && mbr.particiones[3].start == 0) {
        //no hay particiones creadas para nada
        if (tipo == 'l') return -4; //no puede crear particiones logicas sin una particion extendida

        mbr.particiones[0].type = tipo;
        mbr.particiones[0].size = size;
        strcpy(mbr.particiones[0].name, nombre);
        mbr.particiones[0].fit = fit;
        mbr.particiones[0].start = finMbr; //exclusive, si pongo mas 1 seria inclusivo
    }
    else if (mbr.particiones[0].start > 0
             && mbr.particiones[1].start > 0
             && mbr.particiones[2].start > 0
             && mbr.particiones[3].start > 0) {
        return -2; //ya existen 4 particiones
    }
    else {
        //ya hay particiones pero no son mas de 3
        if (tipo == 'e') {
            for (int j = 0; j < 4; j++) {
                if (mbr.particiones[j].type == 'e') return -5; //solo puede haber una particion extendida
            }
        }
        int i; // obtenemos la posicion en el arreglo de particiones
        for (i = 0; i < 4; i++) {
            if (mbr.particiones[i].start == 0)
                break;
        }
        long int bit = 0;
        if (mbr.fit == 'b') {
            bit = findBest(mbr, size);
        }
        else if (mbr.fit == 'w') {
            bit = findWorst(mbr, size);
        }
        else if (mbr.fit == 'f') {
            bit = findFirst(mbr, size);
        }
        if (bit == 0)
            return -3; //no existe un fragmento con la capacidad para crear una particion de ese tama;o con el ajuste actual

        mbr.particiones[i].type = tipo;
        mbr.particiones[i].size = size;
        strcpy(mbr.particiones[i].name, nombre);
        mbr.particiones[i].fit = fit;
        mbr.particiones[i].start = bit;
        ordenarMBR(&mbr);
    }
    disk = fopen(path, "rb+");
    fseek(disk,0,SEEK_SET);
    fwrite(&mbr, sizeof(mbr), 1, disk);
    fflush(disk);
    fclose(disk);
    return 1;
}

long int findBest(MBR mbr, long int size) {
    //donde queda menos espacio disponible y cabe la nueva particion
    //voy a retornar el bit de inicio del fragmento disponible
    long int bitActual = (long int) sizeof(mbr);
    long int globalDif = mbr.size;
    long int bitRetorno = 0;
    int i = 0;
    while (i < 4) {
        if (mbr.particiones[i].start == bitActual) {
            bitActual += mbr.particiones[i].size;
            i++;
        }
        else if (mbr.particiones[i].start == 0) {
            long int diferencia = 0;
            if (i == 0)
                diferencia = mbr.size - (long int)sizeof(mbr);

            else
                diferencia = mbr.size - (mbr.particiones[i-1].start + mbr.particiones[i-1].size);

            if (diferencia >= size && diferencia < globalDif) {
                //encontre lo que buscaba
                return bitActual;
            }
            else
                return bitRetorno;
        }
        else if (mbr.particiones[i].start > bitActual) {
            int localDif = mbr.particiones[i].start - bitActual;
            globalDif = (localDif < globalDif) ? localDif : globalDif;
            if (globalDif >= size) {
                //encontre una posibilidad
                bitRetorno = bitActual;
            }
            bitActual = mbr.particiones[i].start + mbr.particiones[i].size;
            i++;
        }
    }
    return bitRetorno;
}

long int findWorst(MBR mbr, long int size) {
    //donde la diferencia de espacios sea mas grande
    //retornar el bit de inicio del fragmento
    long int bitActual = (long int) sizeof(mbr);
    long int globalDif = 0;
    long int bitRetorno = 0;
    int i = 0;
    while (i < 4) {
        if (mbr.particiones[i].start == bitActual) {
            bitActual += mbr.particiones[i].size;
            i++;
        }
        else if (mbr.particiones[i].start == 0) {
            long int diferencia = 0;
            if (i == 0)
                diferencia = mbr.size - (long int)sizeof(mbr);

            else
                diferencia = mbr.size -
                        (mbr.particiones[i-1].start + mbr.particiones[i-1].size);

            if (diferencia >= size && diferencia > globalDif) {
                //encontre lo que buscaba
                return bitActual;
            }
            else
                return bitRetorno;
        }
        else if (mbr.particiones[i].start > bitActual) {
            int localDif = mbr.particiones[i].start - bitActual;
            globalDif = (localDif > globalDif) ? localDif : globalDif;
            if (globalDif >= size) {
                //encontre una posibilidad
                bitRetorno = bitActual;
            }
            bitActual = mbr.particiones[i].start + mbr.particiones[i].size;
            i++;
        }
    }
    return bitRetorno;
}

long int findFirst(MBR mbr, long int size) {
    //el primero que encuentre
    //retornar el bit de inicio del fragmento disponible
    long int bitActual = (long int) sizeof(mbr);

    int i = 0;
    while (i < 4) {
        if (mbr.particiones[i].start == bitActual) {
            bitActual += mbr.particiones[i].size;
            i++;
        }
        else if (mbr.particiones[i].start > bitActual) {
            //encontre el primer fragmento
            long int dif = mbr.particiones[i].start - bitActual;
            if (dif >= size) {
                //encontre lo que estaba buscando
                return bitActual;
            }
            else {
                bitActual = mbr.particiones[i].start + mbr.particiones[i].size;
                i++;
            }
        }
        else if (mbr.particiones[i].start == 0) {
            long int diference = 0;
            if (i == 0) diference = mbr.size - sizeof(mbr);
            else diference = mbr.size - (mbr.particiones[i-1].start +
                    mbr.particiones[i-1].size);
            if (diference >= size) {
                //encontre lo que buscaba
                return bitActual;
            }
            else
                return 0; //todo lo que venga despues de una particion que no existe tampoco existe, asi que ya no lo evaluamos
        }
    }
    return 0;
}

void ordenarMBR(MBR * mbr) {
    for (int j = 0; j < 3; j++) {
        for(int i = 3; i > j; i--) {
            if (mbr->particiones[i].start == 0) continue;
            else if (mbr->particiones[i].start < mbr->particiones[i-1].start
                     || mbr->particiones[i-1].start == 0) {
                Particion particion = mbr->particiones[i-1];
                mbr->particiones[i-1] = mbr->particiones[i];
                mbr->particiones[i] = particion;
            }
        }
    }
}

int modifyPart(char * path, char * name, int add, char unit) {
    long int localSize = add;
    MBR mbr;
    FILE *disk;
    quitarComillas(path);
    disk = fopen(path, "rb+");
    if (disk == NULL)
        return -1; //no existe el disco al que intenta acceder

    fseek(disk, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, disk);
    fclose(disk);

    //primero chequeamos que exista una particion con ese nombre
    int indice = -1;
    for (int i = 0; i < 4; i++) {
        if (!(strcmp(mbr.particiones[i].name, name))) {
            indice = i;
            break;
        }
    }
    if (indice == -1)
        return -2; //no existe una particion con ese nombre

    //modificamos el tamanio para poder usarlo
    if (unit == 'k') localSize *= 1024;
    else if (unit == 'm') localSize *= 1024 * 1024;

    if (add > 0) {
        //chequeamos si hay espacio suficiente en el lado derecho
        long int dif = 0;
        if (indice == 3 || (indice < 3 && mbr.particiones[indice+1].start == 0)) //en caso de que sea la ultima particion o la siguiente no exista
            dif = mbr.size - (mbr.particiones[indice].start + mbr.particiones[indice].size);
        else
            dif = mbr.particiones[indice+1].start - (mbr.particiones[indice].start + mbr.particiones[indice].size);

        if ((dif-1) >= localSize) {
            mbr.particiones[indice].size += localSize;
        }
        else {
            /*if (indice > 0) // esto hace que la particion pueda crecer hacia la izquierda pero el enunciado lo prohibe
                dif = mbr.particiones[indice].start - (mbr.particiones[indice-1].start + mbr.particiones[indice-1].size);
            else
                dif = mbr.particiones[indice].start - sizeof(mbr);

            if ((dif - 1) >= localSize) {
                mbr.particiones[indice].start -= localSize;
                mbr.particiones[indice].size += localSize;
            }
            else */
            return -3;
        }
    }
    else {
        mbr.particiones[indice].size += localSize;
    }

    disk = fopen(path, "rb+");
    fseek(disk,0,SEEK_SET);
    fwrite(&mbr, sizeof(mbr), 1, disk);
    fflush(disk);
    fclose(disk);
    return 1;
}

int deletePart(char *path, char *mode, char *name) {
    MBR mbr;
    FILE *disk;
    quitarComillas(path);
    disk = fopen(path, "rb+");
    if (disk == NULL)
        return -1; //no existe el disco al que intenta acceder

    fseek(disk, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, disk);
    fclose(disk);

    //primero chequeamos que exista una particion con ese nombre
    int indice = -1;
    for (int i = 0; i < 4; i++) {
        if (!(strcmp(mbr.particiones[i].name, name))) {
            indice = i;
            break;
        }
    }
    if (indice == -1)
        return -2; //no existe una particion con ese nombre

    if (!(strcmp(mode, "full"))) {
        long int pos = mbr.particiones[indice].start
                + mbr.particiones[indice].size;
        int mult = pos/1024;
        int rest = pos%1024;
        char buffer[1024] = {0};
        char buffer2[1] = {0};
        FILE * writer;
        writer = fopen(path, "rb+");
        fseek(writer, mbr.particiones[indice].start, SEEK_SET);
        for (int i = 0; i < mult; i++)
            fwrite(&buffer, 1024, 1, writer);
        for (int j = 0; j < rest; j++)
            fwrite(&buffer2, 1, 1, writer);
        fflush(writer);
        fclose(writer);

        mbr.particiones[indice].start = 0;
        mbr.particiones[indice].size = 0;
        mbr.particiones[indice].status = 'i';
        mbr.particiones[indice].type = 'p';

        for (int k = 0; k < 16; k++)
            mbr.particiones[indice].name[k] = 0;

        mbr.particiones[indice].fit = 'w';

        ordenarMBR(&mbr);

    }
    else { //modo fast
        mbr.particiones[indice].status = 0;
    }

    disk = fopen(path, "rb+");
    fseek(disk,0,SEEK_SET);
    fwrite(&mbr, sizeof(mbr), 1, disk);
    fflush(disk);
    fclose(disk);
    return 1;
}

int mountPart(char *path, char *name, IDLIST *lista) {
    MBR mbr;
    FILE *disk;
    quitarComillas(path);
    disk = fopen(path, "rb+");
    if (disk == NULL)
        return -1; //no existe el disco al que intenta acceder

    fseek(disk, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, disk);
    fclose(disk);

    //primero chequeamos que exista una particion con ese nombre
    int indice = -1;
    for (int i = 0; i < 4; i++) {
        if (!(strcmp(mbr.particiones[i].name, name))) {
            indice = i;
            break;
        }
    }
    if (indice == -1)
        return -2; //no existe una particion con ese nombre

    //chequear si la particion esta montada o no

    //lo metemos a la lista
    insertId(path, name, lista);

    printIdList(lista);

    disk = fopen(path, "rb+");
    fseek(disk, 0, SEEK_SET);
    fwrite(&mbr, sizeof(mbr), 1, disk);
    fflush(disk);
    fclose(disk);
    return 1;
}

int unmountPart(char *id, IDLIST *lista) {
    if (lista->active == 0) {
        return -1;
    }

    for (int i = 0; i < 400; i++) {
        if (lista->ids[i].id[0] == 0) continue;

        if (!(strcmp(lista->ids[i].id, id))) {
                memset(lista->ids[i].id, 0, 7);

            for (int k = 0; k < 256; k++)
                lista->ids[i].path[k] = 0;

            for (int j = 0; j < 16; j++)
                lista->ids[i].name[j] = 0;

            return 1;
        }
    }
    return -2;
}

void insertId(char *path, char *name, IDLIST *lista) {
    if (lista->active) { //ya hay particiones montadas
        //contar cuantas apariciones tiene el disco en la lista
        //encontrar numero y letra
        char num[4] = {0};
        char will = 0;
        int i = 0;
        int apariciones = 0;
        while (i < 400) {
            if (lista->ids[i].id[0] == 0) {
                i++;
                continue;
            }

            if (!(strcmp(lista->ids[i].path, path))) { //significa que el disco ya tiene particiones montadas
                will = lista->ids[i].id[2];
                apariciones = 1;
                break;
            }
            i++;
        }

        if (apariciones == 0) {
            //necesitamos una nueva letra
            lista->current++;
            will = lista->current;
        }

        //buscamos la posicion
        int k = 0;
        while (lista->ids[k].id[0])
            k++;

        //creamos el id y listo para escribir
        lista->ids[k].id[0] = 'v';
        lista->ids[k].id[1] = 'd';
        lista->ids[k].id[2] = will;
        sprintf(num, "%d", lista->correlatives[will-97]);
        strcat(lista->ids[k].id, num);
        lista->ids[k].id[6] = 0;
        lista->correlatives[will-97]++;

        strcpy(lista->ids[k].path, path);
        strcpy(lista->ids[k].name, name);
    }
    else { //no hay particiones montadas
        strcpy(lista->ids[0].id, "vda1");
        strcpy(lista->ids[0].path, path);
        strcpy(lista->ids[0].name, name);
        lista->correlatives[0] = 2;
    }
}

void printIdList(IDLIST *lista) {
    for (int i = 0; i < 400; i++) {
        if (lista->ids[i].id[0] == 0) continue;

        printf("Id: %s -- Name: %s -- Path: %s\n", lista->ids[i].id, lista->ids[i].name, lista->ids[i].path);
    }
}

int makeFileSystem(IDLIST *lista, char *id, char *type, char fs) {
    //primero chequeamos que el identificador si concuerde con un identificador en la lista:
    char path[128] = {0};
    char name[32] = {0};
    int flag = 0;
    for (int i = 0; i < 400; i++) {
        if (!strcmp(id, lista->ids[i].id)) {
            strcpy(path, lista->ids[i].path);
            strcpy(name, lista->ids[i].name);
            flag = 1;
            break;
        }
    }

    if (flag == 0) return -1; //el identificador no se encontraba en la lista

    //si el identificador si se encontraba en la lista ejecutamos empezando con el fast o full:
    //antes chequeamos que si haya una particion con ese nombre
    MBR *mbr = readMBR(path);
    int index  = -1;
    for (int j = 0; j < 4; j++) {
        if (!strcmp(mbr->particiones[j].name, name)) {
            index = j;
            break;
        }
    }
    if (index == -1) return -2; //no existe una particion con ese nombre

    if (!strcmp(type, "full")) {
        int startBit = mbr->particiones[index].start + 1;
        int pos = mbr->particiones[index].size;
        int mult = pos/1024;
        int rest = pos%1024;
        char buffer[1024] = {0};
        char buffer2[1] = {0};
        FILE * writer;
        writer = fopen(path, "rb+");
        fseek(writer, startBit, SEEK_SET);
        for (int i = 0; i < mult; i++)
            fwrite(&buffer, 1024, 1, writer);
        for (int j = 0; j < rest; j++)
            fwrite(&buffer2, 1, 1, writer);
        fflush(writer);
        fclose(writer);
    }

    //una vez los datos borrados, entonces meto el sistema de archivos
    //empezamos por encontrar el numero de inodos y el numero de bloques
    int inodesCount = 0;
    int x = mbr->particiones[index].size;
    int a = sizeof(SUPERBLOQUE);
    int i = sizeof(INODE);
    int b = 64;
    int j = sizeof(JOURNAL);
    int n  = 0;
    if (fs == 2)
        n = (x - a) / (4 + i + 3*b);
    else
        n = (x- a) / (4 + i + 3*b + j);

    inodesCount = floor(n);

    createFileSystem(path, mbr->particiones[index].start+1, fs, inodesCount);

    //vaciamos path y name antes de terminar
    memset(path, 0, 128);
    memset(path, 0, 32);

    return 1; //el formateo fue realizado con exito
}

MBR* readMBR(char *path) {
    MBR *mbr = NULL;
    FILE *disk;
    disk = fopen(path, "rb+");
    if (disk == NULL)
        return NULL; //no existe el disco al que intenta acceder

    fseek(disk, 0, SEEK_SET);
    fread(mbr, sizeof(MBR), 1, disk);
    fclose(disk);
    return mbr;
}

void createFileSystem(char *path, int start, int type, int inodesCount) {
    int blocksCount = 3 * inodesCount;
    char inodesTable[inodesCount];
    char blocksTable[blocksCount];

    for (int j = 0; j < inodesCount; j++)
        inodesTable[j] = '0';
    for (int i = 0; i < blocksCount; i++)
        blocksTable[i] = '0';
    inodesTable[0] = '1';
    blocksTable[0] = '1';

    FILE *disk;

    SUPERBLOQUE super = generateSuperBlock(inodesCount, type);
    INODE rootInode = generateInode(1, 1, 0, 0, 700);
    rootInode.block[0] = 0;
    INODE inode = generateInode(-1, -1, 0, -1, 0);
    DIRECTORYBLOCK block = generateDirectoryBlock();
    DIRECTORYBLOCK rootBlock = generateDirectoryBlock();
    rootBlock.content[0].inode = 0;
    rootBlock.content[1].inode = 0;
    strcpy(rootBlock.content[0].name, "/");
    strcpy(rootBlock.content[1].name, "/");

    if (type == 2) {
        //el orden es: superBloque, bm inodos, bm bloques, inodos, bloques

        disk = fopen(path, "rb+");
        fseek(disk, start, SEEK_SET);
        fwrite(&super, sizeof(SUPERBLOQUE), 1, disk);
        fwrite(&inodesTable, inodesCount, 1, disk);
        fwrite(&blocksTable, blocksCount, 1, disk);
        fwrite(&rootInode, sizeof(INODE), 1, disk);
        for (int i = 0; i < inodesCount - 1; i++)
            fwrite(&inode, sizeof(INODE), 1, disk);
        fwrite(&rootBlock, sizeof(DIRECTORYBLOCK), 1, disk);
        for (int j = 0; j < blocksCount - 1; j++)
            fwrite(&block, sizeof(DIRECTORYBLOCK), 1, disk);
        fflush(disk);
        fclose(disk);
    }
    else {
        //superbloque, journaling, bm inodos, bm bloques, inodos, bloques

        JOURNAL journal = generateJournal();
        disk = fopen(path, "rb+");
        fseek(disk, start, SEEK_SET);
        fwrite(&super, sizeof(SUPERBLOQUE), 1, disk);
        for (int k = 0; k < inodesCount; k++)
            fwrite(&journal, sizeof(JOURNAL), 1, disk);
        fwrite(&inodesTable, inodesCount, 1, disk);
        fwrite(&blocksTable, blocksCount, 1, disk);
        fwrite(&rootInode, sizeof(INODE), 1, disk);
        for (int i = 0; i < inodesCount - 1; i++)
            fwrite(&inode, sizeof(INODE), 1, disk);
        fwrite(&rootBlock, sizeof(DIRECTORYBLOCK), 1, disk);
        for (int j = 0; j < blocksCount; j++)
            fwrite(&block, sizeof(DIRECTORYBLOCK), 1, disk);
        fflush(disk);
        fclose(disk);
    }
}

SUPERBLOQUE generateSuperBlock(int inodesCount, int type) {
    SUPERBLOQUE super_bloque;
    super_bloque.magic = 201403905;
    super_bloque.systemType = type;
    super_bloque.inodesCount = inodesCount;
    super_bloque.blocksCount = 3*inodesCount;
    super_bloque.freeInodesCount = inodesCount - 1;
    super_bloque.freeBlocksCount = (3*inodesCount) - 1;
    super_bloque.inodeSize = sizeof(INODE);
    super_bloque.blockSize = 64;
    super_bloque.firstInode = 1;
    super_bloque.firstBlock = 1;
    super_bloque.bmInodeStart = (start - 1) + sizeof(SUPERBLOQUE); //exclusivo
    super_bloque.bmBlockStart = (start - 1) + sizeof(SUPERBLOQUE) + inodesCount; //exclusivo
    super_bloque.inodeStart = (start - 1) + sizeof(SUPERBLOQUE) + inodesCount + blocksCount; //exclusivo
    super_bloque.blockStart = (start - 1) + sizeof(SUPERBLOQUE) + inodesCount + blocksCount + sizeof(INODE)*inodesCount; //exclusivo
    super_bloque.mountCount = 1;
    timer_t hora;
    time(&hora);
    struct tm *local = localtime(&hora);
    strftime(super_bloque.mTime, 16, "%d/%m/%y %H%M", local);
    memset(super_bloque.unTime, 0, 16);
    return super_bloque;
}

INODE generateInode(int uid, int gid, int size, int type, int perm) {
    INODE inode;
    inode.uid = uid;
    inode.gid = gid;
    inode.size = size;
    memset(inode.readDate, 0, 16);
    timer_t hora;
    time(&hora);
    struct tm *local = localtime(&hora);
    strftime(inode.createDate, 16, "%d/%m/%y %H%M", local);
    memset(inode.modDate, 0, 16);
    for (int i = 0; i < 16; i++) {
        inode.block[i] = -1;
    }
    inode.type = type;
    inode.perm = perm;
    return inode;
}

DIRECTORYBLOCK generateDirectoryBlock() {
    DIRECTORYBLOCK directory;
    for (int i = 0; i < 4; i++) {
        directory.content[i].inode = -1;
        memset(directory.content[i].name, 12, 0);
    }
    return directory;
}

JOURNAL generateJournal() {
    JOURNAL journal;
    journal.operationType = 0;
    journal.type = -1;
    memset(journal.name, 0, 16);
    journal.content = 0;
    memset(journal.date, 0, 16);
    memset(journal.owner, 0, 11);
    journal.permissions = 0;
}
