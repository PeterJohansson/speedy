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

#include <stdio.h>

struct queue_t;

#define MAX_LENGTH 1024u
#define MAX_ARGUMENT 1024u
#define MAX_COMMAND 128u

typedef enum {
    NEW_LINE,
    SPACE,
    COMMENT,
    PRE_COMMAND,
    COMMAND,
    PRE_ARGUMENT,
    ARGUMENT,
    ADD_COMMAND,
    EXIT,
    PRE_ERROR,
    ERROR
} parser_mode_t;

typedef struct config_parser_t {
    char buffer[MAX_LENGTH];
    char command[MAX_COMMAND];
    char argument[MAX_ARGUMENT];
    char *buffer_pos_ptr;
    FILE* file;
    size_t buffer_pos;
    size_t bytes_read;
    unsigned int line;
    parser_mode_t mode;
    bool eof;
} config_parser_t;

config_parser_t *config_parser_open(char* filename);
void config_parser_close(config_parser_t *config);

bool config_parser_is_eof(config_parser_t *config);

void config_parser_read(config_parser_t *config);