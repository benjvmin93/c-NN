#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

#include "files.h"
#include "xmalloc.h"

#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char **getFileNamesFromDir(const char *dataPath)
{
    struct dirent *dirent;
    DIR *dir = opendir(dataPath);

    if (!dir)
        err(errno, "Couldn't open directory `%s`.", dataPath);
    
    size_t nb_files = 0;
    char **fileNames = NULL;

    while ((dirent = readdir(dir)))
    {
        if (dirent->d_type == DT_REG)
        {
            nb_files++;
            fileNames = realloc(fileNames, nb_files * sizeof(char *));
            fileNames[nb_files - 1] = strdup(dirent->d_name);
            if (!fileNames[nb_files - 1])
                err(1, "files.getFleNamesFromDir(): strdup of `%s` failed.", dirent->d_name);
        }
    }

    fileNames = realloc(fileNames, (nb_files + 1) * sizeof(char *));
    fileNames[nb_files] = NULL;

    return fileNames;
}