/*
    Copyright (c) 2010, Peter Johansson <zeronightfall@gmx.com>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "task_handler.h"
#include "hash_lookup.h"
#include "core_type.h"
#include "observer.h"
#include "queue.h"
#include "subject.h"
#include "task.h"

#include <stdlib.h>

task_handler_t * task_handler_create(void)
{
    task_handler_t *this_ptr = (task_handler_t*) malloc(sizeof(task_handler_t));

    if (this_ptr != NULL) {
        if (task_handler_init(this_ptr) != TASK_HANDLER_SUCCESS) {
            task_handler_deinit(this_ptr);
            free(this_ptr);
            this_ptr = NULL;
        }
    }
    return this_ptr;
}

int task_handler_init(task_handler_t * this_ptr)
{
    this_ptr->task_lookup = hash_lookup_create(64);
    this_ptr->tasks = queue_create();

    if ((this_ptr->tasks == NULL) || (this_ptr->tasks == NULL)) {
        task_handler_deinit(this_ptr);
        return TASK_HANDLER_FAIL;
    }

    return TASK_HANDLER_SUCCESS;
}

int task_handler_add_tasks(task_handler_t * this_ptr,
                           struct service_t *services)
{
    unsigned int services_size = sizeof(services) / sizeof(service_t);
    task_t *task;
    int tasks = 0;
    int i;

    for (i = 0; i < services_size; i++) {
        task = task_create(&services[i]);

        if (task != NULL) {
            if (queue_push(this_ptr->tasks, task) != QUEUE_ERROR) {
                tasks++;
            } else {
                task_destroy(task);
            }
        }
    }
    if (tasks != services) {
        return TASK_HANDLER_FAIL;
    }
    return TASK_HANDLER_SUCCESS;
}

void task_handler_deinit(task_handler_t * this_ptr)
{
    task_t *task;

    if (this_ptr->tasks != NULL) {
        while((task = queue_pop(this_ptr->tasks)) != NULL) {
            task_destroy(task);
        }
        queue_destroy(this_ptr->tasks);
    }

    hash_lookup_destroy(this_ptr->task_lookup);
}

void task_handler_destroy(task_handler_t * this_ptr)
{
    task_handler_deinit(this_ptr);
    free(this_ptr);
}
