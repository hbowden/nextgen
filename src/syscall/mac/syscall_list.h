
/**
 * Copyright (c) 2017, Harrison Bowden, Minneapolis, MN
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice
 * and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **/



 /* WARNING, THIS FILE WAS MACHINE GENERATED. DO NOT MODIFY THIS FILE DIRECTLY
   BECAUSE YOUR CHANGES WILL BE OVERWRITTEN WHEN THIS FILE IS GENERATED AGAIN.
   IF YOU WAN'T TO MODIFY THIS FILE YOU SHOULD BE MODIFYING THE GENERATOR IT'S SELF
   AND REGENERATE THE SYSCALL ENTRY FILES. */


#ifndef SYSCALL_LIST_H
#define SYSCALL_LIST_H

#include "arg_types.h"
#include "generate.h"
#include "entry.h"

#include <sys/syscall.h>
#include <unistd.h>


extern struct syscall_entry entry_exit;

extern struct syscall_entry entry_fork;

extern struct syscall_entry entry_read;

extern struct syscall_entry entry_write;

extern struct syscall_entry entry_open;

extern struct syscall_entry entry_close;

extern struct syscall_entry entry_wait4;

extern struct syscall_entry entry_link;

extern struct syscall_entry entry_unlink;

extern struct syscall_entry entry_chdir;

extern struct syscall_entry entry_fchdir;

extern struct syscall_entry entry_mknod;

extern struct syscall_entry entry_chmod;

extern struct syscall_entry entry_chown;

extern struct syscall_entry entry_getfsstat;

extern struct syscall_entry entry_getpid;

extern struct syscall_entry entry_setuid;

extern struct syscall_entry entry_getuid;

extern struct syscall_entry entry_geteuid;

extern struct syscall_entry entry_ptrace;

extern struct syscall_entry entry_recvmsg;

extern struct syscall_entry entry_sendmsg;

extern struct syscall_entry entry_recvfrom;

extern struct syscall_entry entry_accept;

extern struct syscall_entry entry_getpeername;

extern struct syscall_entry entry_getsockname;

extern struct syscall_entry entry_access;

extern struct syscall_entry entry_chflags;

extern struct syscall_entry entry_fchflags;

extern struct syscall_entry entry_sync;

extern struct syscall_entry entry_kill;

extern struct syscall_entry entry_getppid;

extern struct syscall_entry entry_dup;

extern struct syscall_entry entry_pipe;

extern struct syscall_entry entry_getegid;

extern struct syscall_entry entry_sigaction;

extern struct syscall_entry entry_getgid;

extern struct syscall_entry entry_sigprocmask;

extern struct syscall_entry entry_getlogin;

extern struct syscall_entry entry_setlogin;

extern struct syscall_entry entry_acct;

extern struct syscall_entry entry_sigpending;

extern struct syscall_entry entry_sigaltstack;

extern struct syscall_entry entry_ioctl;

extern struct syscall_entry entry_reboot;

extern struct syscall_entry entry_revoke;

extern struct syscall_entry entry_symlink;

extern struct syscall_entry entry_readlink;

extern struct syscall_entry entry_execve;

extern struct syscall_entry entry_umask;

extern struct syscall_entry entry_chroot;

extern struct syscall_entry entry_msync;

extern struct syscall_entry entry_vfork;

extern struct syscall_entry entry_munmap;

extern struct syscall_entry entry_mprotect;

extern struct syscall_entry entry_madvise;

extern struct syscall_entry entry_mincore;

extern struct syscall_entry entry_getgroups;

extern struct syscall_entry entry_setgroups;

extern struct syscall_entry entry_getpgrp;

extern struct syscall_entry entry_setpgid;

extern struct syscall_entry entry_setitimer;

extern struct syscall_entry entry_swapon;

extern struct syscall_entry entry_getitimer;

extern struct syscall_entry entry_getdtablesize;

extern struct syscall_entry entry_dup2;

extern struct syscall_entry entry_fcntl;

extern struct syscall_entry entry_select;

extern struct syscall_entry entry_fsync;

extern struct syscall_entry entry_setpriority;

extern struct syscall_entry entry_socket;

extern struct syscall_entry entry_connect;

extern struct syscall_entry entry_getpriority;

extern struct syscall_entry entry_bind;

