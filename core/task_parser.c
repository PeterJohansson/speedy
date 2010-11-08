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
#include "task_parser.h"
#include "config_parser.h"
#include "task_handler.h"
#include "thread_pool.h"
#include "queue.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

/*! Successful return  code for thread pool callback function. */
#define TASK_PARSER_EXEC_SUCCESS 0

typedef enum namespace_t {
    NAMESPACE_OPTIONS,
    NAMESPACE_CONFIG
} namespace_t;

typedef enum config_options_t {
    CONFIG_OPTIONS_DEPENDENCY,
    CONFIG_OPTIONS_PATH,
    CONFIG_OPTIONS_UNKOWN
} config_options_t;

typedef enum task_options_t {
    TASK_OPTIONS_NAME,
    TASK_OPTIONS_PROVIDES,
    TASK_OPTIONS_DEPENDENCY,
    TASK_OPTIONS_UNKOWN
} task_options_t;

/*!
 * A simpler structure for tasks which doesn't have dependencies.
 */
typedef struct task_parser_simple_task_t {
    /*!< A pointer to the task parser handle. */
    task_parser_t *task_parser;
    /*! A pointer to the functional call for the task. */
    void (*task_exec)(void *argument);
} task_parser_simple_task_t;


/*!
 * A simple structure for tasks which parse a single file.
 */
typedef struct task_parser_config_reader_t {
    /*! C inheritance of a simple task.*/
    task_parser_simple_task_t task;
    /*! Filename for the configuration file. */
    char *filename;
    /*! Handle for the config file. */
    config_parser_t *file;
    /*! Default namespace for the configuration file. */
    char *default_namespace;
    /*! Current namespace for the configuration task. */
    const char *current_namespace;
    /*! Extracted value for the current namespace. */
    namespace_t current_namespace_value;

    service_t *current_task;

    queue_t paths;

    queue_t tasks;
} task_parser_config_reader_t;

/*!
 * A simple structure for tasks which scans directory.
 */
typedef struct task_parser_scan_dir_t {
    /*! C inheritance of a simple task.*/
    task_parser_simple_task_t task;
    /*! A path to where the directory scanner should scan. */
    char * path;

    queue_t tasks;
} task_parser_scan_dir_t;


static int task_parser_exec(void *task);

static void task_parser_read_file_exec(void *argument);
static void task_parser_read_file_destroy(task_parser_config_reader_t *config);
static task_parser_config_reader_t* task_parser_read_file_create(
                                       task_parser_t *this_ptr,
                                       char *filename,
                                       char *default_namespace);

static char* task_parser_check_dependency(task_parser_config_reader_t *config);
static void task_parser_add_task(task_parser_config_reader_t *config);
static void task_parser_create_task(task_parser_config_reader_t *config);

static void task_parser_handle_options(task_parser_config_reader_t *config);
static void task_parser_handle_task(task_parser_config_reader_t *config);

static namespace_t task_parser_get_namespace_value(const char *str_namespace);
static config_options_t task_parser_get_config_options(const char* str_command);
static task_options_t task_parser_get_task_options(const char* str_command);

static void task_parser_scan_dir_exec(void *arg);
static void task_parser_scan_dir_destroy(task_parser_scan_dir_t *scan_dir);
static task_parser_scan_dir_t* task_parser_scan_dir_create(
                                       task_parser_t *this_ptr,
                                       queue_t *file_queue,
                                       const char *path);

static char* task_parser_scan_dir_find_file(task_parser_scan_dir_t *scan_dir,
                                            char *filename);
static void task_parser_scan_dir_read_file(task_parser_scan_dir_t *scan_dir,
                                            char *task);

/*!
 * Creates a task parser handle.
 *
 * \param this_ptr - A pointer to the task handler where all the tasks should
 *                   be registered.
 *
 * \return A pointer to the task parser handle if it was successfully created,
 *         \c NULL otherwise.
 */
task_parser_t* task_parser_create(task_handler_t *handler)
{
    task_parser_t *task_parser = malloc(sizeof(task_parser_t));
    
    if (task_parser != NULL) {
        task_parser->handler = handler;
        task_parser->thread_pool = thread_pool_create(4, task_parser_exec);

        if (task_parser->thread_pool == NULL) {
            free(task_parser);
            task_parser = NULL;
        }
    }
    return task_parser;
}

/*!
 * Creates a task which reads a config file.
 *
 * \param this_ptr - A pointer to the task parser handle.
 * \param filename - The config file.
 */
