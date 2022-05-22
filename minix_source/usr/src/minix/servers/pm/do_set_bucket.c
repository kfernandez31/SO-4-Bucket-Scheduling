#include <assert.h>
#include <sys/types.h>

#include "kernel/proc.h"
#include "pm.h"


int do_set_bucket(void) {
    int bucket_nr = m_in.m1_i1;

    pid_t pid = TODO;
    struct proc *proc_data = pid.getProc();
    int proc_bucket = TODO;

    if (0 <= bucket_nr && bucket_nr <= NR_BUCKETS - 1) {
        // check if the process is scheduled directly by the kernel
        //TODO: do we also have to check whether the scheduler is in fact sched?
        if (proc_data->p_scheduler == NULL) {
            return EPERM;
        }
        else {
            //TODO...
            return 0;
        }
    }
    else {
        // bucket_nr must be from specified range
        return EINVAL;
    }
}