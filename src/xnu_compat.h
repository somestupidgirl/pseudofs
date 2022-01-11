#ifndef _XNU_COMPAT_H
#define _XNU_COMPAT_H

#include <kern/locks.h>

#include <sys/errno.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/vnode.h>

// From XNU sys/mount_internal.h
struct mount {
    lck_mtx_t               *mnt_mlock;
    uint32_t                mnt_flag;
    uint32_t                mnt_kern_flag;
    qaddr_t                 mnt_data;
    struct vfsstatfs        mnt_vfsstat;
};

// From XNU sys/mount_internal.h
int kernel_mount(char *, vnode_t, vnode_t, const char *, void *, size_t, int, uint32_t, vfs_context_t);

// From XNU sys/vnode.h (guarded by KERNEL_PRIVATE)
vfs_context_t vfs_context_kernel(void);

// From XNU sys/mount_internal.h
#define KERNEL_MOUNT_NOAUTH 	0x01

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

// FIXME: error: use of undeclared identifier 'M_PFSNODES'
#define M_PFSNODES				ENOTSUP

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
