#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int port;
    char* log_filename;
    FILE *log_file;

    // Kiểm tra đúng số lượng tham số dòng lệnh
    if (argc != 3) {
        printf("Sử dụng: %s <cổng> <file log>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Lấy thông tin cổng và tên file log từ tham số dòng lệnh
    port = atoi(argv[1]);
    log_filename = argv[2];

    // Tạo socket và liên kết địa chỉ IP và cổng
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Lắng nghe kết nối từ client
    listen(server_sock, 5);

    // Mở file log để ghi
    log_file = fopen(log_filename, "a");
    if (log_file == NULL) {
        perror("Không thể mở file log");
        exit(EXIT_FAILURE);
    }

    // Chờ kết nối từ client
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);

        // Lấy thông tin thời gian hiện tại
        time_t current_time;
        struct tm *time_info;
        char time_str[BUFFER_SIZE];
        time(&current_time);
        time_info = localtime(&current_time);
        strftime(time_str, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", time_info);

        // Nhận dữ liệu từ client
        char buffer[BUFFER_SIZE];
        int n = read(client_sock, buffer, BUFFER_SIZE);

        // In thông tin của client và ghi vào file log
        if (n > 0) {
            buffer[n] = '\0';
            printf("%s %s\n", inet_ntoa(client_addr.sin_addr), buffer);
            fprintf(log_file, "%s %s %s\n", inet_ntoa(client_addr.sin_addr), time_str, buffer);
            fflush(log_file);
        }

        // Đóng kết nối với client
        close(client_sock);
    }

    // Đóng file log và socket
    fclose(log_file);
    close(server_sock);

    return 0;
}