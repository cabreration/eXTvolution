#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include "parser.h"
#include "respuestas.h"


char** split_string(char * comando, char * divisor) {

    int siz = strlen(comando) + 1;
    char * temp = (char*)malloc(siz*sizeof(char));
    memset(temp, '\0', siz);
    temp = strcpy(temp, comando);
    temp[siz] = 0;
    char** splits = 0;
    char* token = strtok(temp, divisor);

    int espacios = 0;

    while (token) {
        splits = realloc(splits, sizeof(char*)* ++espacios);

        if (!splits) {
            return splits;
        }

        splits[espacios-1] = token;
        token = strtok(NULL, divisor);
    }
    splits = realloc(splits, sizeof(char*) * espacios+1);
    splits[espacios] = 0;
    return splits;
}

int makeDisk(char ** parameters) {
    int size = -1;
    char fit = 'f';
    char unit = 'm';
    char * path = (char*) malloc((256) * sizeof(char));
    memset(path, '\0', 256);
    //path[0] = 0;

    char** runner = parameters;
    while (*runner) {
        char ** value = split_string(*runner, "~:~");
        if (!(strcmp(*value, "-size")) || !(strcmp(*value, "size"))) {
            value++;
            char * ptr;
            size = strtol(*value, &ptr, 10);
        }
        else if (!(strcmp(*value, "-fit")) || !strcmp(*value, "fit")) {
            value++;
            if (!(strcmp(*value, "bf"))) fit = 'b';
            else if (!(strcmp(*value, "wf"))) fit = 'w';
        }
        else if (!(strcmp(*value, "-unit")) || !(strcmp(*value, "unit"))) {
            value++;
            if (!(strcmp(*value, "k"))) unit = 'k';
        }
        else if (!(strcmp(*value, "path")) || !(strcmp(*value, "-path"))) {
            value++;
            strcpy(path, *value);
            reemplazar(path, 38, 32);
            doblesPorSimples(path);
        }
        else {
            printf("el parametro especificado no existe: %s\n", *value);
            return -9;
        }
        runner++;
    }

    if (size == -1)
        return -1;
    if (path[0] == 0)
        return -2;
    if (size < -1)
        return -3;
    if (fit != 'f' && fit != 'w' && fit != 'b')
        return -4;
    if (unit != 'k' && unit != 'm')
        return -5;

    int size2 = 0;
    size2 = strlen(path) + 1;
    char * directory = (char*) malloc((size2*sizeof(char)));
    memset(directory, '\0', size2);
    separarFichero(path, directory);
    doblesPorSimples(directory);
    directory = agregarComillaAlFinal(directory);
    DIR* dir = opendir(directory);
    if (dir)
        closedir(dir); //solo cerramos el directorio
    else if (ENOENT == errno)
    {
        //creamos el directorio
        int des = createHardFile(directory);
        if (des < 0) return -6; // no fue posible crear el directorio
    }
    if (fileExists(path))
        return -7; //ya existe un disco con ese nombre

    int ret = createDisk(path, size, unit, fit);
    if (ret == 0) return -8; // algo salio mal
    /*if (directory) {
        free(directory);
        directory = 0;
    }*/
    return 0;
}

int rmDisk(char** comando) {
    char * path = (char*) malloc((256) * sizeof(char));
    memset(path, '\0', 256);
    char** aux = comando;
    char** par = split_string(*aux, "~:~");
    if ((strcmp(*par, "-path")) != 0|| strcmp(*par, "path") != 0)
        return -1; //el parametro no existe para el comando

    par++;
    strcpy(path, *par);
    reemplazar(path, 38, 32);
    doblesPorSimples(path);

    if (path[0] == 0)
        return -2; //no se ingreso una direccion

    if (fileExists(path) == -1)
        return -3; //no existe el disco

    //aqui lo ejecuto
    int remover = removeDisk(path);
    if (remover < 0)
        return -4; //algo salio mal
    if (remover == 1)
        return -5; //no tiene permisos para realizar esta operacion

    return 1;
}

