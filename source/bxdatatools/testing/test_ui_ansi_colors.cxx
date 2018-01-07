// Ourselves:
#include <datatools/ui/ansi_colors.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

// This project:
#include <datatools/ui/special_symbols.h>

int main(int /* argc_ */, char * /* argv_ */[])
{

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_black()
            << "black"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_bright_black()
            << "gray"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_red()
            << "red"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_bright_red()
            << "bright red"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_green()
            << "green"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_bright_green()
            << "bright green"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_yellow()
            << "yellow"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_bright_yellow()
            << "bright yellow"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_blue()
            << "blue"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_bright_blue()
            << "bright blue"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_magenta()
            << "magenta"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_bright_magenta()
            << "bright magenta"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_cyan()
            << "cyan"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: " << datatools::ui::ansi_colors::foreground_bright_cyan()
            << "bright cyan"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::background_cyan()
            << datatools::ui::ansi_colors::foreground_white()
            << "white"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::foreground_blue()
            << datatools::ui::ansi_colors::high_intensity_on()
            << "high intensity blue"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::foreground_black()
            << datatools::ui::ansi_colors::high_intensity_on()
            << "high intensity black"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::inverse_on()
            << "inverse"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::underline_on()
            << "underline"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::foreground_green()
            << datatools::ui::check_mark()
            << "check"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::foreground_red()
            << datatools::ui::error_mark()
            << "error"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::blink_slow()
            << "blink slow"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::concealed_on()
            << "concealed"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::foreground_blue()
            << datatools::ui::ansi_colors::framed_on()
            << "framed blue"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::foreground_blue()
            << datatools::ui::ansi_colors::overlined_on()
            << "overlined blue"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::color_8bits(5,0,0)
            << "red (8 bits)"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::color_8bits(0,5,0)
            << "green (8 bits)"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::color_8bits(0,0,5)
            << "blue (8 bits)"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::color_8bits(datatools::ui::ansi_colors::color_8bits::MODE_GRAYSCALE, 12)
            << "gray (8 bits)"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::color_8bits(5,5,0)
            << "yellow (8 bits)"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << "Test: "
            << datatools::ui::ansi_colors::color_8bits(5,2,0)
            << "orange (8 bits)"  << datatools::ui::ansi_colors::reset() << std::endl;

  std::cout << std::endl;

  return EXIT_SUCCESS;
}
