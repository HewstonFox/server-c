//
// Created by Hewston Fox on 06.07.2022.
//

#pragma once
#include <unistd.h>
#include "types.h"

ssize_t sc_default_writer(const void *buf, size_t n, AcceptContext ctx);

ssize_t sc_default_reader(void *buff, size_t n, AcceptContext ctx);
