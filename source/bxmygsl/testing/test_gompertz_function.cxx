// Standard library:
#include <iostream>

// Ourselves:
#include <mygsl/gompertz_function.h>

int main(void)
{
  std::clog << "test_gompertz_function: Entering...\n";

  mygsl::gompertz_function f(1.0, 0.5, 2.0);

  for (double x = -3.0; x <= +3.0; x += 0.01) {
    std::cout << x << ' ' << f(x) << std::endl;
  }

  std::clog << "test_gompertz_function: Exiting.\n";
  return 0;
}
