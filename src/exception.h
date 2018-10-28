#ifndef SRC_EXCEPTION_H_
#define SRC_EXCEPTION_H_

#include <exception>

constexpr int ESUCCESS = 0;

enum ErrorCode {
  kDefaultError = 0,
  kRootRequired,
  kForkFailed,
  kSetrlimitFailed,
  kDup2Failed,
  kSetuidFailed,
  kChrootFailed,
  kLoadSeccompFailed,
  kExecveFailed,
  kPthreadFailed,
  kWaitFailed,
  kCountError
};

class SandboxException : public std::exception {
 public:
  ErrorCode error_code_;
  int errno_;

  explicit SandboxException(
      const ErrorCode code = kDefaultError,
      const int sys_errno = ESUCCESS);
  const char* what() const noexcept;
};

#endif  // SRC_EXCEPTION_H_
