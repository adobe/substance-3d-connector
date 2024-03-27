/** @file atomic.h
    @brief Contains some atomic utility operations
    @author Galen Helfter - Allegorithmic
    @date 20181220
    @copyright Allegorithmic. All rights reserved.
*/

#ifndef _SUBSTANCE_CONNECTOR_ATOMIC_H
#define _SUBSTANCE_CONNECTOR_ATOMIC_H

#include <substance/connector/common.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* GNU (and Clang) definitions for atomic operations */
#if defined(SUBSTANCE_CONNECTOR_POSIX) && __GNUC__
/* #NOTE:: __ATOMIC_SEQ_CST is the default memory order in C++11, shown
 * as std::memory_order_seq_cst */
#define CONNECTOR_MEM_ORDER __ATOMIC_SEQ_CST
#define CONNECTOR_ATOMIC_SET_1(ptr) __atomic_store_n(&(ptr), 0x01u, CONNECTOR_MEM_ORDER)
#define CONNECTOR_ATOMIC_SET_0(ptr) __atomic_store_n(&(ptr), 0x00u, CONNECTOR_MEM_ORDER)
#define CONNECTOR_ATOMIC_OR(ptr,val,ret) ((ret) = __atomic_fetch_or(&(ptr), (val),\
                                                               CONNECTOR_MEM_ORDER))
#define CONNECTOR_ATOMIC_AND(ptr,val,ret) ((ret) = __atomic_fetch_and(&(ptr), (val),\
                                                                 CONNECTOR_MEM_ORDER))
#define CONNECTOR_ATOMIC_COMPARE_EXCHANGE(ptr,x,y,ret) \
    {\
        (ret)=(x);\
        __atomic_compare_exchange_n(&(ptr),&(ret),(y),0,\
                                    CONNECTOR_MEM_ORDER,CONNECTOR_MEM_ORDER);\
    }
/* Windows MSVC atomic operations */
#elif defined(SUBSTANCE_CONNECTOR_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 0x01
#endif
#include <windows.h>
#define CONNECTOR_ATOMIC_SET_1(ptr) InterlockedOr(&(ptr), 0x01u)
#define CONNECTOR_ATOMIC_SET_0(ptr) InterlockedAnd(&(ptr), 0x00u)
#define CONNECTOR_ATOMIC_OR(ptr,val,ret) ((ret) = InterlockedOr(&(ptr), (val)))
#define CONNECTOR_ATOMIC_AND(ptr,val,ret) ((ret) = InterlockedAnd(&(ptr), (val)))
#define CONNECTOR_ATOMIC_COMPARE_EXCHANGE(ptr,x,y,ret) \
            ((ret) = InterlockedCompareExchange(&(ptr), (y), (x)))
/* Allow override to default C operations if the atomics do not exist */
#elif defined(SUBSTANCE_CONNECTOR_NO_ATOMIC)
#define CONNECTOR_ATOMIC_SET_1(ptr) ((ptr) = 1u)
#define CONNECTOR_ATOMIC_SET_0(ptr) ((ptr) = 0u)
#define CONNECTOR_ATOMIC_OR(ptr,val,ret) ((ret) = ((ptr) |= (val)))
#define CONNECTOR_ATOMIC_AND(ptr,val,ret) ((ret) = ((ptr) &= (val)))
#define CONNECTOR_ATOMIC_COMPARE_EXCHANGE(ptr,x,y,ret) \
             if((ptr) == (x)){(ret) = (ptr); (ptr) = (y);}
/* Set compiler error if no atomic implementations found and it hasn't been
 * overridden at the compiler level */
#else
#error "Could not determine atomic operations for the current platform"
#endif

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_CONNECTOR_ATOMIC_H */
