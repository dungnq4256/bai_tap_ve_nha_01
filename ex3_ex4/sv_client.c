#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_port = atoi(argv[2]);

    // Tạo socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Tạo địa chỉ của server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) != 1) {
        perror("inet_pton");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Kết nối tới server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Nhập dữ liệu từ người dùng
    char mssv[MAX_BUF_SIZE];
    char hoten[MAX_BUF_SIZE];
    char ngaysinh[MAX_BUF_SIZE];
    float diemtb;
    printf("Nhap MSSV: ");
    fgets(mssv, MAX_BUF_SIZE, stdin);
    printf("Nhap ho ten: ");
    fgets(hoten, MAX_BUF_SIZE, stdin);
    printf("Nhap ngay sinh: ");
    fgets(ngaysinh, MAX_BUF_SIZE, stdin);
    printf("Nhap diem trung binh: ");
    scanf("%f", &diemtb);

    // Đóng gói dữ liệu và gửi tới server
    char buffer[MAX_BUF_SIZE];
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%s|%s|%s|%.2f", mssv, hoten, ngaysinh, diemtb);
    int num_bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
    if (num_bytes_sent == -1) {
        perror("send");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Nhận kết quả từ server (không cần thiết cho bài toán này)
    char response[MAX_BUF_SIZE];
    int num_bytes_received = recv(client_fd, response, sizeof(response), 0);
    if (num_bytes_received == -1) {
        perror("recv");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    response[num_bytes_received] = '\0';
    printf("Server response: %s\n", response);

    // Đóng kết nối và kết thúc chương trình
    close(client_fd);
    return 0;
}