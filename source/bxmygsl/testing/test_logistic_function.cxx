#include <iostream>

#include <mygsl/logistic_function.h>

int main(void)
{
  std::clog << "test_logistic_function: Entering...\n";

  mygsl::logistic_function f(3.0, 0.5, 1.0);

  for (double x = -3.0; x <= +3.0; x += 0.01) {
    std::cout << x << ' ' << f.eval(x) << std::endl;
  }

  std::clog << "test_logistic_function: Exiting.\n";
  return 0;
}
