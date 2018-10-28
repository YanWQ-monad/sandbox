#ifndef SRC_PARENT_H_
#define SRC_PARENT_H_

#include "src/runner.h"

SandboxResult parent_process(const SandboxConfig &config, const pid_t child_pid,
                             const timeval start_time);

#endif  // SRC_PARENT_H_