int fDisk(char** comando) {
    //parametros
    int retorno = 0;

    int size = -1;
    char unit = 'k';
    char * path = (char*) malloc((256) * sizeof(char));
    memset(path, '\0', 256);
    char type = 'p';
    char fit = 'w';
    char delete[7] = "fast";
    char * name = (char*) malloc((20) * sizeof(char));
    memset(name, '\0', 20);
    int add = 0;
    char first = 'g';

    char** runner = comando;
    while (*runner) {
        char** parameter = split_string(*runner, "~:~");
        if (!(strcmp(*parameter, "-size")) || !(strcmp(*parameter, "size"))) {
            parameter++;
            char * ptr;
            size = strtol(*parameter, &ptr, 10);
            if (first == 'g')
                first = 'c';
        }
        else if (!(strcmp(*parameter, "-fit")) || !strcmp(*parameter, "fit")) {
            parameter++;
            if (!strcmp(*parameter, "bf")) fit = 'b';
            else if (!strcmp(*parameter, "ff")) fit = 'f';
            else if (!strcmp(*parameter, "wf")) fit = 'w';
            else fit = 'f';
        }
        else if (!(strcmp(*parameter, "-unit")) || !(strcmp(*parameter, "unit"))) {
            parameter++;
            if (!(strcmp(*parameter, "b"))) unit = 'b';
            else if (!(strcmp(*parameter, "m"))) unit = 'm';
        }
        else if (!(strcmp(*parameter, "path")) || !(strcmp(*parameter, "-path"))) {
            parameter++;
            strcpy(path, *parameter);
            reemplazar(path, 38, 32);
            doblesPorSimples(path);
        }
        else if (!(strcmp(*parameter, "type")) || !(strcmp(*parameter, "-type"))) {
            parameter++;
            if (!(strcmp(*parameter, "e"))) type = 'e';
            else if (!(strcmp(*parameter, "l"))) type = 'l';
        }
        else if (!(strcmp(*parameter, "delete")) || !(strcmp(*parameter, "-delete"))) {
            parameter++;
            if (!(strcmp(*parameter, "full")))
                strcpy(delete, "full");
            if (first == 'g')
                first = 'd';
        }
        else if (!(strcmp(*parameter, "name")) || !(strcmp(*parameter, "-name"))) {
            parameter++;
            strcpy(name, *parameter);
            reemplazar(name, 38, 32);
            quitarComillas(name);
        }
        else if (!(strcmp(*parameter, "add")) || !(strcmp(*parameter, "-add"))) {
            parameter++;
            char * ptr2;

            add = strtol(*parameter, &ptr2, 10);
            if (first == 'g')
                first = 'a';
        }
        else {
            printf("el parametro especificado no existe para la operacion: %s\n", *parameter);
            return -9;
        }
        runner++;
    }

    if (first == 'g')
        return -1; // no ingreso parametros suficientes para ejecutar el comando
    if (path[0] == 0)
        return -2; //el parametro path es obligatorio
    if (name[0] == 0)
        return -3; // el parametro name es obligatorio

    if (first == 'c') { // crear particion
        if (size < 0)
            return -4; //size debe ser positivo
        int success = createPart(path, type, size, name, unit, fit);
        if (success == -1)
            return -5; //el disco sobre el que intenta crear una particion no existe
        else if (success == -2)
            return -6; //no pueden crearse mas de 4 particiones fisicas
        else if (success == -3)
            return -7; //no existe un fragmento con la capacidad para almacenar esa particion con el ajuste actual
        else if (success == -4)
            return -14;
        else if (success == -5)
            return -15;
        else if (success == -6)
            return -16;
        else if (success == 1)
            retorno = 1;
    }
    else if (first == 'a') { // agregar o quitar espacio a una particion
        int modified = modifyPart(path, name, add, unit);
        if (modified == 1)
            retorno = 3;
        else if (modified == -1)
            return -11;
        else if (modified == -2)
            return -12;
        else if (modified == -3)
            return -13;
    }
    else if (first == 'd') { //borrar particion
        if ((strcmp(delete, "fast") != 0) && strcmp(delete, "full") != 0)
            return -8;
        int deleted = deletePart(path, delete, name);
        if (deleted == 1)
            retorno = 2;
        else if (deleted == -1)
            return -9;
        else if (deleted == -2)
            return -10;
    }

    return retorno;
}

