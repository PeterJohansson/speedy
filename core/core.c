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

#include "core_type.h"
#include "config_sysinit.h"
#include "config_daemons.h"
#include "config_tests.h"

#include "hash_lookup.h"
#include "queue.h"
#include "task_handler.h"
#include "config_parser.h"

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    config_parser_t *config = config_parser_open("test2.txt");
    char * arg;

    while (!config_parser_is_eof(config)) {

        if (config_parser_read(config) == 0) {

            printf("%s=",config_parser_get_command(config));
            while ((arg = config_parser_get_next_argument(config)) != NULL) {
                printf("%s ",arg);
            }
            printf("\n");
        }
    }

    config_parser_close(config);
/*    unsigned int size = sizeof(tests) / sizeof(service_t);
    task_handler_t *task_handler = task_handler_create();

    task_handler_add_tasks(task_handler, tests, size);
    task_handler_calculate_dependency(task_handler);
    task_handler_wait(task_handler);
    task_handler_destroy(task_handler);*/
    return 0;
}
