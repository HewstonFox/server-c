//
// Created by Hewston Fox on 06.07.2022.
//

#include "defaults.h"

ssize_t sc_default_writer(const void *buf, size_t n, AcceptContext ctx) {
    const char *p = buf;
    while (n) {
        ssize_t ret = write(ctx->socket, p, n);
        if (ret < 0)
            return -1;
        p += ret;
        n -= ret;
    }
    return 0;
}

ssize_t sc_default_reader(void *buff, size_t n, AcceptContext ctx) {
    return read(ctx->socket, buff, n);
}
