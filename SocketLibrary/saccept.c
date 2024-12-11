#include <stdio.h>
#include <winsock2.h>  // Thay sys/socket.h bằng winsock2.h
#include <ws2tcpip.h>
#include <fcntl.h>
#include <errno.h>
#include "socklib.h"

// Khởi tạo Winsock
extern int errno;

int 
saccept (int s)
{
    struct sockaddr_in from;
    int fromlen;
    int ns;

    sclrerr();

    fromlen = sizeof(from);
    // Kiểm tra xem Winsock có sẵn không trước khi gọi accept
    ns = accept(s, (struct sockaddr *)&from, &fromlen);
    if (ns < 0)
    {
        int err_code = WSAGetLastError(); // Sử dụng WSAGetLastError() thay vì errno
        serrno = SE_SYSERR;
        sename = "accept";
        printf("Error in accept: %d\n", err_code);  // In lỗi để kiểm tra
        return -1;
    }
    return ns;
}

int test_accept(int socket_listen, int *returnedSocket)
{
    /* returns -1 for error, 0 for block, 1 for success */

    fd_set fdset;
    static struct timeval timeout = {0, 0};  /* For a poll */
    int val, socket_talk;

    FD_ZERO(&fdset);
    FD_SET(socket_listen, &fdset);

    // Dùng select để kiểm tra kết nối sẵn sàng (polling)
    val = select(FD_SETSIZE, &fdset, NULL, NULL, &timeout);
    if ((val == 0) || (val == -1))
        return val;

    // Nếu socket đã sẵn sàng cho accept
    socket_talk = saccept(socket_listen);
    if (socket_talk < 0)
    {
        int err_code = WSAGetLastError(); // Sử dụng WSAGetLastError() thay vì errno
        if ((err_code == WSAEWOULDBLOCK))
            return 0;
        sperror("test_accept failed");
        exit(1);
    }

    *returnedSocket = socket_talk;
    return 1;  /* SUCCESS */
}