extern struct syscall_entry entry_setsockopt;

extern struct syscall_entry entry_listen;

extern struct syscall_entry entry_sigsuspend;

extern struct syscall_entry entry_gettimeofday;

extern struct syscall_entry entry_getrusage;

extern struct syscall_entry entry_getsockopt;

extern struct syscall_entry entry_readv;

extern struct syscall_entry entry_writev;

extern struct syscall_entry entry_settimeofday;

extern struct syscall_entry entry_fchown;

extern struct syscall_entry entry_fchmod;

extern struct syscall_entry entry_setreuid;

extern struct syscall_entry entry_setregid;

extern struct syscall_entry entry_rename;

extern struct syscall_entry entry_flock;

extern struct syscall_entry entry_mkfifo;

extern struct syscall_entry entry_sendto;

extern struct syscall_entry entry_shutdown;

extern struct syscall_entry entry_socketpair;

extern struct syscall_entry entry_mkdir;

extern struct syscall_entry entry_rmdir;

extern struct syscall_entry entry_utimes;

extern struct syscall_entry entry_futimes;

extern struct syscall_entry entry_adjtime;

extern struct syscall_entry entry_gethostuuid;

extern struct syscall_entry entry_setsid;

extern struct syscall_entry entry_getpgid;

extern struct syscall_entry entry_setprivexec;

extern struct syscall_entry entry_pread;

extern struct syscall_entry entry_pwrite;

extern struct syscall_entry entry_nfssvc;

extern struct syscall_entry entry_statfs;

extern struct syscall_entry entry_fstatfs;

extern struct syscall_entry entry_unmount;

extern struct syscall_entry entry_getfh;

extern struct syscall_entry entry_quotactl;

extern struct syscall_entry entry_mount;

extern struct syscall_entry entry_csops;

extern struct syscall_entry entry_csops_audittoken;

extern struct syscall_entry entry_waitid;

extern struct syscall_entry entry_kdebug_typefilter;

extern struct syscall_entry entry_kdebug_trace_string;

extern struct syscall_entry entry_kdebug_trace64;

extern struct syscall_entry entry_kdebug_trace;

extern struct syscall_entry entry_setgid;

extern struct syscall_entry entry_setegid;

extern struct syscall_entry entry_seteuid;

extern struct syscall_entry entry_sigreturn;

extern struct syscall_entry entry_fdatasync;

extern struct syscall_entry entry_stat;

extern struct syscall_entry entry_fstat;

extern struct syscall_entry entry_lstat;

extern struct syscall_entry entry_pathconf;

extern struct syscall_entry entry_fpathconf;

extern struct syscall_entry entry_getrlimit;

extern struct syscall_entry entry_setrlimit;

extern struct syscall_entry entry_getdirentries;

extern struct syscall_entry entry_mmap;

extern struct syscall_entry entry_lseek;

extern struct syscall_entry entry_truncate;

extern struct syscall_entry entry_ftruncate;

extern struct syscall_entry entry_sysctl;

extern struct syscall_entry entry_mlock;

extern struct syscall_entry entry_munlock;

extern struct syscall_entry entry_undelete;

extern struct syscall_entry entry_open_dprotected_np;

extern struct syscall_entry entry_getattrlist;

extern struct syscall_entry entry_setattrlist;

extern struct syscall_entry entry_getdirentriesattr;

extern struct syscall_entry entry_exchangedata;

extern struct syscall_entry entry_searchfs;

extern struct syscall_entry entry_delete;

extern struct syscall_entry entry_copyfile;

extern struct syscall_entry entry_fgetattrlist;

extern struct syscall_entry entry_fsetattrlist;

extern struct syscall_entry entry_poll;

extern struct syscall_entry entry_watchevent;

extern struct syscall_entry entry_waitevent;

extern struct syscall_entry entry_modwatch;

extern struct syscall_entry entry_getxattr;

extern struct syscall_entry entry_fgetxattr;

extern struct syscall_entry entry_setxattr;

extern struct syscall_entry entry_fsetxattr;

extern struct syscall_entry entry_removexattr;

extern struct syscall_entry entry_fremovexattr;

extern struct syscall_entry entry_listxattr;

extern struct syscall_entry entry_flistxattr;

