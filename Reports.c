#include "reports.h"
#include "structures.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cadenas.h"
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "filesystem.h"

int clasificarReporte(char *id, char *path, char *name, IDLIST *lista) {
    //una vez con el directorio creado verificamos que tipo de reporte es:
    if (lista->active == 0) return -11; //no hay particiones montadas

    if (!(strcmp(name, "mbr")) || !(strcmp(name, "mbr\n")) || !strcmp(name, "mbr\r")) {
        int respuestaMbr = reporteMBR(id, path, lista);
        if (respuestaMbr == -1) return -8; //el identificador no esta en la lista
        if (respuestaMbr == -2) return -9; //el identificadoresta en la lista pero no hay direccion asociada
        if (respuestaMbr == -3) return -10; //la direccion asociada no existe
        if (respuestaMbr == 1) return 2; //reporte mbr creado cone exito
    }
    else {
        int respuestaDisk = reporteDisk(id, path, lista);
        if (respuestaDisk == -1) return -8;
        else if (respuestaDisk == -3) return -10;
        else if (respuestaDisk == 1) return 3;
    }

    return 1;
}

int reporteMBR(char *id, char *direccion, IDLIST *lista) {
    char *name = (char*)malloc(16*sizeof(char));
    name[0] = 0;
    char *path = (char*)malloc(256*sizeof(char));
    path[0] = 0;

    for (int i = 0; i < 400; i++) {
        if (lista->ids[i].id[0] == 0) continue;

        if (!(strcmp(lista->ids[i].id, id))) {
            strcpy(name, lista->ids[i].name);
            strcpy(path, lista->ids[i].path);
        }
    }

    if (name[0] == 0) return -1; //no existe una particion con ese identificador
    else if (path[0] == 0) return -2; //no hay una direccion asociada al identificador, baboso

    //a partir de aqui busco el MBR
    MBR mbr;
    FILE *disk;
    quitarComillas(path);
    disk = fopen(path, "rb+");
    if (disk == NULL)
        return -3; // la direccion asociada no existe, baboso, algo perdiste

    fseek(disk, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, disk);
    fclose(disk);

    //ya con el MBR creo el reporte, lo hacemos escribiendo en un archivo
    FILE *printer = NULL;
    printer = fopen("mbr.dot", "w");

    fprintf(printer, "digraph G{\n");
    fprintf(printer, "MBR [\n");
    fprintf(printer, "shape=plaintext\n");
    fprintf(printer, "label=<\n");
    fprintf(printer, "<table border='0' cellborder='1' cellspacing='0' cellpadding='10'>\n");

    // Encabezado
    fprintf(printer, "<tr>\n");
    fprintf(printer, "<td><b>Nombre</b></td>\n");
    fprintf(printer, "<td><b>Valor</b></td>\n");
    fprintf(printer, "</tr>\n");

    // Valores MBR:
    fprintf(printer, "<tr>\n");
    fprintf(printer, "<td><b>Tamanio MBR</b></td>\n");
    fprintf(printer, "<td>%d</td>\n", mbr.size);
    fprintf(printer, "</tr>\n");

    fprintf(printer, "<tr>\n");
    fprintf(printer, "<td><b>Fecha Creacion MBR</b></td>\n");
    fprintf(printer, "<td>%s</td>\n", mbr.date);
    fprintf(printer, "</tr>\n");

    fprintf(printer, "<tr>\n");
    fprintf(printer, "<td><b>Signature MBR</b></td>\n");
    fprintf(printer, "<td>%d</td>\n", mbr.signature);
    fprintf(printer, "</tr>\n");

    fprintf(printer, "<tr>\n");
    fprintf(printer, "<td><b>Ajuste del Disco</b></td>\n");
    fprintf(printer, "<td>%c</td>\n", mbr.fit);
    fprintf(printer, "</tr>\n");

    // Valores Particiones:

    for(int i = 0; i < 4; i++) {
        if (mbr.particiones[i].start == 0) break;

        fprintf(printer, "<tr>\n");
        fprintf(printer, "<td><b>Estatus de la particion %d</b></td>\n", i+1);
        fprintf(printer, "<td>%c</td>\n", mbr.particiones[i].status);
        fprintf(printer, "</tr>\n");

        fprintf(printer, "<tr>\n");
        fprintf(printer, "<td><b>Tipo de la particion %d</b></td>\n", i+1);
        fprintf(printer, "<td>%c</td>\n", mbr.particiones[i].type);
        fprintf(printer, "</tr>\n");

        fprintf(printer, "<tr>\n");
        fprintf(printer, "<td><b>Ajuste de la particion %d</b></td>\n", i+1);
        fprintf(printer, "<td>%c</td>\n", mbr.particiones[i].fit);
        fprintf(printer, "</tr>\n");

        fprintf(printer, "<tr>\n");
        fprintf(printer, "<td><b>Inicio de la particion %d</b></td>\n", i+1);
        fprintf(printer, "<td>%d</td>\n", mbr.particiones[i].start + 1);
        fprintf(printer, "</tr>\n");

        fprintf(printer, "<tr>\n");
        fprintf(printer, "<td><b>Tamanio de la particion %d</b></td>\n", i+1);
        fprintf(printer, "<td>%d</td>\n", mbr.particiones[i].size);
        fprintf(printer, "</tr>\n");

        fprintf(printer, "<tr>\n");
        fprintf(printer, "<td><b>Nombre de la particion %d</b></td>\n", i+1);
        fprintf(printer, "<td>%s</td>\n", mbr.particiones[i].name);
        fprintf(printer, "</tr>\n");

    }

    fprintf(printer, "</table>\n");
    fprintf(printer, ">\n");
    fprintf(printer, "];}");
    fclose(printer);

    //a partir de aqui lo convierto a pdf
    char* extension = obtenerExtension(direccion);
    char instruccion[1024] = {0};

    sprintf(instruccion, "dot -T%s mbr.dot -o %s", extension, direccion);
    int response = system(instruccion);

    return 1; // el reporte fue creado exitosamente
}

