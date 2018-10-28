#include <cstdio>

#include <cstring>
#include <cstdlib>

#include "src/exception.h"
#include "src/runner.h"
#include "src/args.h"

int main(int argc, char *argv[]) {
  SandboxConfig config = parse_args(argc, argv);
  SandboxResult result;

  try {
    result = run(config);
  }
  catch (SandboxException &e) {
    fprintf(stderr, "Caught error: %s\n", e.what());
    fprintf(stderr, "Errno: %d (%s)\n", e.errno_, strerror(e.errno_));
    exit(EXIT_FAILURE);
  }
  
  puts("");
  printf("Result    : %d\n", result.result);
  printf("CPU Time  : %dms\n", result.cpu_time);
  printf("Real Time : %dms\n", result.real_time);
  printf("Memory    : %dMB\n", result.memory >> 20);
  printf("Signal    : %d\n", result.signal);
  printf("Error     : %d\n", result.error);
  printf("Exit Code : %d\n", result.exit_code);

  return 0;
}
