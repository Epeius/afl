/*
 * SegSynchronizationWrapper.cpp
 *
 *  Created on: 2016年5月3日
 *      Author: epeius
 */
#include <stdlib.h>
#include "SegSynchronization.h"
#include "SegSynchronizationWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SegSynchronizationWrapper
{
    SegSynchronization SegSynch;
};

struct SegSynchronizationWrapper *SegSynchronizationWrapper_GetInstance(void)
{
    //return new struct SegSynchronizationWrapper;
    return (SegSynchronizationWrapper *) malloc(sizeof(struct SegSynchronizationWrapper));
}

void SegSynchronizationWrapper_ReleaseInstance(struct SegSynchronizationWrapper **ppInstance)
{
    free(*ppInstance);
    *ppInstance = 0;
}

int SegSynchronization_initsem(struct SegSynchronizationWrapper *sSInstance, int key)
{
    return sSInstance->SegSynch.initsem(key);
}
int SegSynchronization_release(struct SegSynchronizationWrapper *sSInstance)
{
    return sSInstance->SegSynch.release();
}
int SegSynchronization_acquire(struct SegSynchronizationWrapper *sSInstance)
{
    return sSInstance->SegSynch.acquire();
}
int SegSynchronization_getSemId(struct SegSynchronizationWrapper *sSInstance)
{
    return sSInstance->SegSynch.getSemId();
}

#ifdef __cplusplus
};
#endif
