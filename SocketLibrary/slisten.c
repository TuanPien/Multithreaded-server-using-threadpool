/*
 * slisten.c -- create a socket that will be listening for connections 
 */

#include <stdio.h>
#include <winsock2.h>  // Sử dụng winsock2.h thay vì sys/socket.h và netinet/in.h
#include <ws2tcpip.h>
#include "socklib.h"

// Khởi tạo Winsock
extern int errno;

int 
slisten (char *servicename)
{
    struct sockaddr_in inaddr;
    int s;
    int protonum;

    sclrerr ();

    // Khởi tạo Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return -1;  // Nếu khởi tạo Winsock thất bại
    }

    if ((protonum = protonumber("tcp")) < 0)
        return -1;

    if ((s = socket(PF_INET, SOCK_STREAM, protonum)) < 0)
    {
        serrno = SE_SYSERR;
        sename = "socket";
        WSACleanup();  // Dọn dẹp Winsock trước khi thoát
        return -1;
    }

    // Tạo địa chỉ inet
    if (make_inetaddr((char *)0, servicename, &inaddr) < 0)
        return -1;

    if (bind(s, (struct sockaddr *)&inaddr, sizeof(inaddr)) < 0)
    {
        serrno = SE_SYSERR;
        sename = "bind";
        closesocket(s);  // Đóng socket khi có lỗi
        WSACleanup();    // Dọn dẹp Winsock
        return -1;
    }

    // Bắt đầu lắng nghe kết nối
    if (listen(s, 3) < 0)
    {
        serrno = SE_SYSERR;
        sename = "listen";
        closesocket(s);  // Đóng socket khi có lỗi
        WSACleanup();    // Dọn dẹp Winsock
        return -1;
    }

    // Nếu thành công, trả về socket đã mở
    return s;
}
