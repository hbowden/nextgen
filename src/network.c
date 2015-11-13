

/**
 * Copyright (c) 2015, Harrison Bowden, Secure Labs, Minneapolis, MN
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

#include "network.h"
#include "nextgen.h"
#include "concurrent.h"
#include "crypto.h"
#include "types.h"
#include "io.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>

int connect_ipv4(int *sockFd)
{
    int rtrn;
    
    union
    {
        struct sockaddr_in in;
        struct sockaddr_in6 in6;
        
    }addr;
    
    addr.in.sin_family = AF_INET;
    addr.in.sin_port = htons(map->socket_server_port);
    inet_pton(AF_INET, "127.0.0.1", &addr.in.sin_addr);
    
    *sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if(*sockFd < 0)
    {
        output(ERROR, "socket: %s\n", strerror(errno));
        return -1;
    }
    
    rtrn = connect(*sockFd, (struct sockaddr *)&addr.in, sizeof(addr.in));
    if(rtrn < 0)
    {
        output(ERROR, "connect: %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
}

int connect_ipv6(int *sockFd)
{
    int rtrn;
    
    union
    {
        struct sockaddr_in in;
        struct sockaddr_in6 in6;
        
    }addr;
    
    addr.in6.sin6_family = AF_INET6;
    addr.in6.sin6_port = htons(map->socket_server_port + 1);
    inet_pton(AF_INET6, "::1", &addr.in6.sin6_addr);
    
    *sockFd = socket(AF_INET6, SOCK_STREAM, 0);
    if(*sockFd < 0)
    {
        output(ERROR, "socket: %s\n", strerror(errno));
        return -1;
    }
    
    rtrn = connect(*sockFd, (struct sockaddr *)&addr.in6, sizeof(addr.in6));
    if(rtrn < 0)
    {
        output(ERROR, "connect: %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
}

static int setup_ipv4_tcp_server(int *sockFd)
{
    /* Declarations */
    int rtrn;
    
    union
    {
        struct sockaddr sa;
        struct sockaddr_in in;
        
    }address;
    
    *sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if(*sockFd < 0)
    {
        output(ERROR, "Socket: %s\n", strerror(errno));
        return -1;
    }
    
    address.in.sin_len = sizeof(address.in);
    address.in.sin_family = AF_INET;
    address.in.sin_port = htons(map->socket_server_port);
    address.in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    memset(address.in.sin_zero, 0, sizeof(address.in.sin_zero));
    
    /* Bind socket to port */
    rtrn = bind(*sockFd, &address.sa, address.sa.sa_len);
    if(rtrn < 0)
    {
        output(ERROR, "Bind: %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
}

static int setup_ipv6_tcp_server(int *sockFd)
{
    /* Declarations */
    //int yes = 1;
    int rtrn;
    
    /* Create union so we can avoid casting
     and we glom together ipv4 and ipv6. */
    union
    {
        struct sockaddr sa;
        struct sockaddr_in6 in6;
        
    }address;
    
    *sockFd = socket(AF_INET6, SOCK_STREAM, 0);
    if(*sockFd < 0)
    {
        output(ERROR, "Socket ipv6: %s\n", strerror(errno));
        return -1;
    }
    
    address.in6.sin6_len = sizeof(address.in6);
    address.in6.sin6_family = AF_INET6;
    address.in6.sin6_port = htons(map->socket_server_port + 1);
    address.in6.sin6_flowinfo = 0;
    address.in6.sin6_addr = in6addr_loopback;
    address.in6.sin6_scope_id = 0;
    
    /* Bind socket to port */
    rtrn = bind(*sockFd, (struct sockaddr *)&address.in6, address.in6.sin6_len);
    if(rtrn < 0)
    {
        output(ERROR, "bind 6: %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
}

static int setup_socket_server(int *sockFd4, int *sockFd6)
{
    output(STD, "Setting Up Socket Server\n");
    int rtrn;
    
    rtrn = setup_ipv4_tcp_server(sockFd4);
    if(rtrn < 0)
    {
        output(ERROR, "Can't set up ipv4 socket server\n");
        return -1;
    }
    
    rtrn = setup_ipv6_tcp_server(sockFd6);
    if(rtrn < 0)
    {
        output(ERROR, "Can't set up ipv6 socket server\n");
        return -1;
    }
    
    return 0;
}

static int accept_client(int listenFd)
{
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    int clientFd;
    
    clientFd = accept(listenFd, (struct sockaddr *)&addr, &addr_len);
    if(clientFd < 0)
    {
        output(ERROR, "accept: %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
}

static void *thread_start(void *obj)
{
    int rtrn;
    int *sockFd = (int *)obj;
    
    while(atomic_load(&map->stop) == FALSE)
    {
        rtrn = listen(*sockFd, 1024);
        if(rtrn < 0)
        {
            output(ERROR, "listen: %s\n", strerror(errno));
            return NULL;
        }
        
        rtrn = accept_client(*sockFd);
        if(rtrn < 0)
        {
            output(ERROR, "Can't accept\n");
            return NULL;
        }
    }
    
    return NULL;
}

static int _start_socket_server(int listenFd4, int listenFd6)
{
    output(STD, "Starting Socket Server\n");
    
    int rtrn;
    pthread_t ipv6AcceptThread;
    
    pthread_create(&ipv6AcceptThread, NULL, thread_start, &listenFd6);
    
    while(atomic_load(&map->stop) == FALSE)
    {
        rtrn = listen(listenFd4, 1024);
        if(rtrn < 0)
        {
            output(ERROR, "listen: %s\n", strerror(errno));
            return -1;
        }
        
        rtrn = accept_client(listenFd4);
        if(rtrn < 0)
        {
            output(ERROR, "Can't accept\n");
            return -1;
        }
    }
    
    pthread_join(ipv6AcceptThread, NULL);

    return 0;

}

static int select_port_number(void)
{
    unsigned int offset;
    int rtrn;
    
    rtrn = rand_range(10000, &offset);
    if(rtrn < 0)
    {
        output(ERROR, "Can't generate random number\n");
        return -1;
    }
    
    map->socket_server_port = 1200 + offset;
    
    return 0;
}

int start_socket_server(void)
{
	int sockFd4 = 0, sockFd6 = 0;
	int rtrn;
    pid_t socket_server_pid;

    /* Pick a random port above 1200. */
	rtrn = select_port_number();
	if(rtrn < 0)
	{
        output(ERROR, "Can't select port number\n");
        return -1;
	}

    socket_server_pid = fork();
    if(socket_server_pid == 0)
    {
        /* Set the PID of the socket server. */
        cas_loop_int32(&map->socket_server_pid, socket_server_pid);

        // Socket server process
        rtrn = setup_socket_server(&sockFd4, &sockFd6);
        if(rtrn < 0)
        {
            output(ERROR, "Can't set up socket server\n");
            return -1;
        }
        
        rtrn = _start_socket_server(sockFd4, sockFd6);
        if(rtrn < 0)
        {
            output(ERROR, "Can't start socket server\n");
            return -1;
        }
        
    }
    else if(socket_server_pid > 0)
    {
        // Parent process
        
        return 0;
        
    }
    else
    {
        // fork failed
        output(ERROR, "fork: %s\n", strerror(errno));
        return -1;
    }

	return 0;
}
