#include "pm.h"

int do_set_bucket(void) {
    int bucket_nr = m_in.m1_i1;

    if (0 <= bucket_nr && bucket_nr <= NR_BUCKETS - 1) {
        //TODO...
        return 0;
    }

    //TODO...
    if (PROCES.jestSystemowy()) {
        return EPERM;
    }
    return EINVAL;
}