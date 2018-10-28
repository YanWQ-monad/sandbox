#include "src/runner.h"

#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

#include <cassert>

#include "src/exception.h"
#include "src/parent.h"
#include "src/child.h"

SandboxResult run(const SandboxConfig &config) {
  // uid_t uid = getuid();
  // if (uid != 0) {
  //   throw SandboxException(kRootRequired);
  // }

  timeval start_time;
  gettimeofday(&start_time, nullptr);

  pid_t child_pid = fork();
  if (child_pid == -1) {
    throw SandboxException(kForkFailed, static_cast<int>(errno));
  } else if (child_pid == 0) {
    child_process(config);
  } else {
    SandboxResult result = parent_process(config, child_pid, start_time);
    return result;
  }
  assert(false);
}
