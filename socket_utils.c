/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/_default_fcntl.h>
#include "process_utils.h"
#include "socket_utils.h"
#include "mutils.h"
#include "shared.h"

struct s_socket init_client_socket(const char *adress, int port_number){
    struct s_socket cl_socket;
    memset(&cl_socket, 0, sizeof(cl_socket));
    
    if ((cl_socket.sock_fd = socket(AF_INET, SOCK_STREAM, 0)) > 0){
        cl_socket.addr.sin_addr.s_addr = inet_addr(adress);
        cl_socket.addr.sin_family = AF_INET;
        cl_socket.addr.sin_port = htons(port_number);
        cl_socket.s_size = sizeof(cl_socket.addr);
        cl_socket.s_ip = malloc(sizeof(cl_socket.addr));
        int sopt = 1;
        setsockopt(cl_socket.sock_fd, SOL_SOCKET,SO_REUSEADDR, &sopt, sizeof(sopt));
    }
    else {
        shutdown(cl_socket.sock_fd, SHUT_RDWR);
        close(cl_socket.sock_fd);
        cl_socket.sock_fd = -1;
        perror(EMSG);
    }
   
    return cl_socket;
}

int reinit_cl_socket(struct s_socket *cl_socket, const char *adress, int port_number){
    close(cl_socket->sock_fd);
    free(cl_socket->s_ip);
    memset(cl_socket, 0, sizeof(cl_socket));
    if ((cl_socket->sock_fd = socket(AF_INET, SOCK_STREAM, 0)) > 0){
        cl_socket->addr.sin_addr.s_addr = inet_addr(adress);
        cl_socket->addr.sin_family = AF_INET;
        cl_socket->addr.sin_port = htons(port_number);
        cl_socket->s_size = sizeof(cl_socket->addr);
        cl_socket->s_ip = malloc(sizeof(cl_socket->addr));
        int sopt = 1;
        setsockopt(cl_socket->sock_fd, SOL_SOCKET, SO_REUSEADDR, &sopt, sizeof(sopt));
    }
    else {
        shutdown(cl_socket->sock_fd, SHUT_RDWR);
        close(cl_socket->sock_fd);
        cl_socket->sock_fd = -1;
        perror(EMSG);
        return 0;
    }
    return 1;
}

int listen_for_input(struct s_socket *cl_socket, struct c_array *in_array){
    fd_set fd_in;
    fd_set fd_in_orig;
    FD_ZERO(&fd_in);
    FD_ZERO(&fd_in_orig);
    FD_SET(STDIN_FILENO, &fd_in_orig);
    FD_SET(cl_socket->sock_fd, &fd_in_orig);
    int max_fd = cl_socket->sock_fd;
    char c_pack = '\0';
    while (1){
        memcpy(&fd_in, &fd_in_orig, sizeof(fd_in_orig));
        if (select(max_fd + 1, &fd_in, NULL, NULL, NULL) > 0){
            if (FD_ISSET(cl_socket->sock_fd, &fd_in)){
                if (recv(cl_socket->sock_fd, &c_pack, sizeof(char), 0) > 0){
                    if (c_pack == ACK_INCOMING){
                        send_formatted_data(cl_socket, in_array);
                        clear_c_array(in_array);
                        printf("data sent\n");
                        fflush(stdout);
                    }
                    else if (c_pack == ACK_FIN || c_pack == FIN_SHD) {
                        shutdown(cl_socket->sock_fd, SHUT_RDWR);
                        close(cl_socket->sock_fd);
                        clear_c_array(in_array);
                        max_fd = STDIN_FILENO;
                        FD_CLR(cl_socket->sock_fd, &fd_in_orig);
                        printf("server disconnected\n");
                        fflush(stdout);
                        break;
                    } 
                }
                else {
                    close(cl_socket->sock_fd);
                    max_fd = STDIN_FILENO;
                    FD_CLR(cl_socket->sock_fd, &fd_in_orig);
                    printf("server disconnected\n");
                    fflush(stdout);
                }
            } 
            else if (FD_ISSET(STDIN_FILENO, &fd_in)){
                if (in_array->fill_size < BUFF_SIZE){
                    char c;
                    read(STDIN_FILENO, &c, 1);
                    add_c_to_array(in_array, c);
                }
                if(in_array->fill_size >= BUFF_SIZE){
                    c_pack = INCOMING;
                    if (send(cl_socket->sock_fd, &c_pack, sizeof(char), 0) < 0){
                        printf("reconnecting\n");
                        fflush(stdout);
                        if (reinit_cl_socket(cl_socket, SERVER_ADRESS, SERVER_PORT) > 0){
                            if (connect_socket(cl_socket) && !FD_ISSET(cl_socket->sock_fd, &fd_in_orig)){
                                FD_SET(cl_socket->sock_fd, &fd_in_orig);
                                max_fd = cl_socket->sock_fd;
                            }
                        }
                    }
                }
                read(STDIN_FILENO, NULL, 1); //Consuming character from input
            }
        }  
    }

    return 0;
}

int send_formatted_data(struct s_socket *client, struct c_array *array){
    int bytesSend;
    struct tm time;
    const char *format = "Sent from %s at %s : %s";
    const char *ctime = getlocaltime(&time);
    char buff[strlen(format) + strlen(ctime) + array->fill_size + strlen(client->s_ip)];
    snprintf(buff, sizeof(buff),format, client->s_ip, ctime, array->c_ptr);
    bytesSend = send(client->sock_fd, buff, sizeof(buff), 0);
    return bytesSend;
}

int connect_socket(struct s_socket *cl_socket){
    int res = 0;
    if (connect(cl_socket->sock_fd, (struct sockaddr*)&cl_socket->addr, sizeof(cl_socket->addr)) == 0){
        if ((cl_socket->s_ip = inet_ntoa(cl_socket->addr.sin_addr)) == NULL)
             perror(EMSG);
        res = 1;
    }else {
        perror(EMSG);
    }
    return res;
}

