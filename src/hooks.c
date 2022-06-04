//
// Created by Hewston on 6/4/2022.
//

#include "hooks.h"

struct Hooks_s {
    int _;
};

Hooks create_hooks() {
    Hooks e = (Hooks) malloc(sizeof(struct Hooks_s));

    return e;
}

void destroy_hooks(Hooks e) {
    free(e);
}
