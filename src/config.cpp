#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef WIN32
#include <sys/stat.h>
#include <fcntl.h>
#else
#include <>
#endif
struct ConfText{
    char *buff;
    FILE *fp;
    int modified;
};
struct ConfText ct = {NULL, NULL, 0};
int loadconf(const char *path)
{
    ct.fp = fopen(path, "rw");
    size_t ready = 0, had = 0;
    int size;
    if(fp)
    {
#ifndef WIN32
        struct stat astat;
        int fd = fileno(fp);
        size = (0 == fstat(fd, &astat)) ? astat.st_size : 0;
#else

#endif
        if(size)
        {
            ct.buff = (char *)malloc(size + 1);
            assert(ct.buff);
            memset(ct.buff, 0, size + 1);
        }
        while(feof(ct.fp))
        {
            ready = fread(ct.buff+ had, 1024, 1, ct.fp);
            had += ready;
        }
        buff[size] = '\0';
    }
}

void releaseconf()
{
    if(ct.buff && ct.fp)
    {
        if(ct.modified){
            fprintf(ct.fp, "%s", buff);
            fflush(ct.fp);
        }
        free(ct.buff);
        ct.buff = NULL;
        fclose(ct.fp);
        ct.fp = NULL;
    }
}

static void parser()
{
    assert(ct.buff);
    char * point = ct.buff;
    char * pointTemp = point;
    char * step = pointTemp;
    while(step)
    {
        step = strchr(pointTemp, '\n');
    }
}

int get_INI_value(const char *section, const char *item, char *value)
{

}
