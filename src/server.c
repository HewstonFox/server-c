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

    RequestWriter writer;
    RequestReader reader;
    RequestAcceptor acceptor;
    RequestCleaner cleaner;

    int port;
    void *ctx;

    Hooks hooks;

    char stamp[21];
};


static void server_normalize(Server s);


static Dict running_servers = NULL;
static pthread_mutex_t running_servers_mutex;

static void register_server(Server s) {
    pthread_mutex_lock(&running_servers_mutex);
    if (running_servers == NULL) running_servers = dict_create();
    dict_set(running_servers, s->stamp, (void *) s);
    pthread_mutex_unlock(&running_servers_mutex);
}

static void unregister_server(const char *key) {
    pthread_mutex_lock(&running_servers_mutex);
    if (key != NULL && running_servers != NULL) dict_remove(running_servers, key, NULL);
    pthread_mutex_unlock(&running_servers_mutex);
}

static size_t count_servers() {
    pthread_mutex_lock(&running_servers_mutex);
    size_t count = running_servers != NULL ? dict_count_entries(running_servers) : 0;
    if (count == 0 && running_servers != NULL) dict_destroy(running_servers, NULL);
    pthread_mutex_unlock(&running_servers_mutex);
    return count;
}

static void server_force_stop(Server s) {
    pthread_cancel(s->thread);
    close(s->socket);
    server_normalize(s);
}

static void abort_handler(int s) {
    if (s == SIGINT)
        printf("\n\nKeyboard interrupt...\n");
    if (s == SIGTERM)
        printf("\n\nTermination request...\n");

    printf("Stopping all servers...\n\n");
    pthread_mutex_lock(&running_servers_mutex);
    if (running_servers != NULL) {
        DictIterator di = dict_iterator_create(running_servers);
        while (dict_iterator_next(di)) {
            server_force_stop((Server) di->value);
            printf("Server '%s' stopped.\n", di->key);
        }
        dict_iterator_destroy(di);
        dict_destroy(running_servers, NULL);
        running_servers = NULL;
    }
    pthread_mutex_unlock(&running_servers_mutex);
    printf("All servers stopped.\n\n");
}

static char *read_until(char *str, AcceptContext ctx) {
    size_t str_len = sc_strlen(str);
    size_t cap = 30;
    char *buffer = calloc(cap, sizeof(char));
    size_t actual_size = 0;
    actual_size += ctx->server->reader(buffer, str_len, ctx);
    if (actual_size < str_len) {
        free(buffer);
        return NULL;
    }
    while (sc_strncmp(buffer + actual_size - str_len, str, str_len) != 0) {
        ssize_t read_count = ctx->server->reader(buffer + actual_size, 1, ctx);
        if (read_count <= 0)
            break;
        actual_size += read_count;
        if (actual_size == cap) {
            cap *= 2;
            buffer = sc_realloc(buffer, cap);
        }
    }
    buffer[actual_size] = 0;
    buffer = sc_realloc(buffer, actual_size);
    return buffer;
}

static Request request_create() {
    Request req = malloc(sizeof(struct Request_s));
    req->raw_hat = NULL;
    req->raw_route = NULL;
    req->raw_query = NULL;
    req->raw_headers = NULL;
    req->raw_body = NULL;

    req->version = NULL;
    req->path = NULL;

    req->body_len = 0;

    req->params = dict_create();
    req->query = dict_create();
    req->headers = dict_create();

    return req;
}

static Response response_create() {
    Response res = malloc(sizeof(struct Response_s));

    res->headers = dict_create();
    res->body = NULL;
    res->body_len = 0;

    return res;
}

static void *server_accept(AcceptContext ctx) {
    if (ctx->server->acceptor)
        ctx->server->acceptor(ctx);

    ctx->req = request_create();
    ctx->res = response_create();

    ctx->req->raw_hat = read_until("\r\n", ctx);
    ctx->req->raw_headers = read_until("\r\n\r\n", ctx);

    printf("accepted\n");

    if (ctx->server->cleaner)
        ctx->server->cleaner(ctx);
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
        ctx->data = NULL;
        ctx->req = NULL;
        ctx->res = NULL;
        pthread_t response_thread;
        pthread_create(&response_thread, NULL, (void *(*)(void *)) server_accept, (void *) ctx);
    }

    pthread_exit(NULL);
}

static void server_normalize(Server s) {
    s->is_running = false;
    s->socket = 0;
    s->thread = 0;
    s->port = 0;
    s->address.sin_family = PF_INET;
    s->address.sin_addr.s_addr = INADDR_ANY;
    s->address.sin_port = htons(s->port);
}


