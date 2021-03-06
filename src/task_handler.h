/*
    Copyright (c) 2013, Peter Johansson <peter.johansson@gmx.com>

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

#ifndef _SPEEDY_TASK_HANDLER_H_
#define _SPEEDY_TASK_HANDLER_H_

#define TASK_HANDLER_SUCCESS 0
#define TASK_HANDLER_FAIL -1

struct queue_t;
struct thread_pool_t;
struct hash_lookup_t;
struct service_t;

typedef struct task_handler_t {
    struct hash_lookup_t *task_lookup;
    struct queue_t *tasks; /*!< Queue with all the tasks. */
    struct thread_pool_t *thread_pool;
} task_handler_t;

task_handler_t * task_handler_create(void);
int task_handler_init(task_handler_t *this_ptr);

int task_handler_add_task(task_handler_t * this_ptr,struct service_t *service);
int task_handler_add_tasks(task_handler_t *this_ptr,
                        struct service_t *services, unsigned int services_size);
int task_handler_calculate_dependency(task_handler_t * this_ptr);
int task_handler_wait(task_handler_t * this_ptr);

struct task_t *task_handler_thread_pool_pop(task_handler_t *this_ptr);
void task_handler_run_add_task(task_handler_t *this_ptr, struct task_t *task);

void task_handler_deinit(task_handler_t * this_ptr);
void task_handler_destroy(task_handler_t * this_ptr);


#endif /* _SPEEDY_TASK_HANDLER_H_ */
