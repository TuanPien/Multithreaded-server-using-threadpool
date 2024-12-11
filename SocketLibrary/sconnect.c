#include <stdio.h>
#include <winsock2.h>   // Sử dụng winsock2.h thay vì sys/socket.h
#include <ws2tcpip.h>   // Thêm ws2tcpip.h cho các cấu trúc địa chỉ Internet
#include "socklib.h"

// Khởi tạo Winsock
extern int errno;

int 
sconnect (char *hostname, char *servicename)
{
    struct sockaddr_in remote;
    struct sockaddr_in local;
    int s;
    int protonum;

    sclrerr ();

    // Khởi tạo Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return -1;  // Nếu khởi tạo Winsock thất bại
    }

    if (make_inetaddr (hostname, servicename, &remote) < 0) {
        WSACleanup();  // Dọn dẹp Winsock trước khi thoát
        return -1;
    }

    if ((protonum = protonumber ("tcp")) < 0) {
        WSACleanup();  // Dọn dẹp Winsock trước khi thoát
        return -1;
    }

    if ((s = socket (PF_INET, SOCK_STREAM, protonum)) < 0)
    {
        serrno = SE_SYSERR;
        sename = "socket";
        WSACleanup();  // Dọn dẹp Winsock khi gặp lỗi
        return -1;
    }

    if (make_inetaddr ((char *) 0, (char *) 0, &local) < 0) {
        closesocket(s);  // Đóng socket khi gặp lỗi
        WSACleanup();    // Dọn dẹp Winsock khi gặp lỗi
        return -1;
    }

    if (bind (s, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
        serrno = SE_SYSERR;
        sename = "bind";
        closesocket(s);  // Đóng socket khi gặp lỗi
        WSACleanup();    // Dọn dẹp Winsock khi gặp lỗi
        return -1;
    }

    if (connect (s, (struct sockaddr *)&remote, sizeof(remote)) < 0)
    {
        serrno = SE_SYSERR;
        sename = "connect";
        closesocket(s);  // Đóng socket khi gặp lỗi
        WSACleanup();    // Dọn dẹp Winsock khi gặp lỗi
        return -1;
    }

    // Không cần gọi WSACleanup() ở đây, vì nó sẽ được gọi khi chương trình dừng

    return s;
}