extern struct syscall_entry entry_fsctl;

extern struct syscall_entry entry_initgroups;

extern struct syscall_entry entry_posix_spawn;

extern struct syscall_entry entry_ffsctl;

extern struct syscall_entry entry_nfsclnt;

extern struct syscall_entry entry_fhopen;

extern struct syscall_entry entry_minherit;

extern struct syscall_entry entry_semsys;

extern struct syscall_entry entry_msgsys;

extern struct syscall_entry entry_shmsys;

extern struct syscall_entry entry_semctl;

extern struct syscall_entry entry_semget;

extern struct syscall_entry entry_semop;

extern struct syscall_entry entry_msgctl;

extern struct syscall_entry entry_msgget;

extern struct syscall_entry entry_msgsnd;

extern struct syscall_entry entry_msgrcv;

extern struct syscall_entry entry_shmat;

extern struct syscall_entry entry_shmctl;

extern struct syscall_entry entry_shmdt;

extern struct syscall_entry entry_shmget;

extern struct syscall_entry entry_shm_open;

extern struct syscall_entry entry_shm_unlink;

extern struct syscall_entry entry_sem_open;

extern struct syscall_entry entry_sem_close;

extern struct syscall_entry entry_sem_unlink;

extern struct syscall_entry entry_sem_wait;

extern struct syscall_entry entry_sem_trywait;

extern struct syscall_entry entry_sem_post;

extern struct syscall_entry entry_sysctlbyname;

extern struct syscall_entry entry_open_extended;

extern struct syscall_entry entry_umask_extended;

extern struct syscall_entry entry_stat_extended;

extern struct syscall_entry entry_lstat_extended;

extern struct syscall_entry entry_fstat_extended;

extern struct syscall_entry entry_chmod_extended;

extern struct syscall_entry entry_fchmod_extended;

extern struct syscall_entry entry_access_extended;

extern struct syscall_entry entry_settid;

extern struct syscall_entry entry_gettid;

extern struct syscall_entry entry_setsgroups;

extern struct syscall_entry entry_getsgroups;

extern struct syscall_entry entry_setwgroups;

extern struct syscall_entry entry_getwgroups;

extern struct syscall_entry entry_mkfifo_extended;

extern struct syscall_entry entry_mkdir_extended;

extern struct syscall_entry entry_identitysvc;

extern struct syscall_entry entry_shared_region_check_np;

extern struct syscall_entry entry_vm_pressure_monitor;

extern struct syscall_entry entry_psynch_rw_longrdlock;

extern struct syscall_entry entry_psynch_rw_yieldwrlock;

extern struct syscall_entry entry_psynch_rw_downgrade;

extern struct syscall_entry entry_psynch_rw_upgrade;

extern struct syscall_entry entry_psynch_mutexwait;

extern struct syscall_entry entry_psynch_mutexdrop;

extern struct syscall_entry entry_psynch_cvbroad;

extern struct syscall_entry entry_psynch_cvsignal;

extern struct syscall_entry entry_psynch_cvwait;

extern struct syscall_entry entry_psynch_rw_rdlock;

extern struct syscall_entry entry_psynch_rw_wrlock;

extern struct syscall_entry entry_psynch_rw_unlock;

extern struct syscall_entry entry_psynch_rw_unlock2;

extern struct syscall_entry entry_getsid;

extern struct syscall_entry entry_settid_with_pid;

extern struct syscall_entry entry_psynch_cvclrprepost;

extern struct syscall_entry entry_aio_fsync;

extern struct syscall_entry entry_aio_return;

extern struct syscall_entry entry_aio_suspend;

extern struct syscall_entry entry_aio_cancel;

extern struct syscall_entry entry_aio_error;

extern struct syscall_entry entry_aio_read;

extern struct syscall_entry entry_aio_write;

extern struct syscall_entry entry_lio_listio;

extern struct syscall_entry entry_iopolicysys;

extern struct syscall_entry entry_process_policy;

extern struct syscall_entry entry_mlockall;

extern struct syscall_entry entry_munlockall;

extern struct syscall_entry entry_issetugid;

extern struct syscall_entry entry___pthread_kill;

extern struct syscall_entry entry___pthread_sigmask;

extern struct syscall_entry entry___sigwait;

extern struct syscall_entry entry___disable_threadsignal;

