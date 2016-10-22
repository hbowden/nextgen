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

#include "network.h"
#include "io/io.h"

#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int32_t setup_ipv4_tcp_server(int32_t *sockFd)
{
    /* Declarations */
    int32_t rtrn = 0;

    union {
        struct sockaddr sa;
        struct sockaddr_in in;

    } address;

    *sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if(*sockFd < 0)
    {
        printf("Socket: %s\n", strerror(errno));
        return (-1);
    }

    uint32_t ss_port;
    get_server_port(&ss_port);

    address.in.sin_len = sizeof(address.in);
    address.in.sin_family = AF_INET;
    address.in.sin_port = htons(ss_port);
    address.in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    memset(address.in.sin_zero, 0, sizeof(address.in.sin_zero));

    /* Bind socket to port */
    rtrn = bind(*sockFd, &address.sa, address.sa.sa_len);
    if(rtrn < 0)
    {
        printf("Bind: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}

int32_t setup_ipv6_tcp_server(int32_t *sockFd)
{
    /* Declarations */
    int32_t rtrn = 0;

    /* Create union so we can avoid casting
     and we glom together ipv4 and ipv6. */
    union {
        struct sockaddr sa;
        struct sockaddr_in6 in6;

    } address;

    *sockFd = socket(AF_INET6, SOCK_STREAM, 0);
    if(*sockFd < 0)
    {
        printf("Socket ipv6: %s\n", strerror(errno));
        return (-1);
    }

    uint32_t ss_port;
    get_server_port(&ss_port);

    address.in6.sin6_len = sizeof(address.in6);
    address.in6.sin6_family = AF_INET6;
    address.in6.sin6_port = htons(ss_port + 1);
    address.in6.sin6_flowinfo = 0;
    address.in6.sin6_addr = in6addr_loopback;
    address.in6.sin6_scope_id = 0;

    /* Bind socket to port */
    rtrn = bind(*sockFd, (struct sockaddr *)&address.in6, address.in6.sin6_len);
    if(rtrn < 0)
    {
        printf("bind 6: %s\n", strerror(errno));
        return (-1);
    }

    return (0);
}
