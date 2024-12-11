#include <winsock2.h>  // Thư viện cần thiết cho Winsock trên Windows

int correct_read(int s, char *data, int len) {
    int sofar, ret;
    char *tmp;

    sofar = 0;
    while (sofar != len) {
        tmp = data + (unsigned long) sofar;
        ret = recv(s, tmp, len - sofar, 0);  // Sử dụng recv thay vì read

        if (ret == SOCKET_ERROR) {
            // Kiểm tra lỗi bằng WSAGetLastError() trên Windows
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                return 0;  // Socket đang chờ, không có dữ liệu
            } else {
                perror("Read error");
                return -1;  // Lỗi không xác định
            }
        }
        else if (ret == 0) {
            return 0;  // Kết nối đã đóng
        } else {
            sofar += ret;
        }
    }

    return len;
}

int correct_write(int s, char *data, int len) {
    int sofar, ret;
    char *tmp;

    if (len == -1) 
        len = strlen(data);
  
    sofar = 0;
    while (sofar != len) {
        tmp = data + (unsigned long) sofar;
        ret = send(s, tmp, len - sofar, 0);  // Sử dụng send thay vì write

        if (ret == SOCKET_ERROR) {
            // Kiểm tra lỗi bằng WSAGetLastError() trên Windows
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK) {
                return 0;  // Socket đang chờ, không thể ghi dữ liệu
            } else {
                perror("Write error");
                return -1;  // Lỗi không xác định
            }
        } else {
            sofar += ret;
        }
    }
    return len;
}
