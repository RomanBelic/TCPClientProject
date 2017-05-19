/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#ifndef SHARED_H
#define SHARED_H

#define CountElems(x) (sizeof(x) / sizeof((x)[0]))

#define EMSG ("Error")

#define SERVER_PORT (1000)

#define SERVER_ADRESS ((const char *) ("127.0.0.1"))

#define NEWLINE ("\r\n")

#define INCOMING ((const char) 251)

#define ACK ((const char) 250)

#define FIN_SHD ((const char)253)      //instant EOT

#define ACK_INCOMING ((const char) 254)

#define ACK_FIN ((const char) 255)

#define BUFF_SIZE (8)

#endif /* SHARED_H */



