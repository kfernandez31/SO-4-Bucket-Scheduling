#include <unistd.h>
#include <lib.h>

int set_bucket(int bucket_nr)
{   
    message m;
    m.m1_i1 = getpid();
    m.m1_i2 = bucket_nr;

    return _syscall(PM_BASE, PM_SET_BUCKET, &m);
}