void task_parser_read(task_parser_t *this_ptr, const char * filename)
{
    task_parser_config_reader_t *config;
    config = task_parser_read_file_create(this_ptr, strdup(filename),
                                          strdup("default"));
    if (config != NULL) {
        thread_pool_add_task(this_ptr->thread_pool, config);
    }
}

/*!
 * Waits until the thread pool has executed all the tasks in the queue.
 *
 * \param this_ptr - A pointer to the task parser.
 */
void task_parser_wait(task_parser_t* this_ptr)
{
    thread_pool_wait(this_ptr->thread_pool);
}

/*!
 * Destroys and deallocates a task parser handle.
 *
 * \param this_ptr - A pointer to the task parser handle.
 */
void task_parser_destroy(task_parser_t *task_parser)
{
    thread_pool_destroy(task_parser->thread_pool);
    free(task_parser);
}

/*!
 * This is callback from the thread pool which sends the next task that should
 * be executed as an argument.
 *
 * \param task - A pointer to \c task_parser_simple_task_t struct.
 *
 * \return \c TASK_PARSER_EXEC_SUCCESS if the task executed successfully.
 */
static int task_parser_exec(void *task)
{
    task_parser_simple_task_t *simple_task = (task_parser_simple_task_t*) task;
    simple_task->task_exec(simple_task);

    return TASK_PARSER_EXEC_SUCCESS;
}

/*****************************************************************************/
/* Functions to parse a configuration file.                                  */
/*****************************************************************************/

/*!
 * This is a task which reads a config file.
 *
 * \param arg - A pointer to the arguments that the task needs.
 */
static void task_parser_read_file_exec(void *arg)
{
    task_parser_config_reader_t *config = arg;

    config->file = config_parser_open(config->filename);
    printf("Scanning: %s\n", config->filename);
    config_parser_set_namespace(config->file, config->default_namespace);

    while (!config_parser_is_eof(config->file) &&
           (config_parser_read(config->file) == PARSER_OK)) {

        config->current_namespace = config_parser_get_namespace(config->file);
        config->current_namespace_value = task_parser_get_namespace_value(
                                              config->current_namespace);

        switch (config->current_namespace_value) {
            case NAMESPACE_OPTIONS:
                task_parser_handle_options(config);
                break;

            case NAMESPACE_CONFIG:
                task_parser_handle_task(config);
                break;

            default:
                /* Do nothing. */
                break;
        }
    }

    config_parser_close(config->file);
    task_parser_read_file_destroy(config);
}

/*!
 * Creates a simple task which will parse a file.
 *
 * \param this_ptr - A pointer to the task parser handle.
 * \param filename - Filename of the file that needs to be parsed.
 *
 * \return A simple task which will read a file.
 */
static task_parser_config_reader_t* task_parser_read_file_create(
                                       task_parser_t *this_ptr,
                                       char *filename,
                                       char *default_namespace)
{
    task_parser_config_reader_t *config;
    config = malloc(sizeof(task_parser_config_reader_t));

    if (config != NULL) {
        config->task.task_parser = this_ptr;
        config->filename = filename;
        config->task.task_exec = task_parser_read_file_exec;
        config->default_namespace = default_namespace;

        queue_init(&config->tasks);
        queue_init(&config->paths);

        task_parser_create_task(config);

    } else {
        free(filename);
    }

    return config;
}

/*!
 * Destroys and deallocates a read file task.
 * If there were any paths detected in the configuration file it will now
 * start up a task which will search for files in a directory.
 *
 * \param config - A pointer to the read file task.
 */
static void task_parser_read_file_destroy(task_parser_config_reader_t *config)
{
    char* path;
    char* task;
    bool added_task = false;

    if (config->current_task->name != NULL) {
        task_parser_add_task(config);
    }
    free(config->current_task);

    /* Free all the option paths. */
    while((path = queue_pop(&config->paths)) != NULL) {

        task_parser_scan_dir_t *scanner;
        scanner = task_parser_scan_dir_create(config->task.task_parser,
                                                 &config->tasks,
                                                 path);
        if (scanner != NULL) {
            thread_pool_add_task(config->task.task_parser->thread_pool, scanner);
            added_task = true;
        }
        free(path);
    }
    queue_deinit(&config->paths);

    /* Free all the remaining tasks and print an error since these were not
       possible to find in either the current file or in the paths. */
    while((task = queue_pop(&config->tasks)) != NULL) {
        if (added_task) {
            printf("Task: %s\n",task);
        } else {
            printf("Missing task: %s\n",task);
        }
        free(task);
    }
    queue_deinit(&config->tasks);

    free(config->default_namespace);
    free(config->filename);
    free(config);
}


