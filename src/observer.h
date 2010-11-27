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

struct subject_t;

/*!
 * A type definition of an observer object which is used to keep track of the
 * internal state of a specific observer.
 */
typedef struct observer_t {
    /*! A mutex which makes sure that only one subject is dealing with an
     *  observer. */
    pthread_mutex_t mutex;
    void (*notify)(struct observer_t *this_ptr, struct subject_t *from,
                   void *msg);
} observer_t;

observer_t * observer_create(
       void (*notify)(observer_t *this_ptr, struct subject_t *from, void *msg));
void observer_init(observer_t *this_ptr,
       void (*notify)(observer_t *this_ptr, struct subject_t *from, void *msg));

void observer_notify(observer_t *this_ptr, struct subject_t *from, void *msg);
void observer_set_notify(observer_t *this_ptr,
     void (*notify)(observer_t *this_ptr, struct subject_t *from, void *msg));

void observer_deinit(observer_t *this_ptr);
void observer_destroy(observer_t *this_ptr);