#ifndef _XNU_COMPAT_H
#define _XNU_COMPAT_H

#include <i386/simple_lock.h>
#include <kern/locks.h>

#include <sys/cdefs.h>
#include <sys/errno.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/types.h>
#include <sys/vnode.h>
#include <sys/vnode_if.h>

// FIXME: error: use of undeclared identifier 'M_PFSNODES'
// Specific to pseudofs
#define M_PFSNODES                  ENOTSUP

// FIXME: error: use of undeclared identifier 'M_PFSVNCACHE'
// Specific to pseudofs
#define M_PFSVNCACHE                ENOTSUP

//From FreeBSD sys/limits.h - redefined in accordance with XNU
#define OFF_MAX                     LONG_MAX
#define OFF_MIN                     LONG_MIN

// From XNU osfmk/kern/printf.c, since we don't have FreeBSD include/ctype.h
#define isdigit(d) ((d) >= '0' && (d) <= '9')

/*
 * internal mount, proc and vnode structures
 *
 * mostly derived from XNU:
 *      sys/mount_internal.h
 *      sys/proc_internal.h
 *      sys/vnode_internal.h
 */

struct mount {
    lck_mtx_t                       *mnt_mlock;
    uint32_t                         mnt_flag;
    uint32_t                         mnt_kern_flag;
    qaddr_t                          mnt_data;
    struct vfsstatfs                 mnt_vfsstat;
};

struct proc {
    lck_mtx_t                       *p_mlock;
    pid_t                            p_pid;
    unsigned int                     p_flag;
    u_int                            p_lock; // FreeBSD
};

struct vnode {
    lck_mtx_t                       *v_lock;
    uint32_t                         v_flag;
    int32_t                          v_usecount;
    uint16_t                         v_type;
    mount_t                          v_mount;
    void                            *v_data;
    int(**v_op)(void *);
#if !defined(MACH_KERNEL_PRIVATE) && !defined(__APPLE_API_PRIVATE)
#define MACH_KERNEL_PRIVATE && __APPLE_API_PRIVATE
    simple_lock_data_t               v_interlock; // Darwin 0.3
#endif
};


/*
 * ===== mount =====
 */

// From XNU sys/mount_internal.h
#define KERNEL_MOUNT_NOAUTH         0x01

// From XNU sys/mount_internal.h
int kernel_mount(char *, vnode_t, vnode_t, const char *, void *, size_t, int, uint32_t, vfs_context_t);

// From FreeBSD sys/mount.h - redefined in accordance with XNU
#define MNT_ILOCK(mp)               lck_mtx_lock(&mp->mnt_mlock)
#define MNT_IUNLOCK(mp)             lck_mtx_unlock(&mp->mnt_mlock)


/*
 * ===== proc =====
 */

// From XNU sys/proc_internal.h
#define PID_MAX                     99999
#define NO_PID                      100000

// From XNU sys/proc.h (guarded by XNU_KERNEL_PRIVATE)
#define P_WEXIT                     0x00002000
#define P_INMEM                     0

// From FreeBSD sys/proc.h - redefined in accordance with XNU
#define PROC_LOCK(p)                lck_mtx_lock(&(p)->p_mlock)
#define PROC_UNLOCK(p)              lck_mtx_unlock(&(p)->p_mlock)
#define PROC_TRYLOCK(p)             lck_mtx_try_lock(&(p)->p_mlock)
#define PROC_LOCK_ASSERT(p, type)   LCK_MTX_ASSERT(&(p)->p_mlock, (type))

#define PHOLD(p) do {                                            \
    PROC_LOCK(p);                                                \
    _PHOLD(p);                                                   \
    PROC_UNLOCK(p);                                              \
} while (0)
#define _PHOLD(p) do {                                           \
    PROC_LOCK_ASSERT((p), LCK_MTX_ASSERT_OWNED);                 \
    KASSERT(!((p)->p_flag & P_WEXIT) || (p) == curproc,          \
        ("PHOLD of exiting process %p", p));                     \
    (p)->p_lock++;                                               \
    if (((p)->p_flag & P_INMEM) == 0)                            \
        faultin((p));                                            \
} while (0)
#define _PHOLD_LITE(p) do {                                      \
    PROC_LOCK_ASSERT((p), LCK_MTX_ASSERT_OWNED);                 \
    KASSERT(!((p)->p_flag & P_WEXIT) || (p) == curproc,          \
        ("PHOLD of exiting process %p", p));                     \
    (p)->p_lock++;                                               \
} while (0)
#define PROC_ASSERT_HELD(p) do {                                 \
    KASSERT((p)->p_lock > 0, ("process %p not held", p));        \
} while (0)

// From XNU sys/proc_internal.h
extern int nprocs, maxproc;

// Based on FreeBSD vm/vm_swapout_dummy.c - Needed by _PHOLD
static __inline void faultin(struct proc *p)
{
    PROC_LOCK_ASSERT(p, LCK_MTX_ASSERT_OWNED);
    if ((p->p_flag & P_INMEM) == 0)
        panic("faultin: proc %p swapped out with NO_SWAPPING", p);
}


