/*
 * SegSynchronizationWrapper.h
 *
 *  Created on: 2016年5月3日
 *      Author: epeius
 */

#ifndef SEGSYNCHRONIZATIONWRAPPER_H_
#define SEGSYNCHRONIZATIONWRAPPER_H_

struct SegSynchronizationWrapper;

#ifdef __cplusplus
extern "C" {
#endif
struct SegSynchronizationWrapper *SegSynchronization_GetInstance(void);
void SegSynchronizationWrapper_ReleaseInstance(struct SegSynchronizationWrapper **sSInstance);
int SegSynchronization_initsem(struct SegSynchronizationWrapper *sSInstance, int key);
extern int SegSynchronization_release(struct SegSynchronizationWrapper *sS);
extern int SegSynchronization_acquire(struct SegSynchronizationWrapper *sS);
extern int SegSynchronization_getSemId(struct SegSynchronizationWrapper *sS);
#ifdef __cplusplus
}
;
#endif

#endif /* SEGSYNCHRONIZATIONWRAPPER_H_ */
