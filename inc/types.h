//
// Created by Hewston Fox on 06.07.2022.
//

#pragma once

#include "../deps/dict-c/inc/dict.h"

typedef struct Server_s *Server;

struct Request_s {
    char *raw_hat;
    char *raw_route;
    char *raw_query;
    char *raw_headers;
    void *raw_body;

    char *version;
    char *path;

    size_t body_len;

    Dict params;
    Dict query;
    Dict headers;
};

typedef struct Request_s *Request;


struct Response_s {
    Dict headers;
    void *body;
    size_t body_len;
};

typedef struct Response_s *Response;


struct AcceptContext_s {
    Server server;
    int socket;
    void *data;
    Request req;
    Response res;
};

typedef struct AcceptContext_s *AcceptContext;


typedef ssize_t (*RequestWriter)(const void *buf, size_t n, AcceptContext ctx);

typedef ssize_t (*RequestReader)(void *buff, size_t n, AcceptContext ctx);

typedef void (*RequestAcceptor)(AcceptContext ctx);

typedef void (*RequestCleaner)(AcceptContext ctx);