int mount(char **comando, IDLIST *lista) {
    char * path = (char*) malloc((256) * sizeof(char));
    memset(path, '\0', 256);
    char * name = (char*) malloc((20) * sizeof(char));
    memset(name, '\0', 20);

    char ** runner = comando;
    while (*runner) {
        char** parameter = split_string(*runner, "~:~");
        if (!(strcmp(*parameter, "-path")) || !(strcmp(*parameter, "path"))) {
            parameter++;
            strcpy(path, *parameter);
            reemplazar(path, 38, 32);
            doblesPorSimples(path);
        }
        else if (!(strcmp(*parameter, "-name")) || !(strcmp(*parameter, "name"))) {
            parameter++;
            strcpy(name, *parameter);
            reemplazar(name, 38, 32);
            quitarComillas(name);
        }
        else return -1; //ingreso un parametro ajeno al comando, abortando operacion
        runner++;
    }

    if (path[0] == 0)
        return -2; //el parametro path es obligatorio para este comando
    if (name[0] == 0)
        return -3; //el parametro name es obligatorio para este comando

    int mot = mountPart(path, name, lista);

    if (mot == -1)
        return -4; //no existe el disco
    else if (mot == -2)
        return -5; //no existe la particion
    else if (mot == -3)
        return -6; //la particion ya estaba activa

    return 1;
}

int unmount(char **comando, IDLIST *lista) {
    char * id = (char*) malloc((10) * sizeof(char));
    memset(id, '\0', 10);

    char ** runner = comando;
    char ** parameter = split_string(*runner, "~:~");
    if (strcmp(*parameter, "-id") && strcmp(*parameter, "id"))
        return -1; //parametro ajeno al comando
    parameter++;
    strcpy(id, *parameter);
    reemplazar(id, 38, 32);
    quitarComillas(id);

    if (id[0] == 0)
        return -3; //el parametro id es obligatorio para este comando

    int umot = unmountPart(id, lista);

    if (umot == -1)
        return -4; //no hay particiones montadas
    else if (umot == -2)
        return -5; //no existe una particion con ese identificador montada

    return 1;
}

int report(char **comando) {
    char * id = (char*)malloc(10*sizeof(char));
    memset(id, '\0', 10);
    char * name = (char*)malloc(5*sizeof(char));
    memset(name, '\0', 5);
    char * path = (char*)malloc(256*sizeof(char));
    memset(path, '\0', 256);

    char ** runner = comando;
    while (*runner) {
        char ** parameter = split_string(*runner, "~:~");
        if (!(strcmp(*parameter, "-id")) || !(strcmp(*parameter, "id"))) {
            parameter++;
            strcpy(id, *parameter);
        }
        else if (!(strcmp(*parameter, "-name")) || !(strcmp(*parameter, "name"))) {
            parameter++;
            strcpy(name, *parameter);
        }
        else if (!(strcmp(*parameter, "-path")) || !(strcmp(*parameter, "path"))) {
            parameter++;
            strcpy(path, *parameter);
            reemplazar(id, 38, 32);
            doblesPorSimples(path);
        }
        else return -1; //parametro ajeno al comando
        runner++;
    }

    if (id[0] == 0) return -2; //id obligatorio
    if (path[0] == 0) return -3; //path obligatorio
    if (name[0] ==0) return -4; //name obligatorio
    if (strcmp(name, "mbr") != 0 && strcmp(name, "disk") != 0
            && strcmp(name, "inode") != 0 && strcmp(name, "journaling") != 0
            && strcmp(name, "block") != 0 && strcmp(name, "bm_inode") != 0
            && strcmp(name, "bm_block") != 0 && strcmp(name, "tree") != 0
            && strcmp(name, "sb") != 0 && strcmp(name, "file") != 0 && strcmp(name, "ls"))
        return -5; //valores incorrectos para name

    int size2 = 0;
    size2 = strlen(path) + 1;
    char * directory = (char*) malloc((size2*sizeof(char)));
    memset(directory, '\0', size2);
    separarFichero(path, directory);
    doblesPorSimples(directory);
    directory = agregarComillaAlFinal(directory);
    DIR* dir = opendir(directory);
    if (dir)
        closedir(dir); //solo cerramos el directorio
    else if (ENOENT == errno)
    {
        //creamos el directorio
        int des = createHardFile(directory);
        if (des < 0) return -6; // no fue posible crear el directorio
    }
    char *upath = (char*)malloc(256*sizeof(char));
    memset(upath, '\0', 256);
    quitarComillas(upath);
    if (fileExists(upath))
        return -7; //ya existe un archivo con ese nombre

    //a partir de aqui lo mando a ejecutar
    int response = clasificarReporte(id, path, name);
    return response;
}

