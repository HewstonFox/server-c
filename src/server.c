//
// Created by Hewston on 6/4/2022.
//

#include "server.h"

struct Server_s {
    bool is_running;
    int socket;
    pthread_t thread;
    struct sockaddr_in address;
    struct timeval req_timeout;
    struct timeval res_timeout;

    int port;
    void *ctx;

    Hooks hooks;
};

struct AcceptContext_s {
    Server server;
    int socket;
};

typedef struct AcceptContext_s *AcceptContext;

static void *server_accept(AcceptContext ctx) {

    free(ctx);
    return NULL;
}


static void *server_listen_cb(Server s) {
    int address_length = sizeof(s->address);
    while (s->is_running) {
        int new_client = accept(s->socket, (struct sockaddr *) &s->address, (socklen_t *) &address_length);

        if (!s->is_running)
            break;

        if (new_client < 0) {
            if (errno == ECONNABORTED)
                break;
            else if (
#if __linux__
errno == ENONET ||
#endif
errno == EPROTO ||
errno == ENOPROTOOPT ||
errno == ENETDOWN ||
errno == EHOSTUNREACH ||
errno == EOPNOTSUPP ||
errno == ENETUNREACH ||
errno == EHOSTDOWN ||
errno == EWOULDBLOCK ||
errno == EAGAIN
                    )
                continue;
            else {
                perror("Failure while listening...");
                server_stop(s);
                pthread_exit(NULL);
            }
        }

        AcceptContext ctx = (AcceptContext) malloc(sizeof(struct AcceptContext_s));
        ctx->server = s;
        ctx->socket = new_client;
        pthread_t response_thread;
        pthread_create(&response_thread, NULL, (void *(*)(void *)) server_accept, (void *) ctx);
    }

    pthread_exit(NULL);
}

static void server_normalize(Server s) {
    s->is_running = false;
    s->socket = 0;
    s->thread = PTHREAD_INHERIT_SCHED;
    s->port = 0;
    s->address.sin_family = PF_INET;
    s->address.sin_addr.s_addr = INADDR_ANY;
    s->address.sin_port = htons(s->port);
}


Server create_server(void *ctx) {
    Server s = (Server) malloc(sizeof(struct Server_s));

    server_normalize(s);

    s->hooks = create_hooks();
    s->ctx = ctx;
    s->req_timeout.tv_sec = 0;
    s->req_timeout.tv_usec = 0;
    s->res_timeout.tv_sec = 0;
    s->res_timeout.tv_usec = 0;

    return s;
}

void destroy_server(Server s) {
    server_stop(s);
    destroy_hooks(s->hooks);
    free(s);
}

Server server_set_context(Server s, void *ctx) {
    s->ctx = ctx;
    return s;
}

Server server_set_request_timeout(Server s, struct timeval timeout) {
    s->req_timeout = timeout;
    return s;
}

Server server_set_response_timeout(Server s, struct timeval timeout) {
    s->res_timeout = timeout;
    return s;
}

Server server_clear_hooks(Server s) {
    destroy_hooks(s->hooks);
    s->hooks = create_hooks();
    return s;
}

int server_listen(Server s, int port, void (*cb)(Server), bool detach) {
    s->socket = socket(PF_INET, SOCK_STREAM, 0);
    if (s->socket <= 0) {
        perror("Failed to connect socket...\n");
        return 1;
    }
    int reuse = 1;
    if (setsockopt(s->socket, SOL_SOCKET, SO_REUSEADDR, (const char *) &reuse, sizeof(reuse)) < 0) {
        perror("Failed to reuse address...\n");
        return 1;
    }
    if (setsockopt(s->socket, SOL_SOCKET, SO_RCVTIMEO, (const char *) &s->req_timeout, sizeof(s->req_timeout)) < 0) {
        perror("Failed to set request timeout...\n");
        return 1;
    }
    if (setsockopt(s->socket, SOL_SOCKET, SO_SNDTIMEO, (const char *) &s->res_timeout, sizeof(s->res_timeout)) < 0) {
        perror("Failed to set send timeout...\n");
        return 1;
    }
    int alive = 1;
    if (setsockopt(s->socket, SOL_SOCKET, SO_KEEPALIVE, (const char *) &alive, sizeof(alive))) {
        perror("Failed to set keep alive...\n");
        return 1;
    }

    s->port = port;
    s->address.sin_port = htons(s->port);
    if (bind(s->socket, (struct sockaddr *) &s->address, sizeof(s->address)) < 0) {
        perror("Failed to bind socket...\n");
        return 1;
    }
    if (listen(s->socket, SOMAXCONN) < 0) {
        perror("Failed to start listening...\n");
        return 1;
    }
    signal(SIGPIPE, SIG_IGN);

    s->is_running = true;

    pthread_t cb_thread;
    pthread_create(&cb_thread, NULL, (void *(*)(void *)) cb, (void *) s);

    if (detach)
        pthread_create(&s->thread, NULL, (void *(*)(void *)) server_listen_cb, (void *) s);
    else
        server_listen_cb(s);

    return 0;
}

int server_stop(Server s) {
    pthread_cancel(s->thread);
    close(s->socket);
    server_normalize(s);
    return 0;
}
