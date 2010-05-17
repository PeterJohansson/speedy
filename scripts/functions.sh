#!/bin/bash

# Copyright (c) 2010, Peter Johansson <zeronightfall@gmx.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

if_file_exist()
{
    file_exist=$1
    shift
    action_true=$@
    
    if [ -f $file_exist ]; then
        exec_cmd $action_true
    fi
}

exec_cmd()
{
    cmd=$@
    
    if [ $# -gt 0 ]; then
        $cmd
    else
        return 1
    fi
}

get_md5sum()
{
    filename=$1
    md5sum=$(md5sum $filename 2> /dev/null)
    set -- $md5sum
    echo $1
}

update_file()
{
    old_file=$1
    new_file=$2
    remove_file=$3

    md5sum_new=$(get_md5sum $new_file)
    md5sum_old=$(get_md5sum $old_file)

    if [ "$md5sum_new" != "$md5sum_old" ]; then
        mv -f $old_file $new_file
        printf "Updating: %s\n" $new_file
        if [ $# -ge 3 ]; then 
            rm -f $remove_file
        fi
    fi
}