extern struct syscall_entry entry___pthread_markcancel;

extern struct syscall_entry entry___pthread_canceled;

extern struct syscall_entry entry___semwait_signal;

extern struct syscall_entry entry_proc_info;

extern struct syscall_entry entry_sendfile;

extern struct syscall_entry entry_stat64;

extern struct syscall_entry entry_fstat64;

extern struct syscall_entry entry_lstat64;

extern struct syscall_entry entry_stat64_extended;

extern struct syscall_entry entry_lstat64_extended;

extern struct syscall_entry entry_fstat64_extended;

extern struct syscall_entry entry_getdirentries64;

extern struct syscall_entry entry_statfs64;

extern struct syscall_entry entry_fstatfs64;

extern struct syscall_entry entry_getfsstat64;

extern struct syscall_entry entry___pthread_chdir;

extern struct syscall_entry entry___pthread_fchdir;

extern struct syscall_entry entry_audit;

extern struct syscall_entry entry_auditon;

extern struct syscall_entry entry_getauid;

extern struct syscall_entry entry_setauid;

extern struct syscall_entry entry_getaudit_addr;

extern struct syscall_entry entry_setaudit_addr;

extern struct syscall_entry entry_auditctl;

extern struct syscall_entry entry_bsdthread_create;

extern struct syscall_entry entry_bsdthread_terminate;

extern struct syscall_entry entry_kqueue;

extern struct syscall_entry entry_kevent;

extern struct syscall_entry entry_lchown;

extern struct syscall_entry entry_bsdthread_register;

extern struct syscall_entry entry_workq_open;

extern struct syscall_entry entry_workq_kernreturn;

extern struct syscall_entry entry_kevent64;

extern struct syscall_entry entry___old_semwait_signal;

extern struct syscall_entry entry___old_semwait_signal_nocancel;

extern struct syscall_entry entry_thread_selfid ;

extern struct syscall_entry entry_ledger;

extern struct syscall_entry entry_kevent_qos;

extern struct syscall_entry entry___mac_execve;

extern struct syscall_entry entry___mac_syscall;

extern struct syscall_entry entry___mac_get_file;

extern struct syscall_entry entry___mac_set_file;

extern struct syscall_entry entry___mac_get_link;

extern struct syscall_entry entry___mac_set_link;

extern struct syscall_entry entry___mac_get_proc;

extern struct syscall_entry entry___mac_set_proc;

extern struct syscall_entry entry___mac_get_fd;

extern struct syscall_entry entry___mac_set_fd;

extern struct syscall_entry entry___mac_get_pid;

extern struct syscall_entry entry_pselect;

extern struct syscall_entry entry_pselect_nocancel;

extern struct syscall_entry entry_read_nocancel;

extern struct syscall_entry entry_write_nocancel;

extern struct syscall_entry entry_open_nocancel;

extern struct syscall_entry entry_close_nocancel;

extern struct syscall_entry entry_wait4_nocancel;

extern struct syscall_entry entry_recvmsg_nocancel;

extern struct syscall_entry entry_sendmsg_nocancel;

extern struct syscall_entry entry_recvfrom_nocancel;

extern struct syscall_entry entry_accept_nocancel;

extern struct syscall_entry entry_msync_nocancel;

extern struct syscall_entry entry_fcntl_nocancel;

extern struct syscall_entry entry_select_nocancel;

extern struct syscall_entry entry_fsync_nocancel;

extern struct syscall_entry entry_connect_nocancel;

extern struct syscall_entry entry_sigsuspend_nocancel;

extern struct syscall_entry entry_readv_nocancel;

extern struct syscall_entry entry_writev_nocancel;

extern struct syscall_entry entry_sendto_nocancel;

extern struct syscall_entry entry_pread_nocancel;

extern struct syscall_entry entry_pwrite_nocancel;

extern struct syscall_entry entry_waitid_nocancel;

extern struct syscall_entry entry_poll_nocancel;

extern struct syscall_entry entry_msgsnd_nocancel;

extern struct syscall_entry entry_msgrcv_nocancel;

extern struct syscall_entry entry_sem_wait_nocancel;

extern struct syscall_entry entry_aio_suspend_nocancel;

extern struct syscall_entry entry___sigwait_nocancel;

