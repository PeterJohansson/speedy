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

#include "queue.h"
#include <stdlib.h>

/*! A structure for storing each node in the queue. */
typedef struct node_t {
    data_t *data; /*!< A pointer to the data that is referenced from the node.*/
    struct node_t *next; /*!< The next node in the queue. */
    struct node_t *previous; /*!< The previous node in the queue. */
} node_t;

/*!
 * Creates and initializes a queue.
 *
 * \return The created queue.
 */
queue_t * queue_create(void)
{
    queue_t * this_ptr = (queue_t*) malloc(sizeof(queue_t));
    queue_init(this_ptr);
    return this_ptr;
}

/*!
 * Initializes a queue.
 *
 * \param this_ptr - A pointer to the queue
 */
void queue_init(queue_t *this_ptr)
{
    if (this_ptr != NULL) {
        this_ptr->first = NULL;
        this_ptr->last = NULL;
        this_ptr->iterator.current = NULL;
    }
}
/*!
 * Gets the next data item from the queue.
 *
 * \return A pointer to the data item which has been fetched from the queue,
 *         if it wasn't possible to pop from the queue then NULL is returned.
 */
data_t * queue_pop(queue_t *this_ptr)
{
    node_t *node;
    data_t *data = NULL;

    if (this_ptr != NULL) {
        this_ptr->iterator.current = NULL;

        if (this_ptr->first != NULL) {
            node = this_ptr->first;
            this_ptr->first->previous = NULL;
            this_ptr->first = node->next;

            /* In case it was the last node. */
            if (this_ptr->first == NULL) {
                this_ptr->last = NULL;
            }

            data = node->data;
            free(node);
        }
    }
    return data;
}

/*!
 *  Put the data item at the end of the queue.
 *
 * \param this_ptr - A pointer to the queue
 * \param data - A pointer to the data that is put on the queue.
 *
 * \return \c QUEUE_SUCESS if it was possible to push the node on to the queue.
 * \return \c QUEUE_ERROR if it wasn't possible to allocate memory.
 * \return \c QUEUE_NULL if the queue doesn't exist.
 */
int queue_push(queue_t *this_ptr, data_t* data)
{
    int status = QUEUE_ERROR;

    if (this_ptr != NULL) {
        node_t * node = (node_t*) malloc(sizeof(node_t));

        this_ptr->iterator.current = NULL;

        if (node != NULL) {
            node->data = data;
            node->next = NULL;

            if (this_ptr->last != NULL) {
                node->previous = this_ptr->last;
                this_ptr->last->next = node;
                this_ptr->last = node;
            } else {
                /* The queue was empty so make the node both the first and
                   the last. */
                node->previous = NULL;
                this_ptr->first = node;
                this_ptr->last = node;
            }
            status = QUEUE_SUCESS;
        }

    } else {
        status = QUEUE_NULL;
    }

    return status;
}

/*!
 *  Set the internal iterator to the first item in the queue.
 *
 * \param this_ptr - A pointer to the queue
 *
 * \return \c QUEUE_SUCESS if it was possible to find the last node.
 * \return \c QUEUE_EMPTY if it was not possible since the queue is empty.
 * \return \c QUEUE_NULL if the queue doesn't exist.
 */
int queue_first(queue_t *this_ptr)
{
    int status = QUEUE_EMPTY;

    if (this_ptr != NULL) {
        this_ptr->iterator.current = NULL;
        this_ptr->iterator.direction_next = true;

        if (this_ptr->first != NULL) {
            this_ptr->iterator.current = this_ptr->first;
            status = QUEUE_SUCESS;
        }

    } else {
        status = QUEUE_NULL;
    }

    return status;
}

/*!
 *  Set the internal iterator to the last item in the queue.
 *
 * \param this_ptr - A pointer to the queue
 *
 * \return \c QUEUE_SUCESS if it was possible to find the last node.
 * \return \c QUEUE_EMPTY if it was not possible since the queue is empty.
 * \return \c QUEUE_NULL if the queue doesn't exist.
 */
int queue_last(queue_t *this_ptr)
{
    int status = QUEUE_EMPTY;

    if (this_ptr != NULL) {
        this_ptr->iterator.current = NULL;
        this_ptr->iterator.direction_next = false;

        if (this_ptr->last != NULL) {
            this_ptr->iterator.current = this_ptr->last;
            status = QUEUE_SUCESS;
        }

    } else {
        status = QUEUE_NULL;
    }

    return status;
}

