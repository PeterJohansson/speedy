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

/*! An error code if everything was successfully executed. */
#define TASK_SUCCESS 0
#define TASK_FAIL -1

struct service_t;
struct subject_t;

typedef struct task_t {
    /*! A C inheritance of the \c struct \c subject_t type which makes
     * it possible to use a task as both an observer and a subject. The point
     * of this is to actually be able to track dependencies, more or less to
     * know when a task can execute. */
    subject_t task;
    /*! An unique id for the task, this is used for tracking dependecies. */
    unsigned int task_id;
    /*! An alternative id for the task, this is also used for tracking
     * dependencies, especially dependencies that can be provided by different
     * tasks. */
    unsigned int provides_id;

    struct queue_t *dependency;
    /*! A pointer to the service struct which contains function pointers for
     *  getting the task name, the alternative name of the task, the
     *  dependencies, the initialization function and the shutdown function.
     *  This actually gives the task something to do during startup or shutdown
     *  and it also gives basic knowledge of the dependencies for the task. */
    service_t *service;
} task_t;

task_t * task_create(struct service_t *service);

int task_run_initialization(task_t *task);

unsigned int task_get_id(task_t *this_ptr);
unsigned int task_get_provides_id(task_t *this_ptr);

void task_destroy(task_t *task);
