#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

class Scheduler
{
public:
    static void BeginScheduling(void); // Doesn't return - call once per CPU!
    static void EnterFromInterrupt(void);  // Could be preemptive timer, yield or end of thread
};

#endif // __SCHEDULER_H__
