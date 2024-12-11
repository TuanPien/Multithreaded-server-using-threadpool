/*
 * sprim.c -- some primitives to take the pain out of creating addresses
 *
 * make_inetaddr(host/inet dot-style, port/servicename, &struct sockaddr_in); 
 *
 * int protonumber(protoname) 
 */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <winsock2.h>  // Thay sys/socket.h bằng winsock2.h
#include <ws2tcpip.h>
#include <stdlib.h>
#include "socklib.h"

int     serrno;
char   *sename;

int 
make_inetaddr (char *hostname, char *servicename, struct sockaddr_in *inaddr)
{
    struct hostent *host;
    struct servent *service;

    sclrerr ();

    inaddr->sin_family = AF_INET;

    // Xử lý địa chỉ IP (IPv4)
    if (hostname == NULL)
    {
        inaddr->sin_addr.s_addr = INADDR_ANY;  // INADDR_ANY = 0
    }
    else if (isdigit (*hostname))  // Kiểm tra xem hostname có phải là IP không
    {
        inaddr->sin_addr.s_addr = inet_addr (hostname);
    }
    else
    {
        // Lấy thông tin host từ gethostbyname
        if ((host = gethostbyname (hostname)) == NULL)
        {
            fprintf(stdout, "Unknown host..\n");
            serrno = SE_UNKHOST;
            sename = "gethostbyname";
            return -1;
        }
        if (host->h_addrtype != AF_INET)
        {
            fprintf(stdout, "Unknown address family\n");
            serrno = SE_UNKAF;
            return -1;
        }

        // Sao chép địa chỉ IP vào sockaddr_in
        memcpy(&inaddr->sin_addr.s_addr, host->h_addr, host->h_length);
    }  

    // Xử lý port (sử dụng dịch vụ nếu không phải là số)
    if (servicename == NULL)
    {
        inaddr->sin_port = 0;
    }
    else if (isdigit (*servicename))
    {
        inaddr->sin_port = htons(atoi (servicename));  // Chuyển số port sang dạng network byte order
    }
    else
    {
        // Lấy thông tin dịch vụ từ getservbyname
        if ((service = getservbyname (servicename, "tcp")) == NULL)
        {
            fprintf(stdout, "Unknown service.\n");
            serrno = SE_UNKSERV;
            sename = "getservbyname";
            return -1;
        }
        inaddr->sin_port = service->s_port;
    }
    return 0;
}

int 
protonumber (char *protoname)
{
    struct protoent *proto;

    sclrerr ();

    // Lấy thông tin protocol từ getprotobyname
    if ((proto = getprotobyname (protoname)) == NULL)
    {
        serrno = SE_UNKPROT;
        sename = "getprotobyname";
        return -1;
    }
    return proto->p_proto;
}

void 
sclrerr ()
{
    serrno = SE_NOERR;
    sename = NULL;
}

char   *s_errlist[] = {
    "no error",
    "system error",
    "unknown address family",
    "unknown host",
    "unknown service",
    "unknown protocol",
    "out of memory"
};

void 
sperror (char *msg)
{
    fprintf (stderr, "%s: ", msg);

    if (sename != NULL)
        fprintf (stderr, "%s: ", sename);

    // Thay thế sys_errlist bằng strerror(errno) cho Unix
    if (serrno == SE_SYSERR)
    {
        #ifdef _WIN32
        // Windows sử dụng WSAGetLastError() để lấy mã lỗi
        int ws_error = WSAGetLastError();
        fprintf(stderr, "Winsock error: %d\n", ws_error);
        #else
        // Unix/Linux sử dụng strerror(errno)
        fprintf (stderr, "%s\n", strerror(errno));
        #endif
    }
    else
    {
        fprintf (stderr, "%s\n", s_errlist[serrno]);
    }
}
