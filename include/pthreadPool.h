#include "task.h"


InitPool();

run()
{
    while(1){
        if(getTaskNum(tl))
        {
            getATask(tl);
        }else{
            sleep(4);
        }
    }
}



putATask(TASKLIST *tl, ExecuteTask runtask, void *arg, unsigned int argsize, int priority);

