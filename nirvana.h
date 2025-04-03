/*

The MIT License (MIT)

Copyright (c) <2025> <yuriyuri824>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef _NIRVANA_H
#define _NIRVANA_H

#ifdef NIRVANA_IMPL
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if defined(__linux__)
    #include <linux/limits.h>
    #define NIR_ARG_MAX		(ARG_MAX)
#else
    #error "your platform isn't supported yet!"
#endif

#define NIR_DEFAULT_ARRAY_CAP	(64)

#define NIR_ASSERT(_e, format)	if (!_e) { NIR_ERROR(format); exit(-1); }

#ifdef NIRVANA_ENABLE_LOG
#define NIR_INFO(...) {			\
    fprintf(stdout, "[NIR-INFO]: ");	\
    fprintf(stdout, __VA_ARGS__);	\
}

#define NIR_ERROR(...) {		\
    fprintf(stderr, "[NIR-ERR]: ");	\
    fprintf(stderr, __VA_ARGS__);	\
}

#else
    #define NIR_INFO(...)
    #define NIR_ERROR(...)
#endif

#define NIR_UNUSED(_e)	(void)(_e)

typedef struct {
    const char **element;
    size_t capacity;
    size_t index;
} nir_cmd_t;

bool nir_cmd_init(nir_cmd_t *cmd) {
    cmd->capacity = NIR_DEFAULT_ARRAY_CAP;
    cmd->element = malloc(cmd->capacity * sizeof(char*));
    NIR_ASSERT(cmd->element,
        "Couldn't allocate memory for element array\n");
    cmd->index = 0;
    
    NIR_INFO("cmd->capacity: %zu\n", cmd->capacity);

    return true;
}

void nir_cmd_append(nir_cmd_t *cmd, const char *str) {
    /* NIR_INFO("entry[%zu]: \"%s\"\n", cmd->index, str); */
    if (cmd->index >= cmd->capacity) {
        size_t new_capacity = cmd->capacity * 2;
        cmd->element = realloc(cmd->element, new_capacity * sizeof(char*));
        NIR_ASSERT(cmd->element,
            "Couldn't reallocate memory for element array\n");
        NIR_INFO("Reallocated array to %zu\n", new_capacity);
        cmd->capacity = new_capacity;
    }
    cmd->element[cmd->index++] = str;
}

void nir_cmd_run(nir_cmd_t *cmd) {
    char buffer[NIR_ARG_MAX] = {0};
    char *buffer_ptr = buffer;

    for (size_t i = 0; i < cmd->index; ++i) {
        const char *entry = cmd->element[i];
        size_t entry_len = strlen(entry);
        strncat(buffer_ptr, entry, entry_len);
        strncat(buffer_ptr + entry_len, " ",  entry_len + 1);            
    }
    NIR_INFO("Running: \"%s\"\n\n", buffer);
    system(buffer_ptr);
    cmd->index = 0;
}

void nir_cmd_cleanup(nir_cmd_t *cmd) {
    free(cmd->element);
    NIR_INFO("Cleanup finished\n\n");
}

void nir_cmd_goes_samsara(void) {
    NIR_INFO("Rebuilding nirvana.c ...\n");
    nir_cmd_t cmd = {0};
    NIR_ASSERT(nir_cmd_init(&cmd),
        "Couldn't initialize CMD for rebuilding\n");
    nir_cmd_append(&cmd, "gcc -o nirvana nirvana.c");
    nir_cmd_append(&cmd, "-std=c99 -Wall -Werror -Wextra -pedantic -Wformat");
    nir_cmd_run(&cmd);
    
    nir_cmd_cleanup(&cmd);
}

#endif /* NIRVANA_IMPL */

#endif /* _NIRVANA_H */
