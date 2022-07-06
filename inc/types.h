//
// Created by Hewston Fox on 06.07.2022.
//

#pragma once

typedef struct Server_s *Server;

struct AcceptContext_s {
    Server server;
    int socket;
    void *data;
};

typedef struct AcceptContext_s *AcceptContext;

typedef ssize_t (*RequestWriter)(const void *buf, size_t n, AcceptContext ctx);

typedef ssize_t (*RequestReader)(void *buff, size_t n, AcceptContext ctx);

typedef void (*RequestAcceptor)(AcceptContext ctx);

typedef void (*RequestCleaner)(AcceptContext ctx);