Server server_create(void *ctx) {
    Server s = (Server) malloc(sizeof(struct Server_s));

    server_normalize(s);

    sprintf(s->stamp, "%ld", sc_get_current_time());

    s->hooks = create_hooks();
    s->ctx = ctx;
    s->req_timeout.tv_sec = 0;
    s->req_timeout.tv_usec = 0;
    s->res_timeout.tv_sec = 0;
    s->res_timeout.tv_usec = 0;
    s->writer = sc_default_writer;
    s->reader = sc_default_reader;
    s->acceptor = NULL;
    s->cleaner = NULL;

    return s;
}

void server_destroy(Server s) {
    if (s == NULL) return;

    server_stop(s);
    destroy_hooks(s->hooks);
    free(s);
}

Server server_set_context(Server s, void *ctx) {
    if (s == NULL) return NULL;

    s->ctx = ctx;
    return s;
}

int server_get_port(Server s) {
    return s->port;
}

const char *server_get_stamp(Server s) {
    return s->stamp;
}

Server server_set_request_timeout(Server s, struct timeval timeout) {
    if (s == NULL) return NULL;

    s->req_timeout = timeout;
    return s;
}

Server server_set_response_timeout(Server s, struct timeval timeout) {
    if (s == NULL) return NULL;

    s->res_timeout = timeout;
    return s;
}

Server server_set_acceptor(Server s, RequestAcceptor acceptor) {
    s->acceptor = acceptor;
    return s;
}

Server server_set_cleaner(Server s, RequestCleaner cleaner) {
    s->cleaner = cleaner;
    return s;
}

Server server_set_reader(Server s, RequestReader reader) {
    s->reader = reader;
    return s;
}

Server server_set_writer(Server s, RequestWriter writer) {
    s->writer = writer;
    return s;
}

Server server_clear_hooks(Server s) {
    if (s == NULL) return NULL;

    destroy_hooks(s->hooks);
    s->hooks = create_hooks();
    return s;
}

Server server_listen(Server s, int port, void *(*cb)(Server), bool detach) {
    if (s == NULL) return NULL;

    s->socket = socket(PF_INET, SOCK_STREAM, 0);
    if (s->socket <= 0) {
        perror("Failed to connect socket...\n");
        return NULL;
    }
    int reuse = 1;
    if (setsockopt(s->socket, SOL_SOCKET, SO_REUSEADDR, (const char *) &reuse, sizeof(reuse)) < 0) {
        perror("Failed to reuse address...\n");
        return NULL;
    }
    if (setsockopt(s->socket, SOL_SOCKET, SO_RCVTIMEO, (const char *) &s->req_timeout, sizeof(s->req_timeout)) < 0) {
        perror("Failed to set request timeout...\n");
        return NULL;
    }
    if (setsockopt(s->socket, SOL_SOCKET, SO_SNDTIMEO, (const char *) &s->res_timeout, sizeof(s->res_timeout)) < 0) {
        perror("Failed to set send timeout...\n");
        return NULL;
    }
    int alive = 1;
    if (setsockopt(s->socket, SOL_SOCKET, SO_KEEPALIVE, (const char *) &alive, sizeof(alive))) {
        perror("Failed to set keep alive...\n");
        return NULL;
    }
    s->port = port;
    s->address.sin_port = htons(s->port);
    if (bind(s->socket, (struct sockaddr *) &s->address, sizeof(s->address)) < 0) {
        perror("Failed to bind socket...\n");
        return NULL;
    }
    if (listen(s->socket, SOMAXCONN) < 0) {
        perror("Failed to start listening...\n");
        return NULL;
    }

    signal(SIGPIPE, SIG_IGN);

    signal(SIGINT, abort_handler);
    signal(SIGTERM, abort_handler);

    s->is_running = true;
    register_server(s);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t cb_thread;
    pthread_create(&cb_thread, &attr, (void *(*)(void *)) cb, (void *) s);

    if (detach)
        pthread_create(&s->thread, &attr, (void *(*)(void *)) server_listen_cb, (void *) s);
    else
        server_listen_cb(s);

    pthread_attr_destroy(&attr);

    return s;
}

int server_stop(Server s) {
    pthread_cancel(s->thread);
    close(s->socket);
    server_normalize(s);
    unregister_server(s->stamp);
    return 0;
}

void wait_for_servers_stop() {
    struct timespec t = {0, 100000000};
    while (count_servers() != 0)
        nanosleep(&t, &t);
}
