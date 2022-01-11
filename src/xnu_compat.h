#ifndef _XNU_COMPAT_H
#define _XNU_COMPAT_H

#include <kern/locks.h>

#include <sys/errno.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/vnode.h>

// From FreeBSD sys/limits.h
#define OFF_MAX					LONG_MAX
#define OFF_MIN					LONG_MIN

// From XNU sys/proc_internal.h
#define PID_MAX         		99999
#define NO_PID          		100000

// From XNU sys/proc.h (guarded by XNU_KERNEL_PRIVATE)
#define P_WEXIT					0x00002000
#define P_INMEM					0

// From FreeBSD sys/mount.h
#define MNT_ILOCK(mp)		lck_mtx_lock(&mp->mnt_mlock)
#define MNT_IUNLOCK(mp)		lck_mtx_unlock(&mp->mnt_mlock)

// From FreeBSD sys/proc.h 
#define PROC_LOCK(p)				lck_mtx_lock(&(p)->p_mlock)
#define PROC_UNLOCK(p)				lck_mtx_unlock(&(p)->p_mlock)
#define PROC_TRYLOCK(p)				lck_mtx_try_lock(&(p)->p_mlock)
#define PROC_LOCK_ASSERT(p, type)	LCK_MTX_ASSERT(&(p)->p_mlock, (type))

/* Not supported */
#define CTLFLAG_MPSAFE			ENOTSUP
#define MBF_NOWAIT				ENOTSUP
#define MNTK_NOMSYNC			ENOTSUP
#define VV_PROCD				ENOTSUP
#define LK_CANRECURSE			ENOTSUP
#define LK_RETRY				ENOTSUP
#define LK_EXCLUSIVE			ENOTSUP
#define LK_RETRY				ENOTSUP
#define LK_INTERLOCK			ENOTSUP
#define LK_SHARED				ENOTSUP
#define MTX_DUPOK				ENOTSUP
#define _PHOLD(p)
#define PROC_ASSERT_HELD(p)
#define VI_LOCK(vp)
#define VN_LOCK_AREC(vp)

#endif /* _XNU_COMPAT_H */
