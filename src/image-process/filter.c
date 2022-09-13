#define _GNU_SOURCE

#include "filter.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>
#include <errno.h>

int filter[3][3] = { { 0, 0, 0 },
                     { 0, 0, 0 },
                     { 0, 0, 0 } 
                    };

void print_matrix()
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            printf("%d", filter[i][j]);
            if (j < 2)
                printf(" ");
            else
                printf("\n");
        }
    }
    printf("\n");
}

char *toString()
{
    char *str = malloc((3 * 3) + 1);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            printf("Filter: %d\n", filter[i][j]);
            str[i * 3 + j] = filter[i][j] + '0';
        }
    }
    str[9] = 0;
    return str;
}

void clear_matrix()
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            filter[i][j] = 0;
        }
    }
    printf("Matrix cleared.\n");
    print_matrix();
}


void fill_matrix()
{
    srand(time(0));
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            filter[i][j] = rand() & 1;
        }
    }
    printf("Matrix randomly filled:\n");
    print_matrix();
}

void copyFromFile(const char *path, int index)
{
    if (index > 10 || index < 1) 
        return;
    
    FILE *file = fopen(path, "r");

    if (!file)
        err(1, "Cannot open file %s", path);
    
    if (fseek(file, 10 * (index - 1), SEEK_SET) != 0)
        err(errno, "Fseek failed at index %d.", index);
    
    char *line = NULL;
    size_t len = 0;
    int i = 0;
    int j = 0;
    if (getline(&line, &len, file) != -1)
    {
        clear_matrix();
        for (int k = 0; line[k]; ++k)
        {
            if (j == 3)
            {
                j = 0;
                i++;
            }
            if (i == 3)
                break; // Never goes here.
            if (line[k] == '1')
                filter[i][j] = 1;
            j++;
        }
    }

    print_matrix();
}

void copyToFile(const char *path, int index)
{
    char *str = toString();

    if (index > 10)
    {
        free(str);
        return;
    }

    FILE *file = fopen(path, "r+");
    
    if (!file)
        err(1, "Cannot open file %s", path);
    
    if (index != -1 && index <= 10)
    {
       /* if (index == 1)
        {
            if (fseek(file, 0, SEEK_SET) != 0)
                err(errno, "Index = 1, Fseek failed at index %d.", index);
        }*/
        // else
        //{
            if (fseek(file, 10 * (index - 1), SEEK_SET) != 0)
                err(errno, "Fseek failed at index %d.", index);
        //}

        fprintf(file, "%s\n", str);
    }
    else if (index - 1 < 10)
    {
        if (fseek(file, 0, SEEK_END) != 0)
            err(errno, "Fseek at end failed.");

        fprintf(file, "%s\n", str);
    }
    fclose(file);
    free(str);
}

/*
int main(void)
{
    print_matrix();
    fill_matrix();

    copyToFile("test", 7);
    copyFromFile("test", 4);
    return 0;
}
*/