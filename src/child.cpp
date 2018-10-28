#include "src/child.h"

#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <grp.h>

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "src/exception.h"
#include "src/rules/rules.h"

void set_rlimits(const SandboxConfig &config);
void dup2_stdio(const SandboxConfig &config);
void change_user(const SandboxConfig &config);
void change_root(const SandboxConfig &config);
void load_seccomp(const SandboxConfig &config);
void exec_process(const SandboxConfig &config);

void child_process(const SandboxConfig &config) {
  try {
    set_rlimits(config);
    dup2_stdio(config);
    change_root(config);
    change_user(config);
    load_seccomp(config);
    exec_process(config);
  }
  catch (SandboxException &e) {
    // fprintf(stderr, "Caught error: %s\n", e.what());
    // fprintf(stderr, "Errno: %d (%s)\n", e.errno_, strerror(e.errno_));
    raise(SIGUSR1);
    exit(EXIT_FAILURE);
  }
}

void set_one_rlimit(const int type, const auto value) {
  if (value != UNLIMITED) {
    rlimit limit;
    limit.rlim_cur = limit.rlim_max = static_cast<rlim_t>(value);
    if (setrlimit(type, &limit))
      throw SandboxException(kSetrlimitFailed, errno);
  }
}

void set_rlimits(const SandboxConfig &config) {
  set_one_rlimit(RLIMIT_STACK, config.max_stack);
  set_one_rlimit(RLIMIT_CPU, config.max_cpu_time / 1000);
  set_one_rlimit(RLIMIT_NPROC, config.max_process_number);
  set_one_rlimit(RLIMIT_FSIZE, config.max_output_size);

  if (config.max_memory != UNLIMITED) {
    auto memory_limit = config.max_memory;
    if (config.memory_check_only)
      memory_limit <<= 1;
    set_one_rlimit(RLIMIT_AS, memory_limit);
  }
}

void dup2_stdio(const SandboxConfig &config) {
  FILE *input, *output, *error;

  if (config.input_path) {
    input = fopen(config.input_path, "r");
    if (!input || dup2(fileno(input), fileno(stdin)) == -1)
      throw SandboxException(kDup2Failed, errno);
  }

  if (config.output_path) {
    output = fopen(config.output_path, "w");
    if (!output || dup2(fileno(output), fileno(stdout)) == -1)
      throw SandboxException(kDup2Failed, errno);
  }

  if (config.error_path) {
    if (config.output_path && !strcmp(config.output_path, config.error_path)) {
      error = output;
    } else {
      error = fopen(config.error_path, "w");
      if (!error)
        throw SandboxException(kDup2Failed, errno);
    }
    if (dup2(fileno(error), fileno(stderr)) == -1)
      throw SandboxException(kDup2Failed, errno);
  }
}

void change_root(const SandboxConfig &config) {
  if (config.chroot_path && chroot(config.chroot_path) == -1)
    throw SandboxException(kChrootFailed, errno);
}

void change_user(const SandboxConfig &config) {
  gid_t group_list[1] = {config.gid};
  if (config.gid &&
      (setgid(config.gid) == -1 || setgroups(1, group_list) == -1)) {
    throw SandboxException(kSetuidFailed, errno);
  }

  if (config.uid && setuid(config.uid) == -1)
    throw SandboxException(kSetuidFailed, errno);
}

void load_seccomp(const SandboxConfig &config) {
  switch (config.seccomp_rule) {
    case kNoneRule:
      break;

    case kClikeRule:
      if (rule_load_clike(config.exe_path))
        throw SandboxException(kLoadSeccompFailed, errno);
      break;

    default:
      assert(false);
  }
}

void exec_process(const SandboxConfig &config) {
  if (execve(config.exe_path, const_cast<char* const*>(config.args), nullptr))
    throw SandboxException(kExecveFailed, errno);
}
