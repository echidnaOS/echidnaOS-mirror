#include <stdint.h>
#include <kernel.h>

int stdin_io_wrapper(uint32_t unused, uint64_t loc, int type, uint8_t payload) {
    if (type == 0)
        return vfs_kread(task_table[current_task]->stdin, loc);
    else if (type == 1)
        return 0;
}

int stdout_io_wrapper(uint32_t unused, uint64_t loc, int type, uint8_t payload) {
    if (type == 0)
        return 0;
    else if (type == 1)
        return vfs_kwrite(task_table[current_task]->stdout, loc, payload);
}

int stderr_io_wrapper(uint32_t unused, uint64_t loc, int type, uint8_t payload) {
    if (type == 0)
        return 0;
    else if (type == 1)
        return vfs_kwrite(task_table[current_task]->stderr, loc, payload);
}

void init_streams(void) {
    kernel_add_device("stdin", 0, &stdin_io_wrapper);
    kernel_add_device("stdout", 0, &stdout_io_wrapper);
    kernel_add_device("stderr", 0, &stderr_io_wrapper);
    return;
}
