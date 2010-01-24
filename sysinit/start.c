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

#include "start.h"
#include "config/start.h"
#include <string.h>
#include <stdlib.h>

#ifdef SIMULATE
#include "lib/simulate.h"
#else
#include "lib/run.h"
#include <sys/mount.h> 
#endif

const char *start_get_name(void)
{
    static const char priv_start_name[] = "start";
    
    return priv_start_name;
}

void start_init(void)
{    
    char *const minilogd_arg[] = {"/sbin/minilogd", NULL};
    char *const dmesg_arg[] = {"/bin/dmesg", "-n", "3", NULL};
    char *const mknod_arg[] = {"/bin/mknod", "/dev/rtc0", "c", RTC_MAJOR, "0", NULL};
    char *const ln_arg[] = {"/bin/ln", "-s", "/dev/rtc0", "/dev/rtc", NULL};
    
    mount("udev", "/dev", "tmpfs", MS_NOSUID, "mode=0755,size=10M");
    mount("none", "/proc", "proc", 0, NULL);
    mount("none", "/sys", "sysfs", 0, NULL);

    /* Copy static device nodes to /dev */
    system("/bin/cp -a /lib/udev/devices/* /dev/");

    /* start up mini logger until syslog takes over */
    run("/sbin/minilogd", minilogd_arg);

    run("/bin/dmesg", dmesg_arg);

    system("/sbin/modprobe rtc-cmos >/dev/null 2>&1");
    run("/bin/mknod", mknod_arg);
    run("/bin/ln", ln_arg);
}