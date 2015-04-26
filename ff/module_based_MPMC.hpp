/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*!
 * \link
 * \file module_based_MPMC.hpp
 * \ingroup streaming_network_arbitrary_shared_memory
 *
 * \breif TODO
 *
 * It defines static linked list Single-Writer Single-Reader unbounded queue.
 * No lock is needed around pop and push methods.
 */

#ifndef __FF_MODULE_BASED_MPMC_HPP_
#define __FF_MODULE_BASED_MPMC_HPP_

/* ***************************************************************************
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License version 3 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 ****************************************************************************
 */

/*
 *
 * -- Massimiliano Meneghin: themaxmail@gmail.com
 */

#include <stdlib.h>
#include <ff/buffer.hpp>
#include <ff/sysdep.h>
#include <assert.h>

#if defined(__GNUC__)
#if !defined(likely)
#define likely(x)       __builtin_expect((x),1)
#endif
#if !defined(unlikely)
#define unlikely(x)     __builtin_expect((x),0)
#endif
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif 

#define CAST_TO_UL(X) ((unsigned long)X)
#define CAST_TO_VUL(X) *((volatile unsigned long * )&X)

namespace ff {

/*!
 *
 * \ingroup streaming_network_arbitrary_shared_memory
 *
 * @{
 */
    __thread void*  ttarget_rec_p;

    __thread unsigned long ttarget_push_old;
    __thread unsigned long ttarget_pop_old;
    __thread unsigned long ttarget_push;
    __thread unsigned long ttarget_pop;

#ifndef _max_
#define _max_( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

/*
 * Compute the smallest power of 2 bigger than the given number
 *
 * exemples:
 *                      1->2
 *                      6->8
 *                      4->8
 */
#if !defined(NEXT_SMALLEST_2_POW)
#define NEXT_SMALLEST_2_POW(A) (1 << (32 - __builtin_clz(A)))
#endif

/*!
 *
 * \class module_based_MPMC
 * \ingroup streaming_network_arbitrary_shared_memory
 *
 * \breif TODO
 *
 * This class is defined in \ref module_based_MPMC.hpp
 */
template<class spsc_queue>
class module_based_MPMC {
private:

    /**
     * TODO
     */
    typedef struct {
        unsigned long target_snd_id;
        unsigned long padding[5];
        unsigned long target_rcv_id;
    } centralized_info_t;

    /**
     * TODO
     */
    typedef struct {
        spsc_queue * q;
        centralized_info_t spsc_counters;
    } entry_info_t;

private:
    entry_info_t * info_v;
    unsigned int qnum;
    centralized_info_t sync_info;
    unsigned long module_mask;
public:
    /**
     * TODO
     */
    module_based_MPMC(int prod_num, int cons_num) {

        qnum = (_max_(prod_num, cons_num)*3);
        qnum = NEXT_SMALLEST_2_POW(qnum);

        unsigned long starting_point = 0;
        //starting_point -= qnum * 1000;

        if(starting_point%qnum != 0){
            abort();
        }
        
        sync_info.target_rcv_id = starting_point;
        sync_info.target_snd_id = starting_point;

        info_v = new entry_info_t[qnum];
        module_mask = qnum-1;

        for (unsigned int i = 0; i < qnum; i++) {
            //info_v[i].q = new spsc_queue(1024, true);
            info_v[i].q = new spsc_queue(1024, true);
            if (!(info_v[i].q)->init()) abort();
            info_v[i].spsc_counters.target_rcv_id = i+starting_point;
            info_v[i].spsc_counters.target_snd_id = i+starting_point;
        }
    }

    /**
     * TODO
     */
    void push(void * p) {
        unsigned long circular_index = __sync_fetch_and_add(
        (unsigned long *) (&sync_info.target_snd_id), 1UL);
        ttarget_push_old = ttarget_push;
        ttarget_push = circular_index;
        ttarget_pop_old = 0;
        ttarget_pop = 0;
        //printf("push in id %lu\n", circular_index);
        //unsigned long q_index = circular_index % qnum;
        //unsigned long pp = circular_index& ((qnum-1));
        unsigned long q_index = circular_index&module_mask;
        //printf("p %d %d %lu %lu\n", qnum, ((qnum-1)),pp, q_index);
        do {/*NOTHING*/
            ;
        } while ((*(volatile unsigned long *) (&info_v[q_index].spsc_counters.target_snd_id)) != circular_index);
        do {/*NOTHING*/
            ;
        } while (info_v[q_index].q->push(p) != true);
        __sync_synchronize();
        info_v[q_index].spsc_counters.target_snd_id += qnum;
        return;
    }

    /**
     * TODO
     */
    bool pop(void ** data) {
        unsigned long circular_index = __sync_fetch_and_add((unsigned long *) (&sync_info.target_rcv_id), 1UL);

        ttarget_push = 0;
        ttarget_push_old = 0;
        ttarget_pop_old = ttarget_pop;
        ttarget_pop = circular_index;
        ttarget_rec_p = *data;
        //unsigned long q_index = circular_index % qnum;
        unsigned long q_index = circular_index&module_mask;
        do {/*NOTHING*/
            ;
        } while ((*(volatile unsigned long *) (&info_v[q_index].spsc_counters.target_rcv_id)) != circular_index);
        
        do {/*NOTHING*/
            ;
        } while (info_v[q_index].q->pop(data) == false);
        __sync_synchronize();
        info_v[q_index].spsc_counters.target_rcv_id += qnum;
        return true;
    }
};

/*!
 * @}
 * \endlink
 */
} // namespace ff

#endif /* MODULE_BASED_MPMC */
