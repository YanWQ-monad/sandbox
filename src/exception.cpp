#include "src/exception.h"

#include <cassert>

const char* error_message[] = {
  "Unspecified error",
  "Sandbox requires root permission to set limit",
  "Could fork child process",
  "Failed to set rlimit",
  "Failed to dup2 io files",
  "Failed to change user",
  "Failed to chroot",
  "Failed to load seccomp",
  "Failed to execve target executable file",
  "Failed to create new thread",
  "Failed to wait for the child exit"
};

SandboxException::SandboxException(const ErrorCode code, const int sys_errno)
    : std::exception(), error_code_(code), errno_(sys_errno) {}

const char* SandboxException::what() const noexcept {
  static_assert(kCountError <= sizeof(error_message) / sizeof(const char*),
                "error_message cannot be smaller than enum ErrorCode");
  return error_message[error_code_];
}
