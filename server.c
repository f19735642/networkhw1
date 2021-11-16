#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define SIZE 4096

int count = 0;
char webpage[SIZE] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n";


void write_file(int sockfd){
    int n;
    FILE *fp;
    char filename[SIZE] = "file";
    char buffer[SIZE];
    char counter[20];

    sprintf(counter, "%d", count);
    strcat(filename, counter);
    fp = fopen(filename, "w");
    if (fp == NULL){
      perror("[-]Error in writing file");
      exit(1);
    }
    printf("counter:%d\n", count);
    while(1){
      n = recv(sockfd, buffer, SIZE, 0);
      if (n<=0){
        printf("recv <= 0\n");
        break;
        return;
      }
    fwrite(buffer, sizeof(buffer), 1, fp);
    printf("%s\n", buffer);
    bzero(buffer, SIZE);

    }
    return;
}

int main(){

  int e;
  pid_t PID;

  int sockfd, new_sock;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;
  char buffer[SIZE];
  char line[SIZE];


  FILE *fp;
  fp = fopen("form.html", "rb");
  while(fgets(line,SIZE,fp))
  strcat(webpage, line);


  printf("%s\n", webpage);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0){
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created. \n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e < 0){
    perror("Error in binding");
    exit(1);
  }
  printf("[+]Binding successful\n");

  e = listen(sockfd, 10);
  if(e == 0){
    printf("[+]Listening...\n");
  }else{
    perror("[-]Error in listening");
    exit(1);
  }
  while(1){
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);

    if(new_sock == -1){
      perror("Connection failed");
      continue;
    }

    count++;
    if(!fork()){
      printf("Child process...\n");
      close(sockfd);
      memset(buffer, 0, sizeof(buffer));
      read(new_sock, buffer, sizeof(buffer));

      write(new_sock, webpage, sizeof(webpage) - 1);
      write_file(new_sock);
      printf("[+]Data writing successful\n");
      close(new_sock);
      exit(0);
    }
    else{
      close(new_sock);
      continue;
    }
  }
  return 0;
}
