#ifndef SRC_RUNNER_H_
#define SRC_RUNNER_H_

#include <sys/types.h>

#include "src/exception.h"

constexpr int ARGS_MAX_NUMBER = 32;
constexpr unsigned UNLIMITED = 0xFFFFFFEF;

enum SeccompRule {
  kNoneRule = 0,
  kClikeRule
};

enum RunResult {
  kSuccess = 0,
  kTimeLimitExceeded,
  kRealTimeLimitExceeded,
  kMemoryExceeded,
  kRuntimeError,
  kSystemError
};

struct SandboxConfig {
  unsigned max_cpu_time;
  unsigned max_real_time;
  unsigned max_memory;
  unsigned max_stack;
  unsigned max_process_number;
  unsigned max_output_size;
  bool memory_check_only;
  const char *exe_path;
  const char *input_path;
  const char *output_path;
  const char *error_path;
  const char *args[ARGS_MAX_NUMBER];
  const char *chroot_path;
  SeccompRule seccomp_rule;
  uid_t uid;
  gid_t gid;
};

struct SandboxResult {
  unsigned cpu_time;
  unsigned real_time;
  unsigned memory;
  int signal;
  int exit_code;
  RunResult result;
  ErrorCode error;
};

SandboxResult run(const SandboxConfig &config);

#endif  // SRC_RUNNER_H_
