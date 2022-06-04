//
// Created by Hewston on 6/4/2022.
//

#pragma once

#include "../deps/dict-c/inc/dict.h"

#define ON_REQUEST "onRequest"
#define PRE_PARSING "preParsing"
#define PRE_VALIDATION "preValidation"
#define PRE_HANDLER "preHandler"
#define PRE_SERIALIZATION "preSerialization"
#define ON_ERROR "onError"
#define ON_SEND "onSend"
#define ON_RESPONSE "onResponse"
#define ON_TIMEOUT "onTimeout"

typedef struct Hooks_s *Hooks;

Hooks create_hooks();

void destroy_hooks(Hooks e);