static char* task_parser_check_dependency(task_parser_config_reader_t *config)
{
    char* current_task;

    queue_first(&config->tasks);
    while ((current_task = queue_get_current(&config->tasks)) != NULL) {
        if (strcmp(config->current_task->name, current_task) == 0) {
            queue_remove_current(&config->tasks);
            return current_task;
        }
        queue_next(&config->tasks);
    }
    return NULL;
}

static void task_parser_add_task(task_parser_config_reader_t *config)
{
    char* dependency = task_parser_check_dependency(config);

    if (dependency != NULL) {
        /* Add task. */
        free(dependency);
    }

    free((char*) config->current_task->name);
    free(config->current_task->dependency);
    free((char*) config->current_task->provides);
    free(config->current_task->action);
    free(config->current_task);
    config->current_task = NULL;
}

static void task_parser_create_task(task_parser_config_reader_t *config)
{
    /* Create a new task and initialize it. */
    config->current_task = malloc(sizeof(service_t));
    config->current_task->name = NULL;
    config->current_task->dependency = NULL;
    config->current_task->provides = NULL;
    config->current_task->action = NULL;
}

/*!
 * Handles any options which has been parsed from the configuration.
 *
 * \param config - Contains the local settings for the current
 *                 parser task.
 */
static void task_parser_handle_options(task_parser_config_reader_t *config)
{
    config_options_t options;
    const char *command;
    const char *argument;

    command = config_parser_get_command(config->file);
    options = task_parser_get_config_options(command);

    switch (options) {
        case CONFIG_OPTIONS_DEPENDENCY:
            argument = config_parser_get_next_argument(config->file);
            while (argument != NULL) {
                queue_push(&config->tasks, strdup(argument));                
                argument = config_parser_get_next_argument(config->file);
            }
            break;

        case CONFIG_OPTIONS_PATH:
            argument = config_parser_get_next_argument(config->file);
            while (argument != NULL) {
                queue_push(&config->paths, strdup(argument));
                argument = config_parser_get_next_argument(config->file);
            }
            break;

        case CONFIG_OPTIONS_UNKOWN:
        default:
            break;
    }
}

/*!
 * Handles a task which has been parsed from the configuration.
 *
 * \param config - Contains the local settings for the current
 *                 parser task.
 */
static void task_parser_handle_task(task_parser_config_reader_t *config)
{
    task_options_t options;
    const char *command;
    const char *argument;

    if (config->current_task->name == NULL) {
        config->current_task->name = strdup(config->current_namespace);

    } else if (strcmp(config->current_namespace,
                      config->current_task->name) != 0) {

        /* Add the current task object. */
        task_parser_add_task(config);

        /* Create a new task object and name it according to the
           new namespace. */
        task_parser_create_task(config);
        config->current_task->name = strdup(config->current_namespace);
    }
    command = config_parser_get_command(config->file);
    options = task_parser_get_task_options(command);

    switch (options) {
        case TASK_OPTIONS_DEPENDENCY:
            break;

        case TASK_OPTIONS_PROVIDES:
            argument = config_parser_get_next_argument(config->file);
            printf("provides: %s\n", argument);

            if ((config->current_task->provides == NULL) &&
                (argument != NULL)) {

                config->current_task->provides = strdup(argument);
            } else {
                printf("some error...\n");
            }
            break;

        default:
            break;
    }
}

/*!
 * Gets the config namespace value from a string.
 * \note This is separated here for readability.
 *
 * \param str_namespace - A string which needs to be transformed into an
 *                        integer value.
 *
 * \return The string value represented as an integer value.
 */
static namespace_t task_parser_get_namespace_value(const char *str_namespace)
{
    if (strcmp(str_namespace, "options") == 0) {
        return NAMESPACE_OPTIONS;

    } else {
        return NAMESPACE_CONFIG;
    }
}

/*!
 * Gets the config option value from a string.
 * \note This is separated here for readability.
 *
 * \param str_command - A string which needs to be transformed into an
 *                      integer value.
 *
 * \return The string value represented as an integer value.
 */
static config_options_t task_parser_get_config_options(const char* command)
{
    if (strcmp(command, "dependency") == 0) {
        return CONFIG_OPTIONS_DEPENDENCY;

    } else if (strcmp(command, "path") == 0) {
        return CONFIG_OPTIONS_PATH;

    } else {
        return CONFIG_OPTIONS_UNKOWN;
    }
}