int mkfs(char **comando) {
    char id[10] = {0};
    char type[10] = {0};
    strcpy(type, "full");
    char fs[10] = {0};
    strcpy(fs, "2fs");

    char ** runner = comando;
    while(*runner) {
        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-id")) {
            parametro++;
            strcpy(id, *parametro);
        }
        else if (!strcmp(*parametro, "-type")) {
            parametro++;
            strcpy(type, *parametro);
        }
        else if (!strcmp(*parametro, "-fs")) {
            parametro++;
            strcpy(fs, *parametro);
        }
        runner++;
    }

    if (id[0] = 0) return -1; //no ingreso id
    if (strcmp(type, "fast") != 0 && strcmp(type, "full") != 0) return -2; //valores incorrectos para tipo
    if (strcmp(fs, "3fs") != 0 && strcmp(fs, "2fs") != 0) return -3; //valores incorrectos para fs

    return 1;
}

int login(char **comando) {
    char id[10] = {0};
    char usr[15] = {0};
    char pwd[15] = {0};

    char ** runner = comando;
    while(*runner) {
        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-id")) {
            parametro++;
            strcpy(id, *parametro);
        }
        else if (!strcmp(*parametro, "-usr")) {
            parametro++;
            strcpy(usr, *parametro);
        }
        else if (!strcmp(*parametro, "-pwd")) {
            parametro++;
            strcpy(pwd, *parametro);
        }
        runner++;
    }

    if (id[0] = 0) return -1; //no ingreso id
    if (usr[0] = 0) return -2; //no ingreso usuario
    if (pwd[0] = 0) return -3; //no ingreso password

    return 1;
}

int mkgrp(char **comando) {
    char name[15] = {0};
    char ** parametro = split_string(*comando, "~:~");

    if (strcmp(*parametro, "-name") != 0)
        return -1; //mkgroup toma como unico parametro name

    parametro++;
    strcpy(name, *parametro);

    if (name[0] = 0) return -2; //no ingreso valor para parametro

    return 1;
}

int rmgrp(char **comando) {
    char name[15] = {0};
    char ** parametro = split_string(*comando, "~:~");

    if (strcmp(*parametro, "-name") != 0)
        return -1; //mkgroup toma como unico parametro name

    parametro++;
    strcpy(name, *parametro);

    if (name[0] = 0) return -2; //no ingreso valor para parametro

    return 1;
}

int mkusr(char **comando) {
    char usr[11] = {0};
    char grp[11] = {0};
    char pwd[11] = {0};

    char ** runner = comando;
    while(*runner) {
        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-grp")) {
            parametro++;
            strcpy(grp, *parametro);
        }
        else if (!strcmp(*parametro, "-usr")) {
            parametro++;
            strcpy(usr, *parametro);
        }
        else if (!strcmp(*parametro, "-pwd")) {
            parametro++;
            strcpy(pwd, *parametro);
        }
        runner++;
    }

    if (grp[0] = 0) return -1; //no ingreso grupo
    if (usr[0] = 0) return -2; //no ingreso usuario
    if (pwd[0] = 0) return -3; //no ingreso password

    return 1;
}

int rmusr(char **comando) {
    char usr[11] = {0};
    char ** parametro = split_string(*comando, "~:~");

    if (strcmp(*parametro, "-usr") != 0)
        return -1; //rmusr toma como unico parametro usr

    parametro++;
    strcpy(usr, *parametro);

    if (usr[0] = 0) return -2; //no ingreso valor para parametro

    return 1;
}

