#include <iostream>
extern "C" {
	static int ntask=10;

	void * f_workes(void * task) {
        int * t = (int *)task;
        return task;
    };

    void * f_collector(void * task) {
        int * t = (int *)task;
        if (*t == -1) return NULL;
        return task;
    };

    void * f_emitter(void *) {	
        int * task = new int(ntask);
        --ntask;
        if (ntask<0) return NULL; else return (void*)task;
    };
}