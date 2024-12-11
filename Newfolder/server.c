#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <pthread.h>

#pragma comment(lib, "ws2_32.lib")  // Liên kết với thư viện Winsock

// Hàm xử lý kết nối với client
void *handle_client(void *client_socket) {
    SOCKET sock = *(SOCKET *)client_socket;
    char buffer[1024];
    int bytes_read;

    // Lấy thời gian bắt đầu xử lý client
    pthread_t thread_id = pthread_self();
    printf("Thread %lu started processing client.\n", thread_id);

    // Đọc dữ liệu từ client
    while ((bytes_read = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_read] = '\0';  // Đảm bảo kết thúc chuỗi
        printf("Thread %lu received from client: %s\n", thread_id, buffer);

        // Gửi lại dữ liệu cho client
        send(sock, buffer, bytes_read, 0);
    }

    if (bytes_read == 0) {
        printf("Thread %lu: Client disconnected\n", thread_id);
    } else if (bytes_read == SOCKET_ERROR) {
        printf("Thread %lu: recv failed with error code: %d\n", thread_id, WSAGetLastError());
    }

    // Đóng kết nối
    closesocket(sock);
    printf("Thread %lu finished processing client.\n", thread_id);
    free(client_socket);  // Giải phóng bộ nhớ đã cấp phát cho socket
    return NULL;
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    int client_index = 0;

    // Khởi tạo thư viện WinSock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Tạo socket cho server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Cấu hình địa chỉ server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // Gắn địa chỉ server với socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Lắng nghe các kết nối đến
    if (listen(server_fd, 10) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port 8080...\n");

    // Vòng lặp để chấp nhận các kết nối client
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == INVALID_SOCKET) {
            printf("Accept failed: %d\n", WSAGetLastError());
            continue;
        }

        client_index++;
        printf("Client %d connected: %s\n", client_index, inet_ntoa(client_addr.sin_addr));

        // Cấp phát bộ nhớ cho client socket và tạo thread mới để xử lý client
        SOCKET *new_sock = malloc(sizeof(SOCKET));
        if (new_sock == NULL) {
            printf("Malloc failed\n");
            closesocket(client_fd);
            continue;
        }
        *new_sock = client_fd;

        // Tạo thread mới để xử lý client bằng pthread
        pthread_t thread;
        int thread_result = pthread_create(&thread, NULL, handle_client, (void *)new_sock);
        if (thread_result != 0) {
            printf("Thread creation failed: %d\n", thread_result);
            closesocket(client_fd);
            free(new_sock);
            continue;
        }

        // Đảm bảo thread sẽ tự giải phóng khi kết thúc
        pthread_detach(thread);
    }

    // Đóng socket server
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
