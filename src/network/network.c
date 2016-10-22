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

#include "network.h"
#include "concurrent/concurrent.h"
#include "crypto/crypto.h"
#include "io/io.h"
#include "runtime/platform.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static uint32_t ss_port;

static int32_t stop;

static int32_t listen_fd4;
static int32_t listen_fd6;

void get_server_port(uint32_t *port)
{
    (*port) = ss_port;

    return;
}

int32_t connect_ipv4(int32_t *sockFd)
{
    int32_t rtrn = 0;

    union
    {
        struct sockaddr_in in;
        struct sockaddr_in6 in6;

    } addr;

    addr.in.sin_family = AF_INET;
    addr.in.sin_port = htons(ss_port);
    inet_pton(AF_INET, "127.0.0.1", &addr.in.sin_addr);

    (*sockFd) = socket(AF_INET, SOCK_STREAM, 0);
    if((*sockFd) < 0)
    {
        printf("socket: %s\n", strerror(errno));
        return (-1);
    }

    rtrn = connect((*sockFd), (struct sockaddr *)&addr.in, sizeof(addr.in));
    if(rtrn < 0)
    {
        printf("connect: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

int32_t connect_ipv6(int32_t *sockFd)
{
    int32_t rtrn = 0;

    union
    {
        struct sockaddr_in in;
        struct sockaddr_in6 in6;

    } addr;

    addr.in6.sin6_family = AF_INET6;
    addr.in6.sin6_port = htons(ss_port + 1);
    inet_pton(AF_INET6, "::1", &addr.in6.sin6_addr);

    (*sockFd) = socket(AF_INET6, SOCK_STREAM, 0);
    if((*sockFd) < 0)
    {
        printf("socket: %s\n", strerror(errno));
        return (-1);
    }

    rtrn = connect((*sockFd), (struct sockaddr *)&addr.in6, sizeof(addr.in6));
    if(rtrn < 0)
    {
        printf("connect: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

static int32_t setup_socket_server(int32_t *sockFd4, int *sockFd6)
{
    printf("Setting Up Socket Server\n");

    int32_t rtrn = 0;

    rtrn = setup_ipv4_tcp_server(sockFd4);
    if(rtrn < 0)
    {
        printf("Can't set up ipv4 socket server\n");
        return (-1);
    }

    rtrn = setup_ipv6_tcp_server(sockFd6);
    if(rtrn < 0)
    {
        printf("Can't set up ipv6 socket server\n");
        return (-1);
    }

    return (0);
}

static int32_t accept_client(int listenFd)
{
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    int32_t clientFd = 0;

    clientFd = accept(listenFd, (struct sockaddr *)&addr, &addr_len);
    if(clientFd < 0)
    {
        printf("accept: %s\n", strerror(errno));
        return (-1);
    }

    close(clientFd);

    return (0);
}

static void *accept_thread_start(void *obj)
{
    int rtrn;
    int *sockFd = (int *)obj;

    while(ck_pr_load_int(&stop) != TRUE)
    {
        rtrn = listen(*sockFd, 1024);
        if(rtrn < 0)
        {
            printf("listen: %s\n", strerror(errno));
            return NULL;
        }

        rtrn = accept_client(*sockFd);
        if(rtrn < 0)
        {
            printf("Can't accept\n");
            return NULL;
        }
    }

    return NULL;
}

static void *start_thread(void *arg)
{
    printf("Starting Socket Server\n");

    (void)arg;

    int32_t rtrn = 0;
    pthread_t ipv6AcceptThread = 0;

    rtrn = pthread_create(&ipv6AcceptThread, NULL, accept_thread_start, &listen_fd6);
    if(rtrn < 0)
    {
        printf("Can't start the IPV6 accept thread\n");
        return (NULL);
    }

    while(ck_pr_load_int(&stop) != TRUE)
    {
        rtrn = listen(listen_fd4, 1024);
        if(rtrn < 0)
        {
            printf("listen: %s\n", strerror(errno));
            return (NULL);
        }

        rtrn = accept_client(listen_fd4);
        if(rtrn < 0)
        {
            printf("Can't accept\n");
            return (NULL);
        }
    }

    pthread_join(ipv6AcceptThread, NULL);

    return (NULL);
}

int32_t pick_random_port(uint32_t *port)
{
    int32_t rtrn = 0;
    uint32_t offset = 0;

    /* Should not be grabbing allocator, console writter and random
      generator here should be passed in. However this function is
      depreacted so it will be deleted soon. */
    struct memory_allocator *allocator = NULL;
    allocator = get_default_allocator();
    if(allocator == NULL)
    {
        printf("Failed to get allocator\n");
        return (-1);
    }
    struct output_writter *output = NULL;
    output = get_console_writter();
    if(output == NULL)
    {
        printf("Failed to get console writter\n");
        return (-1);
    }

    struct random_generator *random = NULL;
    random = get_default_random_generator(allocator, output);
    if(random == NULL)
    {
        printf("Failed to get random number generator\n");
        return (-1);
    }

    rtrn = random->range(10000, &offset);
    if(rtrn < 0)
    {
        printf("Can't generate random number\n");
        return (-1);
    }

    (*port) = (1200 + offset);

    return (0);
}

static int32_t select_port_number(void)
{
    int32_t rtrn = 0;

    rtrn = pick_random_port(&ss_port);
    if(rtrn < 0)
    {
        printf("Can't pick random port\n");
        return (-1);
    }

    return (0);
}

static int32_t start_socket_server(void)
{
    int32_t rtrn = 0;
    pthread_t thread = 0;

    /* Pick a random port above 1200. */
    rtrn = select_port_number();
    if(rtrn < 0)
    {
        printf("Can't select port number\n");
        return (-1);
    }

    /* Setup the socket server threads and have them start
    listening on the port selected earlier. */
    rtrn = setup_socket_server(&listen_fd4, &listen_fd6);
    if(rtrn < 0)
    {
        printf("Can't set up socket server\n");
        return (-1);
    }

    rtrn = pthread_create(&thread, NULL, start_thread, NULL);
    if(rtrn < 0)
    {
        printf("Can't start socket server thread\n");
        return (-1);
    }

    return (0);
}

int32_t setup_network_module(enum network_mode mode)
{
    int32_t rtrn = 0;

    switch((int32_t)mode)
    {
        case SOCKET_SERVER:

            rtrn = start_socket_server();
            if(rtrn < 0)
            {
                printf("Can't setup the socket server\n");
                return (-1);
            }

            break;

        default:
            printf("Unknown setup option\n");
            return (-1);
    }

    return (0);
}
