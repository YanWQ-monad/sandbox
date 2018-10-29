#include "src/parent.h"

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "src/exception.h"
#include "src/runner.h"

struct KillerArgument {
  pid_t pid;
  unsigned timeout;
};

void* killer(void *arg) {
  pid_t pid = reinterpret_cast<KillerArgument*>(arg)->pid;
  unsigned timeout = reinterpret_cast<KillerArgument*>(arg)->timeout;

  if (pthread_detach(pthread_self())) {
    kill(pid, SIGKILL);
    return NULL;
  }
  if (sleep(((timeout + 1000) / 1000))) {
    kill(pid, SIGKILL);
    return NULL;
  }
  if (kill(pid, SIGKILL))
    return NULL;
  return NULL;
}

SandboxResult parent_process(
    const SandboxConfig &config,
    const pid_t child_pid,
    const timeval start_time) {
  pthread_t killer_tid = 0;
  KillerArgument *killer_arg =
    static_cast<KillerArgument*>(malloc(sizeof(KillerArgument)));

  if (config.max_real_time != UNLIMITED) {
    killer_arg->pid = child_pid;
    killer_arg->timeout = config.max_real_time;
    if (pthread_create(&killer_tid, NULL, killer,
                       reinterpret_cast<void*>(killer_arg))) {
      auto error = errno;
      kill(child_pid, SIGKILL);
      throw SandboxException(kPthreadFailed, error);
    }
  }

  int status;
  rusage usage;
  timeval end_time;
  SandboxResult result;

  memset(&result, 0, sizeof(result));

  if (wait4(child_pid, &status, WSTOPPED, &usage) == -1) {
    auto error = errno;
    kill(child_pid, SIGKILL);
    throw SandboxException(kWaitFailed, error);
  }

  gettimeofday(&end_time, NULL);
  result.real_time = (end_time.tv_sec * 1000 + end_time.tv_usec / 1000) -
      (start_time.tv_sec * 1000 + start_time.tv_usec / 1000);

  if (killer_tid)
    pthread_cancel(killer_tid);
  free(killer_arg);

  if (WIFSIGNALED(status))
    result.signal = WTERMSIG(status);

  if (result.signal == SIGUSR1) {
    result.result = kSystemError;
  } else {
    result.exit_code = WEXITSTATUS(status);
    result.cpu_time = usage.ru_utime.tv_sec * 1000 +
                      usage.ru_utime.tv_usec / 1000;
    result.memory = usage.ru_maxrss << 10;

    if (result.exit_code)
      result.result = kRuntimeError;

    if (config.max_memory != UNLIMITED && result.memory > config.max_memory) {
      result.result = kMemoryExceeded;
    } else if (config.max_cpu_time != UNLIMITED &&
        result.cpu_time > config.max_cpu_time) {
      result.result = kTimeLimitExceeded;
    } else if (config.max_real_time != UNLIMITED &&
        result.real_time > config.max_real_time) {
      result.result = kRealTimeLimitExceeded;
    } else if (result.signal) {
      result.result = kRuntimeError;
    }
  }

  return result;
}