int chmod(char **comando) {
    char path[256] = {0};
    char ugo[4] = {0};
    int r = 0;

    char ** runner = comando;
    while(*runner) {
        if (!strcmp(*parametro, "-r")) {
            r = 1;
            runner++;
            continue;
        }

        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-path")) {
            parametro++;
            strcpy(path, *parametro);
        }
        else if (!strcmp(*parametro, "-ugo")) {
            parametro++;
            if (strlen(*parametro) > 3) return -4; //los permisos no concuerdan
            strcpy(ugo, *parametro);
        }
        runner++;
    }

    if (path[0] = 0) return -1; //no ingreso path
    //validar rango de permisos

    return 1;
}

int mkfile(char **comando) {
    char path[256] = {0};
    char cont[128] = {0};
    int p = 0;
    int size = 0;

    char ** runner = comando;
    while(*runner) {
        if (!strcmp(*parametro, "-p")) {
            p = 1;
            runner++;
            continue;
        }

        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-path")) {
            parametro++;
            strcpy(path, *parametro);
        }
        else if (!strcmp(*parametro, "-cont")) {
            parametro++;
            strcpy(cont, *parametro);
        }
        else if (!strcmp(*parametro, "-size")) {
            parametro++;
            char * ptr;
            size = strtol(*parametro, &ptr, 10);
        }
        runner++;
    }

    if (path[0] = 0) return -1; //no ingreso path
    if (size < 0) return -2; //solo pueden ser numeros positivos

    if (cont[0] != 0) size = 0;
    //validar rango de permisos

    return 1;
}

int cat(char **comando) {
    return 1;
}

int rem(char **comando) {
    char path[256] = {0};
    char ** parametro = split_string(*comando, "~:~");

    if (strcmp(*parametro, "-path") != 0)
        return -1; //el unico parametro es path

    parametro++;
    strcpy(path, *parametro);

    if (path[0] = 0) return -2; //no ingreso valor para parametro

    return 1;
}

int edit(char **comando) {
    char path[256] = {0};
    char cont[128] = {0};
    int size = 0;

    char ** runner = comando;
    while(*runner) {
        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-path")) {
            parametro++;
            strcpy(path, *parametro);
        }
        else if (!strcmp(*parametro, "-cont")) {
            parametro++;
            strcpy(cont, *parametro);
        }
        else if (!strcmp(*parametro, "-size")) {
            parametro++;
            char * ptr;
            size = strtol(*parametro, &ptr, 10);
        }
        runner++;
    }

    if (path[0] = 0) return -1; //no ingreso path
    if (size < 0) return -2; //solo pueden ser numeros positivos

    if (cont[0] != 0) size = 0;
    //validar que el path exista, validar que cont exista

    return 1;
}

int ren(char **comando) {
    char path[256] = {0};
    char name[30] = {0};

    char ** runner = comando;
    while (*runner) {
        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-path")) {
            parametro++;
            strcpy(path, *parametro);
        }
        else if (!strcmp(*parametro, "-name")) {
            parametro++;
            strcpy(name, *parametro;)
        }
        else {
            return -1; //el parametro no pertenece al comando
        }
        runner++;
    }

    if (path[0] = 0) return -2; //no ingreso path
    if (name[0] = 0) return -3; //no ingreso name

    //verificar que la direccion exista
    //verificar  que el nombre no se repita

    return 1;
}

int mkdir(char **comando) {
    char id[10] = {0};
    char path[256] = {0};
    int p = 0;

    char ** runner = comando;
    while (*runner) {
        if (!strcmp(*runner, "-p")) {
            p = 1;
            runner++;
            continue;
        }

        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-path")) {
            parametro++;
            strcpy(path, parametro);
        }
        else if (!strcmp(*parametro, "-id")) {
            parametro++;
            strcpy(id, *parametro);
        }
        else {
            return -1; //parametro ajeno al comando
        }
        runner++;
    }

    if (id[0] = 0) return -2; //parametro id obligatorio
    if (path[0] = 0) return -3; //parametro id obligatorio

    if (p == 0) {
        //verificar si hay error en las carpetas padre, si lo hay entonces return -4;
    }

    return 1;
}

