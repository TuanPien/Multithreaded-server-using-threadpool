#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <winsock2.h>  // Thư viện cho Winsock trên Windows
#include "SocketLibrary/socklib.h"
#include "common.h"

void initialize_winsock() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);  // Khởi tạo Winsock

    if (iResult != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", iResult);
        exit(1);
    }
}

void cleanup_winsock() {
    int iResult = WSACleanup();  // Dọn dẹp Winsock
    if (iResult != 0) {
        fprintf(stderr, "WSACleanup failed: %d\n", iResult);
    }
}

int main(int argc, char **argv) {
    int socket_talk, i;
    char request[REQUEST_SIZE];
    char response[RESPONSE_SIZE];

    // Khởi tạo Winsock
    initialize_winsock();

    if (argc != 3) {
        fprintf(stderr, "(CLIENT): Invoke as 'client machine.name.address socknum'\n");
        cleanup_winsock();  // Dọn dẹp Winsock trước khi thoát
        exit(1);
    }

    // Initialize request to some silly data
    for (i = 0; i < REQUEST_SIZE; i++) {
        request[i] = (char)(i % 255);
    }

    // Spin forever, opening connections, and pushing requests
    while (1) {
        int result;

        // Open up a connection to the server
        if ((socket_talk = sconnect(argv[1], argv[2])) < 0) {
            perror("(CLIENT): sconnect");
            cleanup_winsock();  // Dọn dẹp Winsock trước khi thoát
            exit(1);
        }

        // Write the request
        result = correct_write(socket_talk, request, REQUEST_SIZE);
        if (result == REQUEST_SIZE) {
            // Read the response
            result = correct_read(socket_talk, response, RESPONSE_SIZE);
        }
        fprintf(stderr, "Result from server = %d\n", result);

        // Close the socket (use closesocket on Windows)
        closesocket(socket_talk);
    }

    // Dọn dẹp Winsock trước khi thoát
    cleanup_winsock();

    return 0;
}
