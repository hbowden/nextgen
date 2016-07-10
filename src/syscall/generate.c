/**
 * Copyright (c) 2015, Harrison Bowden, Minneapolis, MN
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

#include "generate.h"
#include "crypto/crypto.h"
#include "io/io.h"
#include "memory/memory.h"
#include "network/network.h"
#include "resource/resource.h"
#include "runtime/nextgen.h"
#include "utils/utils.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef LINUX

#include <sys/vfs.h>
#include <sys/sysmacros.h>

#endif

int32_t generate_fd(uint64_t **fd, struct child_ctx *child)
{
    /* Allocate the descriptor. */
    (*fd) = mem_alloc(sizeof(int32_t));
    if((*fd) == NULL)
    {
        output(ERROR, "Can't allocate a descriptor\n");
        return (-1);
    }

    /* Get a file descriptor from the descriptor pool. */
    int32_t desc = get_desc();
    if(desc < 0)
    {
        output(ERROR, "Can't get file descriptor\n");
        return (-1);
    }

    memmove((*fd), &desc, sizeof(int32_t));

    /* Set the argument size. */
    set_arg_size(child, sizeof(int32_t));

    return (0);
}

int32_t generate_socket(uint64_t **sock, struct child_ctx *child)
{
    (*sock) = mem_alloc(sizeof(int32_t));
    if((*sock) == NULL)
    {
        output(ERROR, "Can't allocate socket\n");
        return (-1);
    }

    int32_t sock_fd = 0;

    sock_fd = get_socket();
    if(sock_fd < 0)
    {
        output(ERROR, "Can't get socket\n");
        return (-1);
    }

    memmove((*sock), &sock_fd, sizeof(int32_t));

    set_arg_size(child, sizeof(int32_t));

    return (0);
}

int32_t generate_buf(uint64_t **buf, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;
    uint32_t nbytes = 0;

    rtrn = rand_range(1023, &nbytes);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random size\n");
        return (-1);
    }

    /* Add one to nbytes so that we don't have a zero.
    Which would cause mmap to fail on some platforms. */
    nbytes = nbytes + 1;

    rtrn = rand_range(2, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random number\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            (*buf) = mmap(NULL, nbytes, PROT_READ | PROT_WRITE,
                          MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output(ERROR, "mmap: %s\n", strerror(errno));
                return (-1);
            }
            break;

        case 1:
            (*buf) =
                mmap(NULL, nbytes, PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output(ERROR, "mmap: %s\n", strerror(errno));
                return (-1);
            }
            break;

        case 2:
            (*buf) =
                mmap(NULL, nbytes, PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
            if(*buf == MAP_FAILED)
            {
                output(ERROR, "mmap: %s\n", strerror(errno));
                return (-1);
            }
            break;

        default:
            output(ERROR, "Should not get here\n");
            return (-1);
    }

    set_arg_size(child, nbytes);

    return (0);
}

int32_t generate_length(uint64_t **length, struct child_ctx *child)
{
    uint32_t last_arg = 0;

    if(get_current_arg(child) == 0)
        last_arg = 0;
    else
        last_arg = get_current_arg(child) - 1;

    /* Allocate the length buffer */
    (*length) = mem_alloc(sizeof(uint64_t));
    if((*length) == NULL)
    {
        output(ERROR, "Can't alloc length\n");
        return (-1);
    }

    uint64_t arg_size = 0;

    int32_t rtrn = get_arg_size(child, last_arg, &arg_size);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get argument size.\n");
        return (-1);
    }

    /* Set the length to the length of the last syscall argument. */
    memcpy((*length), &arg_size, sizeof(uint64_t));

    /* Set this argument's length. */
    set_arg_size(child, sizeof(uint64_t));

    return (0);
}

