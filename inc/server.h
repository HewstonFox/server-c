//
// Created by Hewston on 6/4/2022.
//

#pragma once

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include "../deps/dict-c/inc/dict.h"
#include "hooks.h"
#include "utils/utils.h"

typedef struct Server_s *Server;

struct AcceptContext_s {
    Server server;
    int socket;
    void *data;
};

typedef struct AcceptContext_s *AcceptContext;

Server server_create(void *ctx);

void server_destroy(Server s);


Server server_set_context(Server s, void *ctx);

Server server_set_request_timeout(Server s, struct timeval timeout);

Server server_set_response_timeout(Server s, struct timeval timeout);

Server server_clear_hooks(Server s);


int server_get_port(Server s);

const char *server_get_stamp(Server s);


Server server_listen(Server s, int port, void *(*cb)(Server), bool detach);


int server_stop(Server s);

void wait_for_servers_stop();
