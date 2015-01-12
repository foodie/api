#ifndef __API_SOCKET_H_INCLUDED__
#define __API_SOCKET_H_INCLUDED__

#include <api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int socket_t;

API int api_tcp_nopush(socket_t s);
API int api_tcp_push(socket_t s);

#ifdef __cplusplus
}
#endif

#endif /*! __API_SOCKET_H_INCLUDED__ */