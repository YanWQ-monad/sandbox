#include <cstdlib>

int main() {
  while (true)
    malloc(128);

  return 0;
}
