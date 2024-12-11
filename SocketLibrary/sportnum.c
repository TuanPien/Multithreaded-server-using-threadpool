/*
 * sportnum.c -- return the port number of the passed socket 
 */

#include <stdio.h>
#include <winsock2.h>  // Thay sys/socket.h bằng winsock2.h
#include <ws2tcpip.h>
#include "socklib.h"

// Khởi tạo Winsock
extern int errno;

int 
sportnum (int s)
{
    struct sockaddr_in sockname;  // Thay struct sockaddr thành sockaddr_in
    int len;

    sclrerr ();

    len = sizeof(sockname);
    if (getsockname(s, (struct sockaddr *)&sockname, &len) < 0)
    {
        serrno = SE_SYSERR;
        sename = "getsockname";
        return -1;
    }
    if (sockname.sin_family != AF_INET)
    {
        serrno = SE_UNKAF;
        return -1;
    }

    return ntohs(sockname.sin_port);  // Dùng ntohs để chuyển đổi từ mạng byte order về host byte order
}