int32_t generate_path(uint64_t **path, struct child_ctx *child)
{
    (*path) = (uint64_t *)get_filepath();
    if((*path) == NULL)
    {
        output(ERROR, "Can't get file path\n");
        return (-1);
    }

    set_arg_size(child, strlen((char *)(*path)));

    return (0);
}

int32_t generate_open_flag(uint64_t **flag, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    rtrn = rand_range(7, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    (*flag) = mem_alloc(12);
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc flag\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            (**flag) |= (uint64_t)O_RDONLY;
            break;

        case 1:
            (**flag) |= (uint64_t)O_WRONLY;
            break;

        case 2:
            (**flag) |= (uint64_t)O_RDWR;
            break;

        case 3:
            (**flag) |= (uint64_t)O_NONBLOCK;
            break;

        case 4:
            (**flag) |= (uint64_t)O_APPEND;
            break;

        case 5:
            (**flag) |= (uint64_t)O_CREAT;
            break;

        case 6:
            (**flag) |= (uint64_t)O_TRUNC;
            break;

        case 7:
            (**flag) |= (uint64_t)O_EXCL;
            break;

        default:
            output(ERROR, "Should not get here\n");
            return (-1);
    }

    set_arg_size(child, sizeof(int64_t));

    return (0);
}

int32_t generate_mode(uint64_t **mode, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    rtrn = rand_range(20, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    *mode = mem_alloc(12);
    if(*mode == NULL)
    {
        output(ERROR, "Can't alloc mode\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            **mode = 001;
            break;

        case 1:
            **mode = 002;
            break;

        case 2:
            **mode = 003;
            break;

        case 3:
            **mode = 004;
            break;

        case 4:
            **mode = 005;
            break;

        case 5:
            **mode = 006;
            break;

        case 6:
            **mode = 007;
            break;

        case 7:
            **mode = 010;
            break;

        case 8:
            **mode = 020;
            break;

        case 9:
            **mode = 030;
            break;

        case 10:
            **mode = 040;
            break;

        case 11:
            **mode = 050;
            break;

        case 12:
            **mode = 060;
            break;

        case 13:
            **mode = 070;
            break;

        case 14:
            **mode = 100;
            break;

        case 15:
            **mode = 200;
            break;

        case 16:
            **mode = 300;
            break;

        case 17:
            **mode = 400;
            break;

        case 18:
            **mode = 500;
            break;

        case 19:
            **mode = 600;
            break;

        case 20:
            **mode = 700;
            break;

        default:
            output(ERROR, "Can't get mode\n");
            return (-1);
    }

    set_arg_size(child, sizeof(int64_t));

    return (0);
}

int32_t generate_fs_stat(uint64_t **stat, struct child_ctx *child)
{
    struct statfs *stat_buf = mem_alloc(sizeof(struct statfs));
    if(stat_buf == NULL)
    {
        output(ERROR, "Can't alloc stat\n");
        return (-1);
    }

    (*stat) = (uint64_t *)stat_buf;

    set_arg_size(child, sizeof(struct statfs));

    return (0);
}

/* Don't compile on Linux. Move to generate-linux.c shim in the future. */
#ifndef LINUX

int32_t generate_fs_stat_flag(uint64_t **flag, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    *flag = mem_alloc(12);
    if(*flag == NULL)
    {
        output(ERROR, "Can't alloc flag\n");
        return (-1);
    }

    rtrn = rand_range(1, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            **flag = MNT_NOWAIT;
            break;

        case 1:
            **flag = MNT_WAIT;
            break;

        default:
            output(ERROR, "Can't pick stat flag\n");
            return (-1);
    }

    set_arg_size(child, sizeof(int64_t));

    return (0);
}

#endif

int32_t generate_pid(uint64_t **pid, struct child_ctx *child)
{
    (*pid) = mem_alloc(sizeof(unsigned long));
    if((*pid) == NULL)
    {
        output(STD, "Can't alloc pid\n");
        return (-1);
    }

    (**pid) = (uint64_t)getpid();

    set_arg_size(child, sizeof(int64_t));

    return (0);
}

int32_t generate_int(uint64_t **integer, struct child_ctx *child)
{
    uint32_t number;
    int32_t rtrn = 0;

    *integer = mem_alloc(sizeof(uint64_t));
    if(*integer == NULL)
    {
        output(ERROR, "Can't alloc int\n");
        return (-1);
    }

    rtrn = rand_range(INT_MAX, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random int\n");
        return (-1);
    }

    (**integer) = (uint64_t)number;

    set_arg_size(child, sizeof(int64_t));

    return (0);
}

int32_t generate_rusage(uint64_t **usage, struct child_ctx *child)
{
    struct rusage *buf = NULL;

    buf = mem_alloc(sizeof(struct rusage));
    if(buf == NULL)
    {
        output(ERROR, "Can't alloc rusage\n");
        return (-1);
    }

    (*usage) = (uint64_t *)buf;

    set_arg_size(child, sizeof(struct rusage));

    return (0);
}

int32_t generate_wait_option(uint64_t **option, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    (*option) = mem_alloc(sizeof(unsigned long));
    if((*option) == NULL)
    {
        output(ERROR, "Can't alloc option\n");
        return (-1);
    }

    rtrn = rand_range(1, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            **option = WUNTRACED;
            break;

        case 1:
            **option = WNOHANG;
            break;

        default:
            output(ERROR, "Should not get here\n");
            return (-1);
    }

    set_arg_size(child, sizeof(int64_t));

    return (0);
}

int32_t generate_whence(uint64_t **whence, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    (*whence) = mem_alloc(sizeof(uint64_t));
    if((*whence) == NULL)
    {
        output(ERROR, "mem_alloc\n");
        return (-1);
    }

#ifdef FREEBSD

    rtrn = rand_range(4, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

#else

    rtrn = rand_range(2, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

#endif

    switch(number)
    {
        case 0:
            (**whence) = SEEK_SET;
            break;

        case 1:
            **whence = SEEK_CUR;
            break;

        case 2:
            **whence = SEEK_END;
            break;

#ifdef FREEBSD

        case 3:
            **whence = SEEK_HOLE;
            break;

        case 4:
            **whence = SEEK_DATA;
            break;

#else
/* Nothing */
#endif

        default:
            output(ERROR, "Should not get here\n");
            return (-1);
    }

    set_arg_size(child, sizeof(int64_t));

    return (0);
}

int32_t generate_offset(uint64_t **offset, struct child_ctx *child)
{
    int32_t rtrn = 0;

    (*offset) = mem_alloc(sizeof(uint64_t));
    if((*offset) == NULL)
    {
        output(ERROR, "mem_alloc\n");
        return (-1);
    }

    rtrn = rand_range(INT_MAX, (uint32_t *)(*offset));
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    set_arg_size(child, sizeof(uint64_t));

    return (0);
}

int32_t generate_mountpath(uint64_t **path, struct child_ctx *child)
{
    (void)path;
    (void)child;
    return (0);
}

int32_t generate_mount_type(uint64_t **type, struct child_ctx *child)
{
    (void)type;
    (void)child;

    return (0);
}

int32_t generate_dirpath(uint64_t **dirpath, struct child_ctx *child)
{
    (*dirpath) = (uint64_t *)get_dirpath();
    if((*dirpath) == NULL)
    {
        output(ERROR, "Can't get directory path\n");
        return (-1);
    }

    set_arg_size(child, strlen((char *)(*dirpath)));

    return (0);
}

int32_t generate_mount_flags(uint64_t **flag, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

#ifdef MAC_OSX

    int32_t flags[] = {MNT_RDONLY, MNT_NOEXEC,      MNT_NOSUID,  MNT_NODEV,
                       MNT_UNION,  MNT_SYNCHRONOUS, MNT_CPROTECT};

    uint32_t range = 7;

#endif
#ifdef FREEBSD

    int32_t flags[] = {MNT_RDONLY, MNT_NOEXEC, MNT_NOSUID, MNT_UNION,
                       MNT_SYNCHRONOUS};

    uint32_t range = 5;

#endif
#ifdef LINUX

    int32_t flags[] = {MS_BIND, MS_DIRSYNC, MS_MANDLOCK,
                       MS_MOVE, MS_NOATIME, MS_NODEV,
                       MS_NODIRATIME, MS_NOEXEC, MS_NOSUID,
                       MS_RDONLY, MS_RELATIME, MS_REMOUNT, 
                       MS_REMOUNT, MS_SILENT};

    uint32_t range = 14;

#endif
    (*flag) = mem_alloc(sizeof(int32_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc mount flags\n");
        return (-1);
    }

    rtrn = rand_range(range, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    /* Copy randomly chosen flag value to flag buffer. */
    memcpy((*flag), &flags[number], sizeof(int32_t));

    /* Set argument size. */
    set_arg_size(child, sizeof(int32_t));

    return (0);
}

int32_t generate_unmount_flags(uint64_t **flag, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

#ifdef MAC_OSX

    int32_t flags[] = {MNT_RDONLY, MNT_NOEXEC,      MNT_NOSUID,  MNT_NODEV,
                       MNT_UNION,  MNT_SYNCHRONOUS, MNT_CPROTECT};

    uint32_t range = 7;

#endif
#ifdef FREEBSD

    int32_t flags[] = {MNT_RDONLY, MNT_NOEXEC, MNT_NOSUID, MNT_UNION,
                       MNT_SYNCHRONOUS};

    uint32_t range = 5;

#endif
#ifdef LINUX

    int32_t flags[] = {MS_BIND, MS_DIRSYNC, MS_MANDLOCK,
                       MS_MOVE, MS_NOATIME, MS_NODEV,
                       MS_NODIRATIME, MS_NOEXEC, MS_NOSUID,
                       MS_RDONLY, MS_RELATIME, MS_REMOUNT, 
                       MS_REMOUNT, MS_SILENT};

    uint32_t range = 14;

#endif

    (*flag) = mem_alloc(sizeof(uint64_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc mount flags\n");
        return (-1);
    }

    rtrn = rand_range(range, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    /* Copy randomly chosen flag value to flag buffer. */
    memcpy((*flag), &flags[number], sizeof(flags[number]));

    /* Set arg size. */
    set_arg_size(child, sizeof(int32_t));

    return (0);
}

int32_t generate_request(uint64_t **flag, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

#ifdef MAC_OSX

    int32_t request[] = {PT_TRACE_ME, PT_DENY_ATTACH, PT_CONTINUE,  PT_STEP,
                         PT_KILL,     PT_ATTACH,      PT_ATTACHEXC, PT_DETACH};

    uint32_t range = 7;

#endif
#ifdef FREEBSD

    int32_t request[] = {
        PT_TRACE_ME,   PT_READ_I,      PT_WRITE_I,      PT_IO,
        PT_CONTINUE,   PT_STEP,        PT_KILL,         PT_ATTACH,
        PT_DETACH,     PT_GETREGS,     PT_SETREGS,      PT_GETFPREGS,
        PT_SETFPREGS,  PT_GETDBREGS,   PT_SETDBREGS,    PT_LWPINFO,
        PT_GETNUMLWPS, PT_GETLWPLIST,  PT_SETSTEP,      PT_CLEARSTEP,
        PT_SUSPEND,    PT_RESUME,      PT_TO_SCE,       PT_TO_SCX,
        PT_SYSCALL,    PT_FOLLOW_FORK, PT_VM_TIMESTAMP, PT_VM_ENTRY
    };

    uint32_t range = 28;

#endif
#ifdef LINUX

    int32_t request[] = {
        PTRACE_TRACEME, PTRACE_PEEKTEXT,
        PTRACE_PEEKDATA, PTRACE_PEEKUSER,
        PTRACE_POKETEXT, PTRACE_POKEDATA,
        PTRACE_POKEUSER, PTRACE_GETREGS,
        PTRACE_GETFPREGS, PTRACE_GETREGSET,
        PTRACE_SETREGS, PTRACE_SETFPREGS,
        PTRACE_SETREGSET, PTRACE_GETSIGINFO,
        PTRACE_PEEKSIGINFO, PTRACE_SETOPTIONS,
    };

    int32_t range = 16;
#endif

    (*flag) = mem_alloc(sizeof(int32_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc mount flags\n");
        return (-1);
    }

    rtrn = rand_range(range, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    memcpy((*flag), &request[number], sizeof(int32_t));

    set_arg_size(child, sizeof(int32_t));

    return (0);
}

int32_t generate_recv_flags(uint64_t **flag, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    (*flag) = mem_alloc(sizeof(uint32_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't alloc mount flags\n");
        return (-1);
    }

    rtrn = rand_range(2, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return (-1);
    }

    switch(number)
    {
        case 0:
            (**flag) = MSG_OOB;
            break;

        case 1:
            (**flag) = MSG_PEEK;
            break;

        case 2:
            (**flag) = MSG_WAITALL;
            break;

        default:
            output(ERROR, "Should not get here\n");
            return (-1);
    }

    set_arg_size(child, sizeof(uint32_t));

    return (0);
}

int32_t generate_dev(uint64_t **dev, struct child_ctx *child)
{
    (*dev) = mem_alloc(sizeof(dev_t));
    if((*dev) == NULL)
    {
        output(ERROR, "Can't alloc dev\n");
        return (-1);
    }

#ifdef LINUX

    dev_t device = makedev(10, 2);

#else

    dev_t device = 0;

#endif

    memmove((*dev), &device, sizeof(dev_t));

    set_arg_size(child, sizeof(dev_t));

    return (0);
}

int32_t generate_message(uint64_t **msg, struct child_ctx *child)
{
    (*msg) = mem_alloc(sizeof(struct msghdr));
    if((*msg) == NULL)
    {
        output(ERROR, "Can't alloc message header\n");
        return (-1);
    }

    struct msghdr message;
    struct iovec iov;
    char *data = NULL;
    int32_t rtrn = 0;

    data = mem_alloc(64);
    if(data == NULL)
    {
        output(ERROR, "Can't allocate data\n");
        return (-1);
    }

    rtrn = rand_bytes(&data, 63);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get random data\n");
        return (-1);
    }

    memset(&message, 0, sizeof(struct msghdr));
    memset(&iov, 0, sizeof(struct iovec));

    message.msg_name = NULL;
    message.msg_namelen = 0;
    iov.iov_base = data;

    iov.iov_len = strlen(data) + 1;
    message.msg_iov = &iov;
    message.msg_iovlen = 1;
    message.msg_control = NULL;
    message.msg_controllen = 0;
    message.msg_flags = 0;

    memmove((*msg), &message, sizeof(struct msghdr));

    set_arg_size(child, sizeof(struct msghdr));

    return (0);
}

int32_t generate_send_flags(uint64_t **flag, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;
    int32_t send_flags[] = {MSG_OOB, MSG_DONTROUTE};

    (*flag) = mem_alloc(sizeof(uint64_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't allocate send flags\n");
        return (-1);
    }

    rtrn = rand_range(2, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random number\n");
        return (-1);
    }

    switch((int32_t)number)
    {
        case 0:
        case 1:
            (**flag) |= (uint64_t) send_flags[number];
            break;

        case 2:
            (**flag) = (uint64_t) send_flags[0] | (uint64_t) send_flags[1];
            break;

        default:
            output(ERROR, "Random number is too high\n");
            return (-1);
    }

    set_arg_size(child, sizeof(uint64_t));

    return (0);
}

int32_t generate_sockaddr(uint64_t **addr, struct child_ctx *child)
{
    int32_t rtrn = 0;
    struct sockaddr_in in;
    uint32_t port = 0;

    (*addr) = mem_alloc(sizeof(struct sockaddr_in));
    if((*addr) == NULL)
    {
        output(ERROR, "Can't allocate sockaddr\n");
        return (-1);
    }

    rtrn = pick_random_port(&port);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random port\n");
        return (-1);
    }

    in.sin_family = AF_INET;
    in.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &in.sin_addr);

    memmove((*addr), &port, sizeof(struct sockaddr_in));

    set_arg_size(child, sizeof(struct sockaddr_in));

    return (0);
}

int32_t generate_socklen(uint64_t **len, struct child_ctx *child)
{
    uint32_t last_arg = 0;

    if(get_current_arg(child) == 0)
        last_arg = 0;
    else
        last_arg = get_current_arg(child) - 1;

    /* Allocate the length buffer */
    (*len) = mem_alloc(sizeof(socklen_t));
    if((*len) == NULL)
    {
        output(ERROR, "Can't alloc length\n");
        return (-1);
    }

    uint64_t arg_size = 0;

    int32_t rtrn = get_arg_size(child, last_arg, &arg_size);
    if(rtrn < 0)
    {
        output(ERROR, "Can't get argument size.\n");
        return (-1);
    }

    /* Set the socklen to the length of the last syscall argument. */
    memcpy((*len), &arg_size, sizeof(socklen_t));

    /* Set this argument's length. */
    set_arg_size(child, sizeof(socklen_t));

    return (0);
}

int32_t generate_amode(uint64_t **amode, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    (*amode) = mem_alloc(sizeof(uint32_t));
    if((*amode) == NULL)
    {
        output(ERROR, "Can't allocate amode\n");
        return (-1);
    }

    rtrn = rand_range(6, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random number\n");
        return (-1);
    }

    switch((int32_t)number)
    {
        case 0:
            (**amode) |= R_OK;
            break;

        case 1:
            (**amode) |= W_OK;
            break;

        case 2:
            (**amode) |= X_OK;
            break;

        case 3:
            (**amode) = R_OK | W_OK;
            break;

        case 4:
            (**amode) = R_OK | X_OK;
            break;

        case 5:
            (**amode) = W_OK | X_OK;
            break;

        case 6:
            (**amode) = W_OK | R_OK;
            break;

        default:
            output(ERROR, "Random number is too high\n");
            return (-1);
    }

    set_arg_size(child, sizeof(uint32_t));

    return (0);
}

#ifndef LINUX

int32_t generate_chflags(uint64_t **flag, struct child_ctx *child)
{
    int32_t rtrn = 0;
    uint32_t number = 0;

    (*flag) = mem_alloc(sizeof(uint32_t));
    if((*flag) == NULL)
    {
        output(ERROR, "Can't allocate flag\n");
        return (-1);
    }

    rtrn = rand_range(7, &number);
    if(rtrn < 0)
    {
        output(ERROR, "Can't pick random number\n");
        return (-1);
    }

    switch((int32_t)number)
    {
        case 0:
            (**flag) |= UF_NODUMP;
            break;

        case 1:
            (**flag) |= UF_IMMUTABLE;
            break;

        case 2:
            (**flag) |= UF_APPEND;
            break;

        case 3:
            (**flag) |= UF_OPAQUE;
            break;

        case 4:
            (**flag) |= UF_HIDDEN;
            break;

        case 5:
            (**flag) |= SF_ARCHIVED;
            break;

        case 6:
            (**flag) |= SF_IMMUTABLE;
            break;

        case 7:
            (**flag) |= SF_APPEND;
            break;

        default:
            output(ERROR, "Random number is too high\n");
            return (-1);
    }

    set_arg_size(child, sizeof(uint32_t));

    return (0);
}

#endif
