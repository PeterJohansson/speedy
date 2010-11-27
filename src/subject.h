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

#include <pthread.h>

struct observer_t;
struct queue_t;

/*! The operation was successfully executed. */
#define SUBJECT_SUCESS 0
/*! General error which mostly likely happens during malloc. */
#define SUBJECT_ERROR -1

/*!
 * \note The current implementation of the subject doesn't support multiple
 *       attach/detach from different observers at the same time.
 */
typedef struct subject_t {
    /*! C inheritance of an observer. This makes it possible to use a subject
     *  as an observer for a different subject. */
    struct observer_t observer;
    /*! A queue which contains all the observers. */
    struct queue_t queue;
} subject_t;

subject_t *subject_create(void);
void subject_init(subject_t *this_ptr);

int subject_attach(subject_t *this_ptr, struct observer_t *observer);
int subject_detach(subject_t *this_ptr, struct observer_t *observer);

void subject_notify(subject_t *this_ptr, void *arg);

void subject_deinit(subject_t *this_ptr);
void subject_destroy(subject_t *this_ptr);