/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Romaaan
 *
 * Created on 12 mai 2017, 08:21
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h>
#include <sys/socket.h>
#include "mutils.h"
#include "process_utils.h"
#include "socket_utils.h"
#include "shared.h" /* POSIX terminal control definitions */

/*
 * 
 */
int main(int argc, char** argv) {
    struct termios term;
    struct termios term_orig;
    
    tcgetattr(STDIN_FILENO, &term_orig);
    memcpy(&term, &term_orig, sizeof(term_orig));
    term.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    
    struct c_array buffer = init_c_array(BUFF_SIZE);
    struct s_socket cl_socket = init_client_socket(SERVER_ADRESS, SERVER_PORT);
    
    connect_socket(&cl_socket);
    listen_for_input(&cl_socket, &buffer);

    tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
    shutdown(SHUT_RDWR, cl_socket.sock_fd);
    close(cl_socket.sock_fd);
    return (EXIT_SUCCESS);
}