extern struct syscall_entry entry___semwait_signal_nocancel;

extern struct syscall_entry entry___mac_mount;

extern struct syscall_entry entry___mac_get_mount;

extern struct syscall_entry entry___mac_getfsstat;

extern struct syscall_entry entry_fsgetpath;

extern struct syscall_entry entry_audit_session_self;

extern struct syscall_entry entry_audit_session_join;

extern struct syscall_entry entry_fileport_makeport;

extern struct syscall_entry entry_fileport_makefd;

extern struct syscall_entry entry_audit_session_port;

extern struct syscall_entry entry_pid_suspend;

extern struct syscall_entry entry_pid_resume;

extern struct syscall_entry entry_shared_region_map_and_slide_np;

extern struct syscall_entry entry_kas_info;

extern struct syscall_entry entry_memorystatus_control;

extern struct syscall_entry entry_guarded_open_np;

extern struct syscall_entry entry_guarded_close_np;

extern struct syscall_entry entry_guarded_kqueue_np;

extern struct syscall_entry entry_change_fdguard_np;

extern struct syscall_entry entry_usrctl;

extern struct syscall_entry entry_proc_rlimit_control;

extern struct syscall_entry entry_connectx;

extern struct syscall_entry entry_disconnectx;

extern struct syscall_entry entry_peeloff;

extern struct syscall_entry entry_socket_delegate;

extern struct syscall_entry entry_telemetry;

extern struct syscall_entry entry_proc_uuid_policy;

extern struct syscall_entry entry_memorystatus_get_level;

extern struct syscall_entry entry_system_override;

extern struct syscall_entry entry_vfs_purge;

extern struct syscall_entry entry_sfi_ctl;

extern struct syscall_entry entry_sfi_pidctl;

extern struct syscall_entry entry_coalition;

extern struct syscall_entry entry_coalition_info;

extern struct syscall_entry entry_necp_match_policy;

extern struct syscall_entry entry_getattrlistbulk;

extern struct syscall_entry entry_clonefileat;

extern struct syscall_entry entry_openat;

extern struct syscall_entry entry_openat_nocancel;

extern struct syscall_entry entry_renameat;

extern struct syscall_entry entry_faccessat;

extern struct syscall_entry entry_fchmodat;

extern struct syscall_entry entry_fchownat;

extern struct syscall_entry entry_fstatat;

extern struct syscall_entry entry_fstatat64;

extern struct syscall_entry entry_linkat;

extern struct syscall_entry entry_unlinkat;

extern struct syscall_entry entry_readlinkat;

extern struct syscall_entry entry_symlinkat;

extern struct syscall_entry entry_mkdirat;

extern struct syscall_entry entry_getattrlistat;

extern struct syscall_entry entry_proc_trace_log;

extern struct syscall_entry entry_bsdthread_ctl;

extern struct syscall_entry entry_openbyid_np;

extern struct syscall_entry entry_recvmsg_x;

extern struct syscall_entry entry_sendmsg_x;

extern struct syscall_entry entry_thread_selfusage;

extern struct syscall_entry entry_csrctl;

extern struct syscall_entry entry_guarded_open_dprotected_np;

extern struct syscall_entry entry_guarded_write_np;

extern struct syscall_entry entry_guarded_pwrite_np;

extern struct syscall_entry entry_guarded_writev_np;

extern struct syscall_entry entry_renameatx_np;

extern struct syscall_entry entry_mremap_encrypted;

extern struct syscall_entry entry_netagent_trigger;

extern struct syscall_entry entry_stack_snapshot_with_config;

extern struct syscall_entry entry_microstackshot;

extern struct syscall_entry entry_grab_pgo_data ;

extern struct syscall_entry entry_persona;

extern struct syscall_entry entry_work_interval_ctl;

extern struct syscall_entry entry_getentropy;

extern struct syscall_entry entry_necp_open;

extern struct syscall_entry entry_necp_client_action;

extern struct syscall_entry entry_ulock_wait;

extern struct syscall_entry entry_ulock_wake;

extern struct syscall_entry entry_fclonefileat;

extern struct syscall_entry entry_fs_snapshot;

extern struct syscall_entry entry_terminate_with_payload;

extern struct syscall_entry entry_abort_with_payload;


#endif
