
#ifndef MUTILS_H
#define MUTILS_H

#include "socket_structs.c"
#include "array_structs.c"

char *getlocaltime(struct tm *timeinfo);
struct c_array init_c_array(int size);
int add_c_to_array(struct c_array *array, char c);
int clear_c_array(struct c_array *array);

#endif /* UTILS_H */