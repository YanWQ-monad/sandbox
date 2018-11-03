#include "src/args.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

ProgramOptions::ProgramOptions(const int argc, char const* const* argv)
    : argc_(argc), argv_(argv) {}


int ProgramOptions::find_key(const char *key) const {
  for (int i=1; i < argc_; i++)
    if (!strcmp(argv_[i], key))
      return i;
  return -1;
}

int ProgramOptions::parse_int(const char *key, const int defaul) const {
  int pos = find_key(key);
  if (pos + 1 == argc_ || pos == -1)
    return defaul;
  return atoi(argv_[pos + 1]);
}

const char* ProgramOptions::parse_string(const char *key,
                                         const char *defaul) const {
  int pos = find_key(key);
  if (pos + 1 == argc_ || pos == -1)
    return defaul;
  return argv_[pos + 1];
}

void ProgramOptions::copy_extra_args(const char **target,
                                     const int max_length) const {
  int pos = 1;
  for (; pos < argc_; pos+=2)
    if (!(argv_[pos][0] == '-' && argv_[pos][1] == '-'))
      break;
  for (int i=pos; i < argc_; i ++) {
    if (i - pos + 1 >= max_length)
      break;
    target[i - pos + 1] = argv_[i];
  }
}

SandboxConfig parse_args(const int argc, char *argv[]) {
  const ProgramOptions opts(argc, argv);

  SandboxConfig config;

  for (int i=0; i < ARGS_MAX_NUMBER; i++)
    config.args[i] = nullptr;

  config.max_cpu_time = opts.parse_int("--max_cpu_time", UNLIMITED);
  config.max_real_time = opts.parse_int("--max_real_time", UNLIMITED);
  config.max_memory = opts.parse_int("--max_memory", UNLIMITED);
  config.memory_check_only = opts.parse_int("--memory_check_only", false);
  config.max_stack = opts.parse_int("--max_stack", UNLIMITED);
  config.max_process_number = opts.parse_int("--max_process_number", UNLIMITED);
  config.max_output_size = opts.parse_int("--max_output_size", UNLIMITED);
  config.chroot = opts.parse_string("--chroot");
  config.target = opts.parse_string("--target");
  config.stdin = opts.parse_string("--stdin");
  config.stdout = opts.parse_string("--stdout");
  config.stderr = opts.parse_string("--stderr");
  config.uid = opts.parse_int("--uid", 0);
  config.gid = opts.parse_int("--gid", 0);
  config.seccomp_rule =
      static_cast<SeccompRule>(opts.parse_int("--seccomp_rule", 0));
  opts.copy_extra_args(
      config.args,
      sizeof(config.args) / sizeof(config.args[0]));
  config.args[0] = config.target;

  if (config.target == nullptr) {
    puts("Missing args: target");
    exit(1);
  }

  return config;
}