int reporteDisk(char *id, char *direccion, IDLIST *lista) {
    char *path = (char*)malloc(256*sizeof(char));
    path[0] = 0;

    for (int i = 0; i < 400; i++) {
        if (lista->ids[i].id[0] == 0) continue;

        if (!(strcmp(lista->ids[i].id, id))) {
            strcpy(path, lista->ids[i].path);
        }
    }

    if (path[0] == 0) return -1; //no hay una direccion asociada al identificador, baboso

    //a partir de aqui busco el MBR
    MBR mbr;
    FILE *disk;
    quitarComillas(path);
    disk = fopen(path, "rb+");
    if (disk == NULL)
        return -3; // la direccion asociada no existe, baboso, algo perdiste

    fseek(disk, 0, SEEK_SET);
    fread(&mbr, sizeof(MBR), 1, disk);
    fclose(disk);

    int totalSize = mbr.size - sizeof(mbr);

    //a partir de aqui vamos a generar el reporte
    FILE *writer = NULL;
    writer = fopen("genericDisk.dot", "w");

    fprintf(writer, "digraph {\n");
    fprintf(writer, "Nodo [\n");
    fprintf(writer, "shape=plaintext\n");
    fprintf(writer, "label=<\n");
    fprintf(writer, "<table border='0' cellborder='1' cellspacing='0' cellpadding='10'>\n");
    fprintf(writer, "<tr>\n");

    //espacio del mbr
    fprintf(writer, "<td border='1' height='75' width='25'>MBR<br/>%ld bytes</td>\n", sizeof(MBR));

    //resto del disco
    long int bitActual = (long int)sizeof(mbr);

    int i = 0;
    while (bitActual < mbr.size) {
        if (i == 4) { //significa que aun hay un espacio libre despues de la ultima particion
            int dif = mbr.size - bitActual;
            double porcent = ((double)dif/(double)totalSize)*100;
            fprintf(writer, "<td border='1' height='75' width='50'> Libre <br/> %.2f %c</td>\n", porcent, '%');
            bitActual = mbr.size;
        }
        else if (mbr.particiones[i].start == bitActual) {
            double porcentaje = ((double)mbr.particiones[i].size/(double)totalSize)*100;
            char tipo[10] = {0};
            if (mbr.particiones[i].type == 'p') strcpy(tipo, "Primaria");
            else if (mbr.particiones[i].type == 'e') strcpy(tipo, "Extendida");
            fprintf(writer, "<td border='1' height='75' width='50'> %s <br/> %.2f %c</td>\n", tipo, porcentaje, '%');
            bitActual += mbr.particiones[i].size;
            i++;
        }
        else if (mbr.particiones[i].start > bitActual) {
            //este es espacio libre
            long int local = mbr.particiones[i].start - bitActual;
            double porcentaje = ((double)local/(double)totalSize)*100;
            fprintf(writer, "<td border='1' height='75' width='50'> Libre <br/> %.2f %c</td>\n", porcentaje, '%');
            bitActual = mbr.particiones[i].start;
        }
        else if (mbr.particiones[i].start == 0) { //aqui tambien es espacio libre
            long int tamanio = 0;
            if (i == 0) tamanio = totalSize;
            else tamanio = totalSize - (mbr.particiones[i-1].start +
                    mbr.particiones[i-1].size);
            double porcentaje = ((double)tamanio/(double)totalSize)*100;
            fprintf(writer, "<td border='1' height='75' width='50'> Libre <br/> %.2f %c</td>\n", porcentaje, '%');
            break; //rompemos porque media vez encontramos una particion vacia solo hay espacio libre despues
        }
    }

    fprintf(writer, "</tr>\n");
    fprintf(writer, "</table>\n");
    fprintf(writer, ">\n");
    fprintf(writer, "];\n");
    fprintf(writer, "}");
    fclose(writer);

    char* extension = obtenerExtension(direccion);
    char instruccion[1024] = {0};

    sprintf(instruccion, "dot -T%s genericDisk.dot -o %s", extension, direccion);
    int response = system(instruccion);


    return 1; //reporte creado con exito
}
