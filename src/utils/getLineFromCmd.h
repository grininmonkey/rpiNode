#ifndef GET_LINE_FROM_CMD_H
#define GET_LINE_FROM_CMD_H

#include <stddef.h>  // For size_t

// Executes a shell command and stores the first line of output in `output`.
// Returns 0 on success, -1 on error.
// `output` should have space for at least `output_size` bytes.
int get_line_from_cmd(const char *cmd, char *output, size_t output_size);

#endif // GET_LINE_FROM_CMD_H

