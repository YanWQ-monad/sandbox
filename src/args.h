#ifndef SRC_ARGS_H_
#define SRC_ARGS_H_

#include "src/runner.h"

class ProgramOptions {
 public:
  ProgramOptions(const int argc, char const* const* argv);

  int parse_int(const char *key, const int defaul) const;
  const char* parse_string(const char *key, const char *defaul = nullptr) const;
  void copy_extra_args(const char **target, const int max_length) const;
  int find_key(const char *key) const;

 private:

  int argc_;
  char const* const* argv_;
};

SandboxConfig parse_args(const int argc, char *argv[]);

#endif  // SRC_ARGS_H_
