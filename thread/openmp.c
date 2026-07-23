#include <stdio.h>
// 要加上-fopenmp
int main(void)
{
#pragma omp parallel
{
    puts("hello");
    puts("world");
}

    return 0;
}