/*!
 * Gets the config option value from a string.
 * \note This is separated here for readability.
 *
 * \param str_command - A string which needs to be transformed into an
 *                      integer value.
 *
 * \return The string value represented as an integer value.
 */
static task_options_t task_parser_get_task_options(const char* command)
{
    if (strcmp(command, "dependency") == 0) {
        return TASK_OPTIONS_DEPENDENCY;

    } else if (strcmp(command, "provides") == 0) {
        return TASK_OPTIONS_PROVIDES;

    } else {
        return TASK_OPTIONS_UNKOWN;
    }
}

/*****************************************************************************/
/* Functions to scan a directory for configuration files.                    */
/*****************************************************************************/

/*!
 * This is a task which scans a directory for configuration files.
 *
 * \param arg - A pointer to the arguments that the task needs.
 */
static void task_parser_scan_dir_exec(void *arg)
{
    task_parser_scan_dir_t *scan_dir = arg;
    struct dirent *content;
    char *task;
    DIR *dir;

    printf("Path: %s\n",scan_dir->path);

    dir = opendir(scan_dir->path);
    if (dir) {
        while ((content = readdir(dir)) != NULL) {

            task = task_parser_scan_dir_find_file(scan_dir, content->d_name);

            if (task != NULL) {
                task_parser_scan_dir_read_file(scan_dir, task);
            }
            free(task);
        }
        closedir(dir);
    }
    task_parser_scan_dir_destroy(scan_dir);
}

/*!
 * Creates a simple task which will scan a directory for configuration files.
 *
 * \param this_ptr - A pointer to the task parser handle.
 * \param filename - Filename of the file that needs to be parsed.
 *
 * \return A simple task which will read a file.
 */
static task_parser_scan_dir_t* task_parser_scan_dir_create(
                                       task_parser_t *this_ptr,
                                       queue_t *tasks,
                                       const char *path)
{
    char *task;
    char *task_dup;
    task_parser_scan_dir_t *scan_dir;
    scan_dir = malloc(sizeof(task_parser_scan_dir_t));

    if (scan_dir != NULL) {
        scan_dir->task.task_parser = this_ptr;
        scan_dir->path = strdup(path);
        scan_dir->task.task_exec = task_parser_scan_dir_exec;

        queue_init(&scan_dir->tasks);

        /* Copy the task list just to know the expected name of the
           configuration files. */
        queue_first(tasks);
        while((task = queue_get_current(tasks)) != NULL) {
            task_dup = strdup(task);
            queue_push(&scan_dir->tasks, task_dup);
            queue_next(tasks);
        }
    }

    return scan_dir;
}

/*!
 * Destroys and deallocates a directory scan task.
 *
 * \param config - A pointer to the directory scan task.
 */
static void task_parser_scan_dir_destroy(task_parser_scan_dir_t *scan_dir)
{
    char* task;

    while((task = queue_pop(&scan_dir->tasks)) != NULL) {
        printf("Missing task: %s\n",task);
        free(task);
    }

    queue_deinit(&scan_dir->tasks);

    free(scan_dir->path);
    free(scan_dir);
}


static char* task_parser_scan_dir_find_file(task_parser_scan_dir_t *scan_dir,
                                       char *filename)
{
    char *task;

    queue_first(&scan_dir->tasks);
    while ((task = queue_get_current(&scan_dir->tasks)) != NULL) {

        if (strcmp(task, filename) == 0) {
            queue_remove_current(&scan_dir->tasks);
            return task;
        }
        queue_next(&scan_dir->tasks);
    }
    return NULL;
}


static void task_parser_scan_dir_read_file(task_parser_scan_dir_t *scan_dir,
                                            char *task)
{
    task_parser_config_reader_t *config;
    char *filename;
    char *next;

    /* Create path to file. */
    filename = malloc(strlen(scan_dir->path) + strlen(task) + 2u);
    filename[0] = '\0';
    next = strcat(filename, scan_dir->path);
    next = strcat(next, "/");
    (void) strcat(next, task);

    config = task_parser_read_file_create(scan_dir->task.task_parser, filename,
                                          strdup(task));

    if (config != NULL) {
        queue_push(&config->tasks, strdup(task));
        thread_pool_add_task(scan_dir->task.task_parser->thread_pool, config);
    }
}
