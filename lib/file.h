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

#include <stdbool.h>

bool libspeedy_file_exists(const char *filename);

bool libspeedy_file_copy(const char *source, const char *destination);

bool libspeedy_file_copy_all(const char *source_dir, const char *dest_dir, bool recursive);

bool libspeedy_file_empty(const char *destination);

bool libspeedy_file_remove(const char *destination);

bool libspeedy_file_remove_all(const char *dest_path, bool recursive, bool remove_dir);

bool libspeedy_file_chmod(const char *pathname, int mode);

bool libspeedy_file_write(const char *filename, const char *text);

bool libspeedy_file_append(const char *filename, const char *text);

bool libspeedy_file_tty_action(bool (*callback)(char *filename));
