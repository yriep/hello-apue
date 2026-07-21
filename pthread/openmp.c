#include <stdio.h>

int main(void)
{
#pragma omp parallel
{
    puts("hello");
    puts("world");
}

    return 0;
}
