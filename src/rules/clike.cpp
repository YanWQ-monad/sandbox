#include <sys/prctl.h>
#include <seccomp.h>
#include <fcntl.h>

#include "src/rules/rules.h"

int rule_load_clike(const char *exe_path) {
  prctl(PR_SET_NO_NEW_PRIVS, 1);

  constexpr static int syscalls_whitelist[] = {
    SCMP_SYS(read), SCMP_SYS(readlink), SCMP_SYS(write), SCMP_SYS(close),
    SCMP_SYS(fstat), SCMP_SYS(access), SCMP_SYS(mprotect), SCMP_SYS(mmap),
    SCMP_SYS(munmap), SCMP_SYS(uname), SCMP_SYS(arch_prctl), SCMP_SYS(brk),
    SCMP_SYS(exit_group), SCMP_SYS(sysinfo), SCMP_SYS(writev), SCMP_SYS(lseek)
  };

  scmp_filter_ctx ctx;
  if (!(ctx = seccomp_init(SCMP_ACT_KILL)))
    return -1;

  for (auto white : syscalls_whitelist)
    if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, white, 0))
      return -1;

  if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 1,
                       SCMP_A0(SCMP_CMP_EQ, (scmp_datum_t)(exe_path))))
    return -1;

  if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 1,
                       SCMP_CMP(1, SCMP_CMP_MASKED_EQ, O_WRONLY | O_RDWR, 0)))
    return -1;

  if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat), 1,
                       SCMP_CMP(2, SCMP_CMP_MASKED_EQ, O_WRONLY | O_RDWR, 0)))
    return -1;

  if (seccomp_load(ctx))
    return -1;

  seccomp_release(ctx);

  return 0;
}
