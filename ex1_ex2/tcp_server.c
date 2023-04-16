#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <port> <greeting file> <client data file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    char *greeting_file = argv[2];
    char *client_data_file = argv[3];

    // Tạo socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Liên kết socket với địa chỉ và cổng
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối
    if (listen(server_fd, 5) == -1)
    {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connection...\n");

    // Chấp nhận kết nối
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1)
    {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Gửi xâu chào
    FILE *greeting_fp = fopen(greeting_file, "r");
    if (greeting_fp == NULL)
    {
        perror("fopen");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    char greeting[256];
    fgets(greeting, sizeof(greeting), greeting_fp);
    fclose(greeting_fp);

    if (send(client_fd, greeting, strlen(greeting), 0) == -1)
    {
        perror("send");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Greeting sent: %s", greeting);

    // Ghi nội dung client gửi đến vào tệp tin
    FILE *client_data_fp = fopen(client_data_file, "w");
    if (client_data_fp == NULL)
    {
        perror("fopen");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    int num_bytes;

    while ((num_bytes = recv(client_fd, buffer, sizeof(buffer), 0)) > 0)
    {
        if (fwrite(buffer, sizeof(char), num_bytes, client_data_fp) != num_bytes)
        {
            perror("fwrite");
            close(client_fd);
            close(server_fd);
            fclose(client_data_fp);
            exit(EXIT_FAILURE);
        }
    }
    if (num_bytes == -1)
    {
        perror("recv");
        close(client_fd);
        close(server_fd);
        fclose(client_data_fp);
        exit(EXIT_FAILURE);
    }

    fclose(client_data_fp);
    printf("Client data written to file: %s\n", client_data_file);

    // Đóng socket
    close(client_fd);
    close(server_fd);

    return 0;
}