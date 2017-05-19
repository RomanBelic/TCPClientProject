/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <sys/_default_fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shared.h"
#include <sys/socket.h>
#include <string.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <stdlib.h>
#include <sys/types.h>
#include "termios.h"
#include "mutils.h"
#include "socket_utils.h"

char *getlocaltime(struct tm *timeinfo){
    time_t rawtime;
    time (&rawtime); 
    timeinfo = localtime (&rawtime);
    return asctime (timeinfo);
}

int add_c_to_array(struct c_array *array, char c){
    int res = 0;
    for(int i = array->fill_size; i < array->size; i++){
        if (array->c_ptr[i] == 0){
            array->c_ptr[i] = c;
            array->fill_size ++;
            res = 1;
            break;
        }
    }
    return res;
}

struct c_array init_c_array(int size){
    struct c_array array;
    array.fill_size = 0;
    array.size = size;
    array.c_ptr = calloc(size, sizeof(char));
    return array;
}

int clear_c_array(struct c_array *array){
    memset(array->c_ptr, 0, array->size * sizeof(char));
    array->fill_size = 0;
    return (array->c_ptr[0] == 0) ? 1 : 0;
}
