#ifndef ARGS_H
#define ARGS_H

/**
 * @brief Parses port, path & workers arguments, changes
 * changes working directory to path.
 *
 * @return 0 on success, -1 otherwise
 */
int args_init(int argc, char** argv);

/**
 * @return workers value
 */
int get_workers();

/**
 * @return port value
 */
int get_port();

#endif