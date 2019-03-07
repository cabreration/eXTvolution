#include "respuestas.h"
#include <stdio.h>

void respuestasMkDisk(int respuesta) {
    if (respuesta == -7)
        printf("Ya existe un disco con ese nombre en la ubicacion especificada");
    else if (respuesta == -6)
        printf("Algo salio mal, no fue posible crear el directorio especificado");
    else if (respuesta == -5)
        printf("El parametro unit solo puede tomar los valores: k, m");
    else if (respuesta == -4)
        printf("El parametro fit solo puede tomar los valores: f, w, b");
    else if (respuesta == -3)
        printf("El valor del parametro size debe ser positivo, imposible trabajar con valor negativo");
    else if (respuesta == -2)
        printf("El parametro path es obligatorio, debe ingresarlo");
    else if (respuesta == -1)
        printf("El parametro size es obligatorio, debe ingresarlo");
    else if (respuesta == -8)
        printf("Algo salio mal durante la operacion del sistema");
    else if (respuesta == 0)
        printf("Disco creado con exito");
    else
        printf("Operacion finalizada sin exito");
}

void respuestasRmDisk(int respuesta) {
    if (respuesta == -5)
        printf("no cuenta con permiso para realizar esta operacion");
    else if (respuesta == -4)
        printf("La eliminacion del disco ha sido cancelada, disco no eliminado");
    else if (respuesta == -3)
        printf("El disco que intenta borrar no existe");
    else if (respuesta == -2)
        printf("El parametro path es obligatorio, sin este no puede usar el comando");
    else if (respuesta == - 1)
        printf("Ha ingresado un parametro que no es parte de la sintaxis del comando, abortando operacion");
    else
        printf("El disco ha sido eliminado");
}

void respuestasFDisk(int respuesta) {
    if (respuesta == -1)
        printf("La combinacion de parametros ingresada no es suficiente para ejecutar el comando en ninguna de sus variantes");
    else if (respuesta == -2)
        printf("El parametro path es obligatorio para el comando fdisk");
    else if (respuesta == -3)
        printf("El parametro name es obligatorio para el comando fdisk");
    else if (respuesta == -4)
        printf("El parametro size debe ser siempre positivo");
    else if (respuesta == -5)
        printf("El disco sobre el que intenta crear una particion no existe");
    else if (respuesta == -6)
        printf("Ya existen 4 particiones fisicas en el disco, no es posible crear mas");
    else if (respuesta == -7)
        printf("No existe un fragmento en el disco con la capacidad para almacenar la particion que intenta crear con el ajuste actual");
    else if (respuesta == -8)
        printf("El parametro delete solo acepta como valores fast o full, abortando operacion");
    else if (respuesta == -9)
        printf("No existe el disco sobre el que intenta borrar una particion");
    else if (respuesta == -10)
        printf("No existe la particion que intenta borrar");
    else if (respuesta == -11)
        printf("No existe el disco sobre el que intenta modificar una particion");
    else if (respuesta == -12)
        printf("No existe la particion que intenta modificar");
    else if (respuesta == -13)
        printf("No existe espacio junto a la particion para extenderla");
    else if (respuesta == -14)
        printf("No puede crear una particion logica si no existe una particion extendida en el disco");
    else if (respuesta == -15)
        printf("No puede crear mas de una particion extendida en un disco");
    else if (respuesta == -16)
        printf("Ya existe una particion con ese nombre en el disco");
    else if (respuesta == 0)
        printf("Algo salio mal pero no se que es prro, el comando fallo");
    else if (respuesta == 1)
        printf("La particion fue creada con exito");
    else if (respuesta == 2)
        printf("La particion ha sido borrada");
    else if (respuesta == 3)
        printf("El tamanio de la particion fue modificado con exito");
}

void respuestasMount(int respuesta) {
    if (respuesta == -1)
        printf("Ingreso un parametro ajeno al comando, abortando operacion");
    else if (respuesta == -2)
        printf("El parametro path es obligatorio");
    else if (respuesta == -3)
        printf("El parametro name es obligatorio");
    else if (respuesta == -4)
        printf("El disco sobre el que intenta montar una particion no existe");
    else if (respuesta == -5)
        printf("La particion que desea montar no existe");
    else if (respuesta == -6)
        printf("La particion ya esta montada");
    else if (respuesta)
        printf("La particion fue montada");
}

void respuestasUnmount(int respuesta) {
    if (respuesta == -1)
        printf("Ingreso un parametro ajeno al comando, abortando operacion");
    else if (respuesta == -3)
        printf("El parametro id es obligatorio");
    else if (respuesta == -4)
        printf("No hay particiones montadas");
    else if (respuesta == -5)
        printf("No existe un particion con ese identificador montada");
    else if (respuesta)
        printf("La particion fue desmontada");
}

void respuestasReport(int respuesta) {
    if (respuesta == -1)
        printf("Ingreso un parametro ajeno al comando");
    else if (respuesta == -2)
        printf("El parametro id es obligatorio");
    else if (respuesta == -3)
        printf("El parametro path es obligatorio");
    else if (respuesta == -4)
        printf("El parametro name es obligatorio");
    else if (respuesta == -5)
        printf("Name solo puede tomar los valores mbr y disk");
    else if (respuesta == -6)
        printf("No fue posible crear el directorio");
    else if (respuesta == -7)
        printf("Ya existe un archivo con ese nombre");
    else if (respuesta == -8)
        printf("");
    else if(respuesta == -9)
        printf("");
    else if (respuesta == -10)
        printf("");
    else if (respuesta == 2)
        printf("El reporte Mbr ha sido creado con exito");
    else if (respuesta == 3)
        printf("El reporte Disk ha sido creado con exito");
}

