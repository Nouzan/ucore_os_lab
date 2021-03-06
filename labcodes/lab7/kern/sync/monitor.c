#include <stdio.h>
#include <monitor.h>
#include <kmalloc.h>
#include <assert.h>


// Initialize monitor.
void     
monitor_init (monitor_t * mtp, size_t num_cv) {
    int i;
    assert(num_cv>0);
    mtp->next_count = 0;
    mtp->cv = NULL;
    sem_init(&(mtp->mutex), 1); //unlocked
    sem_init(&(mtp->next), 0);
    mtp->cv =(condvar_t *) kmalloc(sizeof(condvar_t)*num_cv);
    assert(mtp->cv!=NULL);
    for(i=0; i<num_cv; i++){
        mtp->cv[i].count=0;
        sem_init(&(mtp->cv[i].sem),0);
        mtp->cv[i].owner=mtp;
    }
}

// Unlock one of threads waiting on the condition variable. 
void 
cond_signal (condvar_t *cvp) {
   //LAB7 EXERCISE1: YOUR CODE
   cprintf("cond_signal begin: cvp %x, cvp->count %d, cvp->owner->next_count %d\n", cvp, cvp->count, cvp->owner->next_count);  
  /*
   *      cond_signal(cv) {
   *          if(cv.count>0) {
   *             mt.next_count ++;
   *             signal(cv.sem);
   *             wait(mt.next);
   *             mt.next_count--;
   *          }
   *       }
   */
// ------------------------------ lab7的方案 ----------------------------------
//    if (cvp->count > 0) {
//        cvp->owner->next_count ++;
//        up(&cvp->sem);                // 发出信号并唤醒一个waiter
//        down(&cvp->owner->next);      // 进入睡眠
//        cvp->owner->next_count --;
//    }
// ---------------------------------------------------------------------------

   if (cvp->count > 0) {
       cvp->count --;
       up(&cvp->sem);
   }

   cprintf("cond_signal end: cvp %x, cvp->count %d, cvp->owner->next_count %d\n", cvp, cvp->count, cvp->owner->next_count);
}

// Suspend calling thread on a condition variable waiting for condition Atomically unlocks 
// mutex and suspends calling thread on conditional variable after waking up locks mutex. Notice: mp is mutex semaphore for monitor's procedures
void
cond_wait (condvar_t *cvp) {
    //LAB7 EXERCISE1: YOUR CODE
    cprintf("cond_wait begin:  cvp %x, cvp->count %d, cvp->owner->next_count %d\n", cvp, cvp->count, cvp->owner->next_count);
   /*
    *         cv.count ++;
    *         if(mt.next_count>0)
    *            signal(mt.next)
    *         else
    *            signal(mt.mutex);
    *         wait(cv.sem);
    *         cv.count --;
    */
// ------------------------------ lab7的方案 ----------------------------------
    // cvp->count ++;
    // if (cvp->owner->next_count > 0) {  // 判断是否有signaler线程处于睡眠状态(在释放锁之前，必须确保所有的signaler线程退出)
    //     up(&cvp->owner->next);          // 唤醒一个signaler
    // } else {
    //     up(&cvp->owner->mutex);         // 释放锁
    // }
    // down(&cvp->sem);                    // 睡眠并等待信号
    // cvp->count --;
// ---------------------------------------------------------------------------

    cvp->count ++;
    up(&cvp->owner->mutex);
    down(&cvp->sem);
    down(&cvp->owner->mutex);

    cprintf("cond_wait end:  cvp %x, cvp->count %d, cvp->owner->next_count %d\n", cvp, cvp->count, cvp->owner->next_count);
}

/* 
 * 不使用信号量的方案：
 * 需要另外实现一个等待队列
 * signal: 如果count > 0, 则从等待队列中唤醒1个线程，并将count--
 * wait: count ++，然后 先 把自己丢入等待队列 后 释放锁(确保原子性)，然后主动调度，回来后重新取得锁
 */
