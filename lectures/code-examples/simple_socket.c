#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>


int main(int argc, char **argv) {
  int pid = fork();
  int err;

  if (pid < 0) {
    printf("Fork failed!\n");
    return -1;
  } else if (pid > 0) {
    printf("This is parent running the server code!\n");
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(2520);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int server_socket_f = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_f == -1) {
      printf("Server could not creat a socket!\n");
      return -1;
    }
    
    err = bind(server_socket_f, (struct sockaddr*) &addr, sizeof (addr));
    if (err != 0) {
      printf("Bind failed!\n");
      close(server_socket_f);
      return -1;
    }

    err = listen(server_socket_f, 10);
    if (err != 0) {
      printf("Listen failed!\n");
      close(server_socket_f);
      return -1;
    }

    int new_socket_fd = accept(server_socket_f, NULL, NULL);
    if (new_socket_fd == -1) {
      printf("Accept failed!\n");
      close(server_socket_f);
      return -1;
    }

    char *received_msg = malloc(21);
    memset(received_msg, 0, 21);
    int received_len = recv(new_socket_fd, received_msg, 20, 0);
    if (received_len < 0) {
      printf("Receive failed!\n");
      close(new_socket_fd);
      free(received_msg);
      close(server_socket_f);
      return -1;
    }

    printf("Server received: %s\n", received_msg);
    close(new_socket_fd);
    free(received_msg);
    close(server_socket_f);
  } else {
    printf("This is child running client code!\n");
    printf("Client taking a nap for server to set things up!\n");
    sleep(4);

    struct addrinfo hints;
    struct addrinfo *server_info;
    memset(&hints, 0, sizeof (hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int result = getaddrinfo("127.0.0.1", "2520", &hints, &server_info);
    if (result != 0) {
      printf("getaddrinfo failed to find localhost!\n");
      return -1;
    }

    int client_socket_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (client_socket_fd == -1) {
      printf("Client could not create a socket!\n");
      freeaddrinfo(server_info);
      return -1;
    }

    err = connect(client_socket_fd, server_info->ai_addr, server_info->ai_addrlen);
    if (err != 0) {
      printf("Connect failed!\n");
      freeaddrinfo(server_info);
      close(client_socket_fd);
      return -1;
    }

    char *msg = malloc(21);
    memset(msg, 0, 21);
    sprintf(msg, "Message is %d!\n", 110);
    int send_len = strlen(msg) + 1;
    err = send(client_socket_fd, msg, send_len, 0);
    if (err != send_len) {
      printf("Something went wrong with send!\n");
      freeaddrinfo(server_info);
      close(client_socket_fd);
      free(msg);
      return -1;
    }
    printf("Client sent its message!\n");
    freeaddrinfo(server_info);
    close(client_socket_fd);
    free(msg);
  }
  return 0;
}