#include "structures.h"

char** split_string(char * comando, char * divisor);

int makeDisk(char ** parameters);

int rmDisk(char** comando);

int fDisk(char** comando);

int mount(char** comando, IDLIST *lista);

int unmount(char** comando, IDLIST *lista);

int report(char** comando);

int mkfs(char **comando, IDLIST *lista);

int login(char **comando);

int mkgrp(char **comando);

int rmgrp(char **comando);

int mkusr(char **comando);

int rmusr(char **comando);

int chmod(char **comando);

int mkfile(char **comando);

int cat(char **comando);

int rem(char **comando);

int edit(char **comando);

int ren(char **comando);

int mkdir(char **comando);

int cp(char **comando);

int mv(char **comando);

int find(char **comando);

int chown(char **comando);

int chgrp(char **comando);

int convert(char **comando);

int loss(char **comando);


