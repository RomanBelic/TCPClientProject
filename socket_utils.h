#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include "socket_structs.c"
#include "array_structs.c"

struct s_socket init_client_socket(const char *adress, int port_number);
int send_formatted_data(struct s_socket *client, struct c_array *array);    
int connect_socket(struct s_socket *cl_socket);
int listen_for_input(struct s_socket *cl_socket, struct c_array *in_array);
int reinit_cl_socket(struct s_socket *cl_socket, const char *adress, int port_number);

#endif /* SOCKET_UTILS_H */