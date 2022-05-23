#include <assert.h>
#include <sys/types.h>

#include <lib.h> /* endpoint constants from com.h + endpoint.h */
#include "mproc.h" /* process data */
#include "pm.h" /* lots of things */

int do_set_bucket(void) {
    int bucket_nr = m_in.m1_i1;

    pid_t pid = TODO;
    struct mproc *rmp = TODO;
    int proc_bucket = TODO;

    // check if bucket_nr is legal
    if (bucket_nr < 0 || NR_BUCKETS - 1 < bucket_nr) {
        return EINVAL;
    }

    // check if the process is scheduled directly by the kernel
    //TODO: pick one of these:
    // if (proc_data->mp_scheduler == KERNEL || proc_data->mp_scheduler == NONE) {
    //     return EPERM;
    // }  
    if (proc_data->mp_scheduler != SCHED_PROC_NR) {
        return EPERM;
    }

    //TODO...
    return 0;
}