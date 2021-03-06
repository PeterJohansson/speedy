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

#include "observer.h"
#include "queue.h"
#include "subject.h"

#include <stdlib.h>

/*!
 * Creates and initializes a subject.
 *
 * \return A pointer to the created subject.
 */
subject_t *subject_create(void)
{
    subject_t *this_ptr = (subject_t*) malloc(sizeof(subject_t));
    subject_init(this_ptr);
    return this_ptr;
}

/*!
 * Initializes a subject.
 *
 * \param this_ptr - A pointer to the subject.
 */
void subject_init(subject_t *this_ptr)
{
    if (this_ptr != NULL) {
        queue_init(&this_ptr->queue);
        observer_init(&this_ptr->observer, NULL);
    }
}

/*!
 * Attached an observer to a subject.
 *
 * \param this_ptr - A pointer to the subject.
 * \param observer - A pointer to the observer that is attached to the subject.
 *
 * \return \c SUBJECT_SUCESS if it was possible to attach the observer,
 * \return \c SUBJECT_ERROR if it wasn't possible to push the observer on to
 *                          the queue.
 * \return \c SUBJECT_MISSING_OBSERVER if the observer is \c NULL.
 * \return \c SUBJECT_NULL if the subject is \c NULL.
 */
int subject_attach(subject_t *this_ptr, observer_t *observer)
{
    int status = SUBJECT_SUCESS;

    if (this_ptr != NULL) {
        if (observer != NULL) {
            if (queue_push(&this_ptr->queue,
                           (void*) observer) != QUEUE_SUCESS) {

                status = SUBJECT_ERROR;
            }
        } else {
            status = SUBJECT_MISSING_OBSERVER;
        }
    } else {
        status = SUBJECT_NULL;
    }

    return status;
}

/*!
 * Detach an observer from a subject.
 *
 * \param this_ptr - A pointer to the subject.
 * \param observer - A pointer to the observer that is detached from
 *                   the subject.
 *
 * \return \c SUBJECT_SUCESS if it was possible to detach the observer,
 * \return \c SUBJECT_ERROR if it wasn't possible to remove the observer from
 *                          the queue.
 * \return \c SUBJECT_MISSING_OBSERVER if the observer is \c NULL.
 * \return \c SUBJECT_NULL if the subject is \c NULL.
 */
int subject_detach(subject_t *this_ptr, observer_t *observer)
{
    int status = SUBJECT_SUCESS;
    observer_t *current;

    if (this_ptr != NULL) {
        if (observer != NULL) {

            queue_first(&this_ptr->queue);
            while((current = queue_get_current(&this_ptr->queue)) != NULL) {
                if (current == observer) {
                    if (queue_remove_current(
                        &this_ptr->queue) != QUEUE_SUCESS) {

                        status = SUBJECT_ERROR;
                    }
                }
                queue_next(&this_ptr->queue);
            }

        } else {
            status = SUBJECT_MISSING_OBSERVER;
        }
    } else {
        status = SUBJECT_NULL;
    }
    return status;
}

/*!
 * Function which is used for notify all the observers.
 *
 * \param this_ptr - A pointer to the subject.
 * \param msg - A pointer to the message that is sent to the observers.
 */
int subject_notify(subject_t *this_ptr, void *msg)
{
    int status = SUBJECT_SUCESS;

    if (this_ptr != NULL) {
        observer_t *current;

        queue_first(&this_ptr->queue);
        while((current = queue_get_current(&this_ptr->queue)) != NULL) {
            if (observer_notify(current, this_ptr, msg) != OBSERVER_SUCESS) {
                status = SUBJECT_MISSING_OBSERVER;
            }
            queue_next(&this_ptr->queue);
        }

    } else {
        status = SUBJECT_NULL;
    }
    return status;
}

/*!
 * Deinitializes a subject.
 *
 * \param this_ptr - A pointer to the subject.
 */
void subject_deinit(subject_t *this_ptr)
{
    if (this_ptr != NULL) {
        queue_deinit(&this_ptr->queue);
        observer_deinit(&this_ptr->observer);
    }
}

/*!
 * Deletes a subject.
 *
 * \param this_ptr - A pointer to the subject.
 */
void subject_destroy(subject_t *this_ptr)
{
    subject_deinit(this_ptr);
    free(this_ptr);
}
