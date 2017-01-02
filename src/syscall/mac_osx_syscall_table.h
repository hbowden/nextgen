

/**
 * Copyright (c) 2016, Harrison Bowden, Minneapolis, MN
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



#ifndef MAC_OSX_SYSCALL_TABLE_H
#define MAC_OSX_SYSCALL_TABLE_H

#include "syscall_list.h"
#include "syscall_table.h"

struct syscall_table mac_osx_syscall_table = {
.total_syscalls = 400,

.sys_entry[0] = &entry_exit,

.sys_entry[1] = &entry_fork,

.sys_entry[2] = &entry_read,

.sys_entry[3] = &entry_write,

.sys_entry[4] = &entry_open,

.sys_entry[5] = &entry_close,

.sys_entry[6] = &entry_wait4,

.sys_entry[7] = &entry_link,

.sys_entry[8] = &entry_unlink,

.sys_entry[9] = &entry_chdir,

.sys_entry[10] = &entry_fchdir,

.sys_entry[11] = &entry_mknod,

.sys_entry[12] = &entry_chmod,

.sys_entry[13] = &entry_chown,

.sys_entry[14] = &entry_getfsstat,

.sys_entry[15] = &entry_getpid,

.sys_entry[16] = &entry_setuid,

.sys_entry[17] = &entry_getuid,

.sys_entry[18] = &entry_geteuid,

.sys_entry[19] = &entry_ptrace,

.sys_entry[20] = &entry_recvmsg,

.sys_entry[21] = &entry_sendmsg,

.sys_entry[22] = &entry_recvfrom,

.sys_entry[23] = &entry_accept,

.sys_entry[24] = &entry_getpeername,

.sys_entry[25] = &entry_getsockname,

.sys_entry[26] = &entry_access,

.sys_entry[27] = &entry_chflags,

.sys_entry[28] = &entry_fchflags,

.sys_entry[29] = &entry_sync,

.sys_entry[30] = &entry_kill,

.sys_entry[31] = &entry_getppid,

.sys_entry[32] = &entry_dup,

.sys_entry[33] = &entry_pipe,

.sys_entry[34] = &entry_getegid,

.sys_entry[35] = &entry_sigaction,

.sys_entry[36] = &entry_getgid,

.sys_entry[37] = &entry_sigprocmask,

.sys_entry[38] = &entry_getlogin,

.sys_entry[39] = &entry_setlogin,

.sys_entry[40] = &entry_acct,

.sys_entry[41] = &entry_sigpending,

.sys_entry[42] = &entry_sigaltstack,

.sys_entry[43] = &entry_ioctl,

.sys_entry[44] = &entry_reboot,

.sys_entry[45] = &entry_revoke,

.sys_entry[46] = &entry_symlink,

.sys_entry[47] = &entry_readlink,

.sys_entry[48] = &entry_execve,

.sys_entry[49] = &entry_umask,

.sys_entry[50] = &entry_chroot,

.sys_entry[51] = &entry_msync,

.sys_entry[52] = &entry_vfork,

.sys_entry[53] = &entry_munmap,

.sys_entry[54] = &entry_mprotect,

.sys_entry[55] = &entry_madvise,

.sys_entry[56] = &entry_mincore,

.sys_entry[57] = &entry_getgroups,

.sys_entry[58] = &entry_setgroups,

.sys_entry[59] = &entry_getpgrp,

.sys_entry[60] = &entry_setpgid,

.sys_entry[61] = &entry_setitimer,

.sys_entry[62] = &entry_swapon,

.sys_entry[63] = &entry_getitimer,

.sys_entry[64] = &entry_getdtablesize,

.sys_entry[65] = &entry_dup2,

.sys_entry[66] = &entry_fcntl,

.sys_entry[67] = &entry_select,

.sys_entry[68] = &entry_fsync,

.sys_entry[69] = &entry_setpriority,

.sys_entry[70] = &entry_socket,

.sys_entry[71] = &entry_connect,

.sys_entry[72] = &entry_getpriority,

.sys_entry[73] = &entry_bind,

.sys_entry[74] = &entry_setsockopt,

.sys_entry[75] = &entry_listen,

.sys_entry[76] = &entry_sigsuspend,

.sys_entry[77] = &entry_gettimeofday,

.sys_entry[78] = &entry_getrusage,

.sys_entry[79] = &entry_getsockopt,

.sys_entry[80] = &entry_readv,

.sys_entry[81] = &entry_writev,

.sys_entry[82] = &entry_settimeofday,

.sys_entry[83] = &entry_fchown,

.sys_entry[84] = &entry_fchmod,

.sys_entry[85] = &entry_setreuid,

.sys_entry[86] = &entry_setregid,

.sys_entry[87] = &entry_rename,

.sys_entry[88] = &entry_flock,

.sys_entry[89] = &entry_mkfifo,

.sys_entry[90] = &entry_sendto,

.sys_entry[91] = &entry_shutdown,

.sys_entry[92] = &entry_socketpair,

.sys_entry[93] = &entry_mkdir,

.sys_entry[94] = &entry_rmdir,

.sys_entry[95] = &entry_utimes,

.sys_entry[96] = &entry_futimes,

.sys_entry[97] = &entry_adjtime,

.sys_entry[98] = &entry_gethostuuid,

.sys_entry[99] = &entry_setsid,

.sys_entry[100] = &entry_getpgid,

.sys_entry[101] = &entry_setprivexec,

.sys_entry[102] = &entry_pread,

.sys_entry[103] = &entry_pwrite,

.sys_entry[104] = &entry_nfssvc,

.sys_entry[105] = &entry_statfs,

.sys_entry[106] = &entry_fstatfs,

.sys_entry[107] = &entry_unmount,

.sys_entry[108] = &entry_getfh,

.sys_entry[109] = &entry_quotactl,

.sys_entry[110] = &entry_mount,

.sys_entry[111] = &entry_csops,

.sys_entry[112] = &entry_csops_audittoken,

.sys_entry[113] = &entry_waitid,

.sys_entry[114] = &entry_kdebug_typefilter,

.sys_entry[115] = &entry_kdebug_trace_string,

.sys_entry[116] = &entry_kdebug_trace64,

.sys_entry[117] = &entry_kdebug_trace,

.sys_entry[118] = &entry_setgid,

.sys_entry[119] = &entry_setegid,

.sys_entry[120] = &entry_seteuid,

.sys_entry[121] = &entry_sigreturn,

.sys_entry[122] = &entry_fdatasync,

.sys_entry[123] = &entry_stat,

.sys_entry[124] = &entry_fstat,

.sys_entry[125] = &entry_lstat,

.sys_entry[126] = &entry_pathconf,

.sys_entry[127] = &entry_fpathconf,

.sys_entry[128] = &entry_getrlimit,

.sys_entry[129] = &entry_setrlimit,

.sys_entry[130] = &entry_getdirentries,

.sys_entry[131] = &entry_mmap,

.sys_entry[132] = &entry_lseek,

.sys_entry[133] = &entry_truncate,

.sys_entry[134] = &entry_ftruncate,

.sys_entry[135] = &entry_sysctl,

.sys_entry[136] = &entry_mlock,

.sys_entry[137] = &entry_munlock,

.sys_entry[138] = &entry_undelete,

.sys_entry[139] = &entry_open_dprotected_np,

.sys_entry[140] = &entry_getattrlist,

.sys_entry[141] = &entry_setattrlist,

.sys_entry[142] = &entry_getdirentriesattr,

.sys_entry[143] = &entry_exchangedata,

.sys_entry[144] = &entry_searchfs,

.sys_entry[145] = &entry_delete,

.sys_entry[146] = &entry_copyfile,

.sys_entry[147] = &entry_fgetattrlist,

.sys_entry[148] = &entry_fsetattrlist,

.sys_entry[149] = &entry_poll,

.sys_entry[150] = &entry_watchevent,

.sys_entry[151] = &entry_waitevent,

.sys_entry[152] = &entry_modwatch,

.sys_entry[153] = &entry_getxattr,

.sys_entry[154] = &entry_fgetxattr,

.sys_entry[155] = &entry_setxattr,

.sys_entry[156] = &entry_fsetxattr,

.sys_entry[157] = &entry_removexattr,

.sys_entry[158] = &entry_fremovexattr,

.sys_entry[159] = &entry_listxattr,

.sys_entry[160] = &entry_flistxattr,

.sys_entry[161] = &entry_fsctl,

.sys_entry[162] = &entry_initgroups,

.sys_entry[163] = &entry_posix_spawn,

.sys_entry[164] = &entry_ffsctl,

.sys_entry[165] = &entry_nfsclnt,

.sys_entry[166] = &entry_fhopen,

.sys_entry[167] = &entry_minherit,

.sys_entry[168] = &entry_semsys,

.sys_entry[169] = &entry_msgsys,

.sys_entry[170] = &entry_shmsys,

.sys_entry[171] = &entry_semctl,

.sys_entry[172] = &entry_semget,

.sys_entry[173] = &entry_semop,

.sys_entry[174] = &entry_msgctl,

.sys_entry[175] = &entry_msgget,

.sys_entry[176] = &entry_msgsnd,

.sys_entry[177] = &entry_msgrcv,

.sys_entry[178] = &entry_shmat,

.sys_entry[179] = &entry_shmctl,

.sys_entry[180] = &entry_shmdt,

.sys_entry[181] = &entry_shmget,

.sys_entry[182] = &entry_shm_open,

.sys_entry[183] = &entry_shm_unlink,

.sys_entry[184] = &entry_sem_open,

.sys_entry[185] = &entry_sem_close,

.sys_entry[186] = &entry_sem_unlink,

.sys_entry[187] = &entry_sem_wait,

.sys_entry[188] = &entry_sem_trywait,

.sys_entry[189] = &entry_sem_post,

.sys_entry[190] = &entry_sysctlbyname,

.sys_entry[191] = &entry_open_extended,

.sys_entry[192] = &entry_umask_extended,

.sys_entry[193] = &entry_stat_extended,

.sys_entry[194] = &entry_lstat_extended,

.sys_entry[195] = &entry_fstat_extended,

.sys_entry[196] = &entry_chmod_extended,

.sys_entry[197] = &entry_fchmod_extended,

.sys_entry[198] = &entry_access_extended,

.sys_entry[199] = &entry_settid,

.sys_entry[200] = &entry_gettid,

.sys_entry[201] = &entry_setsgroups,

.sys_entry[202] = &entry_getsgroups,

.sys_entry[203] = &entry_setwgroups,

.sys_entry[204] = &entry_getwgroups,

.sys_entry[205] = &entry_mkfifo_extended,

.sys_entry[206] = &entry_mkdir_extended,

.sys_entry[207] = &entry_identitysvc,

.sys_entry[208] = &entry_shared_region_check_np,

.sys_entry[209] = &entry_vm_pressure_monitor,

.sys_entry[210] = &entry_psynch_rw_longrdlock,

.sys_entry[211] = &entry_psynch_rw_yieldwrlock,

.sys_entry[212] = &entry_psynch_rw_downgrade,

.sys_entry[213] = &entry_psynch_rw_upgrade,

.sys_entry[214] = &entry_psynch_mutexwait,

.sys_entry[215] = &entry_psynch_mutexdrop,

.sys_entry[216] = &entry_psynch_cvbroad,

.sys_entry[217] = &entry_psynch_cvsignal,

.sys_entry[218] = &entry_psynch_cvwait,

.sys_entry[219] = &entry_psynch_rw_rdlock,

.sys_entry[220] = &entry_psynch_rw_wrlock,

.sys_entry[221] = &entry_psynch_rw_unlock,

.sys_entry[222] = &entry_psynch_rw_unlock2,

.sys_entry[223] = &entry_getsid,

.sys_entry[224] = &entry_settid_with_pid,

.sys_entry[225] = &entry_psynch_cvclrprepost,

.sys_entry[226] = &entry_aio_fsync,

.sys_entry[227] = &entry_aio_return,

.sys_entry[228] = &entry_aio_suspend,

.sys_entry[229] = &entry_aio_cancel,

.sys_entry[230] = &entry_aio_error,

.sys_entry[231] = &entry_aio_read,

.sys_entry[232] = &entry_aio_write,

.sys_entry[233] = &entry_lio_listio,

.sys_entry[234] = &entry_iopolicysys,

.sys_entry[235] = &entry_process_policy,

.sys_entry[236] = &entry_mlockall,

.sys_entry[237] = &entry_munlockall,

.sys_entry[238] = &entry_issetugid,

.sys_entry[239] = &entry___pthread_kill,

.sys_entry[240] = &entry___pthread_sigmask,

.sys_entry[241] = &entry___sigwait,

.sys_entry[242] = &entry___disable_threadsignal,

.sys_entry[243] = &entry___pthread_markcancel,

.sys_entry[244] = &entry___pthread_canceled,

.sys_entry[245] = &entry___semwait_signal,

.sys_entry[246] = &entry_proc_info,

.sys_entry[247] = &entry_sendfile,

.sys_entry[248] = &entry_stat64,

.sys_entry[249] = &entry_fstat64,

.sys_entry[250] = &entry_lstat64,

.sys_entry[251] = &entry_stat64_extended,

.sys_entry[252] = &entry_lstat64_extended,

.sys_entry[253] = &entry_fstat64_extended,

.sys_entry[254] = &entry_getdirentries64,

.sys_entry[255] = &entry_statfs64,

.sys_entry[256] = &entry_fstatfs64,

.sys_entry[257] = &entry_getfsstat64,

.sys_entry[258] = &entry___pthread_chdir,

.sys_entry[259] = &entry___pthread_fchdir,

.sys_entry[260] = &entry_audit,

.sys_entry[261] = &entry_auditon,

.sys_entry[262] = &entry_getauid,

.sys_entry[263] = &entry_setauid,

.sys_entry[264] = &entry_getaudit_addr,

.sys_entry[265] = &entry_setaudit_addr,

.sys_entry[266] = &entry_auditctl,

.sys_entry[267] = &entry_bsdthread_create,

.sys_entry[268] = &entry_bsdthread_terminate,

.sys_entry[269] = &entry_kqueue,

.sys_entry[270] = &entry_kevent,

.sys_entry[271] = &entry_lchown,

.sys_entry[272] = &entry_bsdthread_register,

.sys_entry[273] = &entry_workq_open,

.sys_entry[274] = &entry_workq_kernreturn,

.sys_entry[275] = &entry_kevent64,

.sys_entry[276] = &entry___old_semwait_signal,

.sys_entry[277] = &entry___old_semwait_signal_nocancel,

.sys_entry[278] = &entry_thread_selfid ,

.sys_entry[279] = &entry_ledger,

.sys_entry[280] = &entry_kevent_qos,

.sys_entry[281] = &entry___mac_execve,

.sys_entry[282] = &entry___mac_syscall,

.sys_entry[283] = &entry___mac_get_file,

.sys_entry[284] = &entry___mac_set_file,

.sys_entry[285] = &entry___mac_get_link,

.sys_entry[286] = &entry___mac_set_link,

.sys_entry[287] = &entry___mac_get_proc,

.sys_entry[288] = &entry___mac_set_proc,

.sys_entry[289] = &entry___mac_get_fd,

.sys_entry[290] = &entry___mac_set_fd,

.sys_entry[291] = &entry___mac_get_pid,

.sys_entry[292] = &entry_pselect,

.sys_entry[293] = &entry_pselect_nocancel,

.sys_entry[294] = &entry_read_nocancel,

.sys_entry[295] = &entry_write_nocancel,

.sys_entry[296] = &entry_open_nocancel,

.sys_entry[297] = &entry_close_nocancel,

.sys_entry[298] = &entry_wait4_nocancel,

.sys_entry[299] = &entry_recvmsg_nocancel,

.sys_entry[300] = &entry_sendmsg_nocancel,

.sys_entry[301] = &entry_recvfrom_nocancel,

.sys_entry[302] = &entry_accept_nocancel,

.sys_entry[303] = &entry_msync_nocancel,

.sys_entry[304] = &entry_fcntl_nocancel,

.sys_entry[305] = &entry_select_nocancel,

.sys_entry[306] = &entry_fsync_nocancel,

.sys_entry[307] = &entry_connect_nocancel,

.sys_entry[308] = &entry_sigsuspend_nocancel,

.sys_entry[309] = &entry_readv_nocancel,

.sys_entry[310] = &entry_writev_nocancel,

.sys_entry[311] = &entry_sendto_nocancel,

.sys_entry[312] = &entry_pread_nocancel,

.sys_entry[313] = &entry_pwrite_nocancel,

.sys_entry[314] = &entry_waitid_nocancel,

.sys_entry[315] = &entry_poll_nocancel,

.sys_entry[316] = &entry_msgsnd_nocancel,

.sys_entry[317] = &entry_msgrcv_nocancel,

.sys_entry[318] = &entry_sem_wait_nocancel,

.sys_entry[319] = &entry_aio_suspend_nocancel,

.sys_entry[320] = &entry___sigwait_nocancel,

.sys_entry[321] = &entry___semwait_signal_nocancel,

.sys_entry[322] = &entry___mac_mount,

.sys_entry[323] = &entry___mac_get_mount,

.sys_entry[324] = &entry___mac_getfsstat,

.sys_entry[325] = &entry_fsgetpath,

.sys_entry[326] = &entry_audit_session_self,

.sys_entry[327] = &entry_audit_session_join,

.sys_entry[328] = &entry_fileport_makeport,

.sys_entry[329] = &entry_fileport_makefd,

.sys_entry[330] = &entry_audit_session_port,

.sys_entry[331] = &entry_pid_suspend,

.sys_entry[332] = &entry_pid_resume,

.sys_entry[333] = &entry_shared_region_map_and_slide_np,

.sys_entry[334] = &entry_kas_info,

.sys_entry[335] = &entry_memorystatus_control,

.sys_entry[336] = &entry_guarded_open_np,

.sys_entry[337] = &entry_guarded_close_np,

.sys_entry[338] = &entry_guarded_kqueue_np,

.sys_entry[339] = &entry_change_fdguard_np,

.sys_entry[340] = &entry_usrctl,

.sys_entry[341] = &entry_proc_rlimit_control,

.sys_entry[342] = &entry_connectx,

.sys_entry[343] = &entry_disconnectx,

.sys_entry[344] = &entry_peeloff,

.sys_entry[345] = &entry_socket_delegate,

.sys_entry[346] = &entry_telemetry,

.sys_entry[347] = &entry_proc_uuid_policy,

.sys_entry[348] = &entry_memorystatus_get_level,

.sys_entry[349] = &entry_system_override,

.sys_entry[350] = &entry_vfs_purge,

.sys_entry[351] = &entry_sfi_ctl,

.sys_entry[352] = &entry_sfi_pidctl,

.sys_entry[353] = &entry_coalition,

.sys_entry[354] = &entry_coalition_info,

.sys_entry[355] = &entry_necp_match_policy,

.sys_entry[356] = &entry_getattrlistbulk,

.sys_entry[357] = &entry_clonefileat,

.sys_entry[358] = &entry_openat,

.sys_entry[359] = &entry_openat_nocancel,

.sys_entry[360] = &entry_renameat,

.sys_entry[361] = &entry_faccessat,

.sys_entry[362] = &entry_fchmodat,

.sys_entry[363] = &entry_fchownat,

.sys_entry[364] = &entry_fstatat,

.sys_entry[365] = &entry_fstatat64,

.sys_entry[366] = &entry_linkat,

.sys_entry[367] = &entry_unlinkat,

.sys_entry[368] = &entry_readlinkat,

.sys_entry[369] = &entry_symlinkat,

.sys_entry[370] = &entry_mkdirat,

.sys_entry[371] = &entry_getattrlistat,

.sys_entry[372] = &entry_proc_trace_log,

.sys_entry[373] = &entry_bsdthread_ctl,

.sys_entry[374] = &entry_openbyid_np,

.sys_entry[375] = &entry_recvmsg_x,

.sys_entry[376] = &entry_sendmsg_x,

.sys_entry[377] = &entry_thread_selfusage,

.sys_entry[378] = &entry_csrctl,

.sys_entry[379] = &entry_guarded_open_dprotected_np,

.sys_entry[380] = &entry_guarded_write_np,

.sys_entry[381] = &entry_guarded_pwrite_np,

.sys_entry[382] = &entry_guarded_writev_np,

.sys_entry[383] = &entry_renameatx_np,

.sys_entry[384] = &entry_mremap_encrypted,

.sys_entry[385] = &entry_netagent_trigger,

.sys_entry[386] = &entry_stack_snapshot_with_config,

.sys_entry[387] = &entry_microstackshot,

.sys_entry[388] = &entry_grab_pgo_data ,

.sys_entry[389] = &entry_persona,

.sys_entry[390] = &entry_work_interval_ctl,

.sys_entry[391] = &entry_getentropy,

.sys_entry[392] = &entry_necp_open,

.sys_entry[393] = &entry_necp_client_action,

.sys_entry[394] = &entry_ulock_wait,

.sys_entry[395] = &entry_ulock_wake,

.sys_entry[396] = &entry_fclonefileat,

.sys_entry[397] = &entry_fs_snapshot,

.sys_entry[398] = &entry_terminate_with_payload,

.sys_entry[399] = &entry_abort_with_payload,

};

#endif