/*!
 *  Set the internal iterator to the next item in the queue.
 *
 * \param this_ptr - A pointer to the queue
 *
 * \return \c QUEUE_SUCESS if it was possible to move the iterator to the
 *                         next node.
 * \return \c QUEUE_LAST when the last node was reached and there was no
 *                       next node.
 * \return \c QUEUE_NULL if the queue doesn't exist.
 */
int queue_next(queue_t *this_ptr)
{
    int status = QUEUE_LAST;

    if (this_ptr != NULL) {
        this_ptr->iterator.direction_next = true;

        if (this_ptr->iterator.current != NULL) {
            this_ptr->iterator.current = this_ptr->iterator.current->next;
            status = QUEUE_SUCESS;
        }

    } else {
        status = QUEUE_NULL;
    }

    return status;
}

/*!
 *  Set the internal iterator to the previous item in the queue.
 *
 * \param this_ptr - A pointer to the queue
 *
 * \return \c QUEUE_SUCESS if it was possible to move the iterator to the
 *                         previous node.
 * \return \c QUEUE_LAST when the first node was reached and there was no
 *                       previous node.
 * \return \c QUEUE_NULL if the queue doesn't exist.
 */
int queue_previous(queue_t *this_ptr)
{
    int status = QUEUE_LAST;

    if (this_ptr != NULL) {
        this_ptr->iterator.direction_next = false;

        if (this_ptr->iterator.current != NULL) {
            this_ptr->iterator.current = this_ptr->iterator.current->previous;
            status = QUEUE_SUCESS;
        }

    } else {
        status = QUEUE_NULL;
    }


    return status;
}

/*!
 *  Get the data at the current position by using the internal iterator.
 *
 * \param this_ptr - A pointer to the queue
 *
 * \return The data from the current position in the queue. If the internal
 *         iterator hasn't been moved NULL is returned.
 */
data_t * queue_get_current(queue_t * this_ptr)
{
    data_t * data = NULL;

    if (this_ptr != NULL && this_ptr->iterator.current != NULL) {
        data = this_ptr->iterator.current->data;
    }
    return data;
}

/*!
 *  Set the internal iterator to the next item in the queue.
 *
 * \param this_ptr - A pointer to the queue
 *
 * \return \c QUEUE_SUCESS if it was possible to remove the current node.
 * \return \c QUEUE_NULL if the queue doesn't exist.
 */
int queue_remove_current(queue_t *this_ptr)
{
    int status = QUEUE_EMPTY;
    node_t *node;

    if (this_ptr != NULL) {
        if (this_ptr->iterator.current != NULL) {
            node = this_ptr->iterator.current;

            if ((node->previous != NULL) && (node->next != NULL)) {
                /* The current item is somewhere in the middle of the queue. */
                node->previous->next = node->next;
                node->next->previous = node->previous;

            } else if ((node == this_ptr->first) && (node != this_ptr->last)) {
                /* The current item is the first in the queue and the queue size
                 * is greater or equal to 2 items. */
                this_ptr->first = node->next;
                node->next->previous = NULL;

            } else if ((node != this_ptr->first) && (node == this_ptr->last)) {
                /* The current item is the last in the queue and the queue size
                 * is greater or equal to 2 items. */
                this_ptr->last = node->previous;
                node->previous->next = NULL;

            } else {
                /* It was the last item in the queue. */
                this_ptr->first = NULL;
                this_ptr->last = NULL;
            }

            /* Depending on the last direction command, set the current position
             * to the previous position. */
            if (this_ptr->iterator.direction_next) {
                this_ptr->iterator.current = node->previous;
            } else {
                this_ptr->iterator.current = node->next;
            }
            free(node);
            status = QUEUE_SUCESS;
        }

    } else {
        status = QUEUE_NULL;
    }

    return status;
}

/*!
 *  Deinitializes the queue.
 *
 * \param this_ptr - A pointer to the queue
 */
void queue_deinit(queue_t *this_ptr)
{
    while(queue_pop(this_ptr) != NULL) {
    }
}

/*!
 *  Remove the queue.
 *
 * \param this_ptr - A pointer to the queue
 */
void queue_destroy(queue_t *this_ptr)
{
    queue_deinit(this_ptr);
    free(this_ptr);
}

