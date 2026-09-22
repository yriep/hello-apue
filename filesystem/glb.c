#include <stdio.h>
#include <glob.h>


#define PAT    "/etc/*.conf"


int main(void)
{
    glob_t globuf;

    glob(PAT, 0, NULL, &globuf);

    for (int i = 0; i < globuf.gl_pathc; i++) {
        printf("%s\n", globuf.gl_pathv[i]);
    }
    globfree(&globuf);
    return 0;
}