int cp(char **comando) {
    char path[256] = {0};
    char dest[256] = {0};

    char ** runner = comando;
    while (*runner) {
        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-path")) {
            parametro++;
            strcpy(path, *parametro);
        }
        else if (!strcmp(*parametro, "-dest")) {
            parametro++;
            strcpy(dest, *parametro);
        }
        else {
            return -1; //parametro ajeno al comando
        }
        runner++;
    }

    if (path[0] = 0) return -2; //path es obligatorio
    if (dest[0] = 0) return -3; //dest es obligatorio

    //verificar que el path exista
    //verificar que dest exista

    return 1;
}

int mv(char **comando) {
    char path[256] = {0};
    char dest[256] = {0};

    char ** runner = comando;
    while (*runner) {
        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-path")) {
            parametro++;
            strcpy(path, *parametro);
        }
        else if (!strcmp(*parametro, "-dest")) {
            parametro++;
            strcpy(dest, *parametro);
        }
        else {
            return -1; //parametro ajeno al comando
        }
        runner++;
    }

    if (path[0] = 0) return -2; //path es obligatorio
    if (dest[0] = 0) return -3; //dest es obligatorio

    //verificar que el path exista
    //verificar que dest exista

    return 1;
}

int find(char **comando) {
    char path[256] = {0};
    char name[30] = {0};

    char ** runner = comando;
    while (*runner) {
        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-path")) {
            parametro++;
            strcpy(path, *parametro);
        }
        else if (!strcmp(*parametro, "-name")) {
            parametro++;
            strcpy(name, *parametro);
        }
        else {
            return -1; //parametro ajeno al comando
        }
        runner++;
    }

    if (path[0] = 0) return -2; //path es obligatorio
    if (name[0] = 0) return -3; //name es obligatorio

    //verificar que path si exista

    return -1;
}

int chown(char **comando) {
    char path[256] = {0};
    char usr[11] = {0};
    int r = 0;

    char ** runner = comando;
    while (*runner) {
        if (!strcmp(*runner, "-r")) {
            r = 1;
            runner++;
            continue;
        }

        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-path")) {
            parametro++;
            strcpy(path, *parametro);
        }
        else if (!strcmp(*parametro, "-usr")) {
            parametro++;
            strcpy(usr, *parametro);
        }
        else {
            return -1; //parametro ajeno al comando
        }
        runner++;
    }

    if (path[0] = 0) return -2; //path es obligatorio
    if (usr[0] = 0) return -3; //usr es obligatorio

    return 1;
}

int chgrp(char **comando) {
    char usr[11] = {0};
    char grp[11] = {0};

    char ** runner = comando;
    while (*runner) {
        char ** parametro = split_string(*runner, "~:~");
        if (!strcmp(*parametro, "-usr")) {
            parametro++;
            strcpy(usr, *parametro);
        }
        else if (!strcmp(*parametro, "-grp")) {
            parametro++;
            strcpy(grp, *parametro);
        }
        else {
            return -1; //parametro ajeno al comando
        }
        runner++;
    }

    if (usr[0] = 0) return -2; //usr es obligatorio
    if (grp[0] = 0) return -3; //grp es obligatorio

    return 1;
}


int convert(char **comando) {
    char id[10] = {0};

    char ** runner = comando;
    char ** parametro = split_string(*runner, "~:~");

    if (strcmp(*parametro, "-id") != 0) {
        return -1; //parametro ajeno al comando
    }

    parametro++;
    strcpy(id, *parametro);

    if (id[0] == 0) return -2; // el valor de id es obligatorio

    return 1;
}

int loss(char **comando) {
    char id[10] = {0};

    char ** runner = comando;
    char ** parametro = split_string(*runner, "~:~");

    if (strcmp(*parametro, "-id") != 0) {
        return -1; //parametro ajeno al comando
    }

    parametro++;
    strcpy(id, *parametro);

    if (id[0] == 0) return -2; // el valor de id es obligatorio

    return 1;
}



