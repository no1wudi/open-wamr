/*
 * Copyright (C) 2023 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "type.h"

static dyn_ctx_t g_dynamic_context = NULL;

JSValue *
dyntype_dup_value(JSContext *ctx, JSValue value)
{
    JSValue *ptr = js_malloc(ctx, sizeof(value));

    if (!ptr) {
        return NULL;
    }
    memcpy(ptr, &value, sizeof(value));
    return ptr;
}

/******************* Initialization and destroy *****************/

dyn_ctx_t
dyntype_context_init()
{
    if (g_dynamic_context) {
        return g_dynamic_context;
    }

    dyn_ctx_t ctx = malloc(sizeof(DynTypeContext));
    if (!ctx) {
        return NULL;
    }
    memset(ctx, 0, sizeof(DynTypeContext));

    ctx->js_rt = JS_NewRuntime();
    if (!ctx->js_rt) {
        goto fail;
    }
    ctx->js_ctx = JS_NewContext(ctx->js_rt);
    if (!ctx->js_ctx) {
        goto fail;
    }

    ctx->js_undefined = dyntype_dup_value(ctx->js_ctx, JS_UNDEFINED);
    if (!ctx->js_undefined) {
        goto fail;
    }

    ctx->js_null = dyntype_dup_value(ctx->js_ctx, JS_NULL);
    if (!ctx->js_null) {
        goto fail;
    }

    g_dynamic_context = ctx;
    return ctx;

fail:
    dyntype_context_destroy(ctx);
    return NULL;
}

dyn_ctx_t
dyntype_context_init_with_opt(dyn_options_t *options)
{
    // TODO
    return NULL;
}

void
dyntype_context_destroy(dyn_ctx_t ctx)
{
    if (ctx) {
        if (ctx->js_undefined) {
            js_free(ctx->js_ctx, ctx->js_undefined);
        }
        if (ctx->js_null) {
            js_free(ctx->js_ctx, ctx->js_null);
        }
        if (ctx->js_ctx) {
            JS_FreeContext(ctx->js_ctx);
        }
        if (ctx->js_rt) {
            JS_FreeRuntime(ctx->js_rt);
        }
        free(ctx);
    }

    g_dynamic_context = NULL;
}

void
dyntype_set_callback_dispatcher(dyn_ctx_t ctx,
                                dyntype_callback_dispatcher_t callback)
{
    ctx->cb_dispatcher = callback;
}

dyn_ctx_t
dyntype_get_context()
{
    return g_dynamic_context;
}