/*
 * ===== vnode =====
 */

// From XNU sys/vnode_internal.h
#define VROOT                       0x000001

// From FreeBSD sys/vnode.h - redefined in accordance with XNU
#define VV_ROOT                     VROOT

// From FreeBSD sys/vnode.h - redefined in accordance with XNU
#if !defined(MACH_KERNEL_PRIVATE) && !defined(__APPLE_API_PRIVATE)
#define MACH_KERNEL_PRIVATE && __APPLE_API_PRIVATE
#define VI_LOCK(vp)                 lck_mtx_lock(&(vp)->v_interlock)
#endif

#ifndef KERNEL
#define KERNEL

// From XNU sys/vnode_internal.h
#define VOCALL(OPSV, OFF, AP)       (( *((OPSV)[(OFF)])) (AP))
#define VCALL(VP, OFF, AP)          VOCALL((VP)->v_op,(OFF),(AP))
#define VDESC(OP)                   (& __CONCAT(OP,_desc))
#define VOFFSET(OP)                 (VDESC(OP)->vdesc_offset)

// From Darwin 0.3 sys/vnode_if.h
struct vop_lock_args {
    struct vnodeop_desc *a_desc;
    struct vnode *a_vp;
    int a_flags;
    struct proc *a_p;
};
extern struct vnodeop_desc vop_lock_desc;
#define VOP_LOCK(vp, flags, p) _VOP_LOCK(vp, flags, p)
static __inline int _VOP_LOCK(vp, flags, p)
    struct vnode *vp;
    int flags;
    struct proc *p;
{
    struct vop_lock_args a;
    a.a_desc = VDESC(vop_lock);
    a.a_vp = vp;
    a.a_flags = flags;
    a.a_p = p;
    return (VCALL(vp, VOFFSET(vop_lock), &a));
}

// From Darwin 0.3 sys/vnode_if.h
struct vop_unlock_args {
    struct vnodeop_desc *a_desc;
    struct vnode *a_vp;
    int a_flags;
    struct proc *a_p;
};
extern struct vnodeop_desc vop_unlock_desc;
#define VOP_UNLOCK(vp, flags, p) _VOP_UNLOCK(vp, flags, p)
static __inline int _VOP_UNLOCK(vp, flags, p)
    struct vnode *vp;
    int flags;
    struct proc *p;
{
    struct vop_unlock_args a;
    a.a_desc = VDESC(vop_unlock);
    a.a_vp = vp;
    a.a_flags = flags;
    a.a_p = p;
    return (VCALL(vp, VOFFSET(vop_unlock), &a));
}

// From Darwin 0.3 sys/vnode_if.h
struct vop_islocked_args {
    struct vnodeop_desc *a_desc;
    struct vnode *a_vp;
};
extern struct vnodeop_desc vop_islocked_desc;
#define VOP_ISLOCKED(vp) _VOP_ISLOCKED(vp)
static __inline int _VOP_ISLOCKED(vp)
    struct vnode *vp;
{
    struct vop_islocked_args a;
    a.a_desc = VDESC(vop_islocked);
    a.a_vp = vp;
    return (VCALL(vp, VOFFSET(vop_islocked), &a));
}

#endif /* KERNEL */

// From XNU sys/vnode_internal.h
void vnode_lock(vnode_t);

// From XNU sys/vnode.h (guarded by KERNEL_PRIVATE)
vfs_context_t vfs_context_kernel(void);

// From XNU sys/vnode.h (guarded by BSD_KERNEL_PRIVATE)
int vaccess(mode_t file_mode, uid_t uid, gid_t gid, mode_t acc_mode, kauth_cred_t cred);

// From XNU sys/vnode_if.h (guarded by XNU_KERNEL_PRIVATE)
extern errno_t VNOP_GETATTR(vnode_t, struct vnode_attr *, vfs_context_t);

// From FreeBSD sys/vnode.h
static __inline int vrefcnt(struct vnode *vp)
{
    return (vp->v_usecount);
}

// From FreeBSD sys/vnode.h
int     vaccess __P((mode_t file_mode, uid_t uid, gid_t gid, mode_t acc_mode, struct ucred *cred));
int     vget __P((struct vnode *vp, int lockflag, struct proc *p));
void    vgone __P((struct vnode *vp));
void    vhold __P((struct vnode *));

/* Not supported yet */
#define CTLFLAG_MPSAFE              ENOTSUP
#define MBF_NOWAIT                  ENOTSUP
#define MNTK_NOMSYNC                ENOTSUP
#define VV_PROCD                    ENOTSUP
#define LK_CANRECURSE               ENOTSUP
#define LK_RETRY                    ENOTSUP
#define LK_EXCLUSIVE                ENOTSUP
#define LK_RETRY                    ENOTSUP
#define LK_INTERLOCK                ENOTSUP
#define LK_SHARED                   ENOTSUP
#define MTX_DUPOK                   ENOTSUP
#define VV_PROCDEP                  ENOTSUP
#define VN_LOCK_AREC(vp)            //lockallowrecurse((vp)->v_vnlock)
#define VREF(pvn)

#endif /* _XNU_COMPAT_H */
