#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cadenas.h"

char* quitarSaltos(char* cadena) {
    if (*cadena == 0) return 0;
    int size = strlen(cadena) + 1;
    char * temp = cadena;
    for(int i = 0; i < size; i++) {
        if (*(temp + i) == 10 || *(temp + i) == '\r')
            *(temp + i) = 0;
    }
    return cadena;
}

char* minusculizar(char* cadena) {
    if (*cadena == 0) return;
    int i = 0;
    int flag = 1;
    int flag2 = 1;
    while(*(cadena + i)) {
        if (*(cadena + i) >= 'A' && *(cadena + i) <= 'Z'
                && flag) {
                 *(cadena + i) = *(cadena + i) + 32;
        }
        else if (*(cadena + i) == 34 && flag2) {
            flag2 = 0;
        }
        else if (*(cadena + i) == 34 && !flag2) {
            flag2 = 1;
        }
        else if (*(cadena + i) == 32 && flag2) {
            flag = 1;
        }

        if ((*(cadena + i) == 'H' || *(cadena + i) == 'h')
                 && *(cadena + i -1) == 't'
                 && *(cadena + i - 2) == 'a'
                 && *(cadena + i - 3) == 'p')
        {
            flag = 0;
        }
        i++;
    }
    return cadena;
}

void reemplazar(char * original, int off, int on) {
    if (*original == 0) return;
    char * aux = original;
    int flag = 0;
    while (*aux) {
        if (*aux == 34 && flag == 0) {
            flag = 1;
        }
        else if (*aux == 34 && flag == 1) {
            flag = 0;
        }

        if (*aux == off && flag == 1) {
            *aux = on;
        }
        aux++;
    }
}

char * prepararCadena(char* cadena) {
    comentar(cadena);
    quitarSaltos(cadena);
    minusculizar(cadena);
    reemplazar(cadena, 32, 38);
    return cadena;
}

void separarFichero(char * original, char * dir) {
    char * aux = original;
    int contador = 0;
    int espacios = 0;
    if (*aux == 34) espacios = 1;
    while (*aux) {
        if (*aux == 47)
            contador++;
        aux++;
    }

    int contador2 = 0;
    int i = 0;
    while (contador2 < contador && *(original + i)) {
        if (*(original + i) == 47) {
            contador2++;
            if (contador2 == contador) {
                if (espacios) {
                    *(dir + i) = 34;
                    i++;
                }
                *(dir + i) = 0;
                break;
            }
        }
        *(dir + i) = *(original + i);
        i++;
    }
}

void doblesPorSimples(char * original) {
    if (*original != 34) return;
    char *aux = original;
    while (*aux) {
        if (*aux == 34) {
            *aux = 39;
        }
        aux++;
    }
}

void quitarComillas(char * original) {
    if (*original != 39 && *original != 34) return;

    char * aux = original;
    int i = 0;
    while (*(aux + i + 1) != 39 && *(aux + i + 1) != 34) {
        *(original + i) = *(aux+(i+1));
        i++;
    }
    while (*(aux + i)) {
        *(aux + i) = 0;
        i++;
    }
}

char* agregarComillaAlFinal(char *original) {
    if (*original != 39) return original;
    int tamanio = strlen(original) + 2;
    char * nueva = (char*) malloc(tamanio * sizeof(char));
    strcpy(nueva, original);
    nueva[tamanio-2] = 39;
    nueva[tamanio-1] = 0;
    return nueva;
}

void comentar(char * original) {
    char * aux = original;

    while (*aux) {
        if (*aux != 35)
            aux++;
        else break;
    }

    while (*aux) {
        *aux = 0;
        aux++;
    }
}

char* obtenerExtension(char *original) {
    char *extension = (char*)malloc(10*sizeof(char));
    char *aux = original;

    int flag = 0;
    int i = 0;
    while (*aux) {
        if (*aux == '.') flag = 1;
        else if (flag) {
            extension[i] = *aux;
            i++;
        }
        aux++;
    }
    while (extension[i]) extension[i] = 0;
    return extension;
}
