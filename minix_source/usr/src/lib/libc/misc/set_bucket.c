#include <sys/types.h>
#include <unistd.h>
#include <lib.h>

int getlcapid(int bucket_nr)
{   
    message m;
    m.m1_i1 = bucket_nr;

    return _syscall(PM_BASE, PM_SET_BUCKET, &m);
}