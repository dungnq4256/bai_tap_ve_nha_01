#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char message[BUF_SIZE];

    if (argc != 3) {
        printf("Usage: %s <Server IP> <Port>\n", argv[0]);
        exit(1);
    }

    // Tạo socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    // Thiết lập thông tin của máy chủ
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    // Kết nối đến máy chủ
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect to server");
        exit(1);
    }

    while (1) {
        // Nhập dữ liệu từ bàn phím
        printf("Enter data to send: ");
        fgets(message, BUF_SIZE, stdin);
        message[strlen(message) - 1] = '\0';

        // Gửi dữ liệu đến máy chủ
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("Failed to send data to server");
            exit(1);
        }

        // Nhận dữ liệu từ máy chủ
        memset(message, 0, sizeof(message));
        if (recv(sock, message, BUF_SIZE, 0) < 0) {
            perror("Failed to receive data from server");
            exit(1);
        }
        printf("Received from server: %s\n", message);

        // Nếu người dùng nhập "exit" thì thoát vòng lặp
        if (strcmp(message, "exit") == 0) {
            break;
        }
    }

    // Đóng kết nối socket
    close(sock);

    return 0;
}