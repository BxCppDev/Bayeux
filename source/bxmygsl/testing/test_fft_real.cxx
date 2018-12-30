// test_fft_real.cxx

// Standard Library:
#include <iostream>

// Ourselves:
#include <mygsl/fft_real.h>

int main(int argc_,  char ** argv_)
{
  bool debug = false;
  double t_step = 0.5;

  int iarg = 1;
  while (iarg < argc_) {
    std::string arg=argv_[iarg];
    if (arg == "-d") {
      debug = true;
    }
    if (arg == "-step=0.5") t_step = 0.5;
    if (arg == "-step=0.25") t_step = 0.25;
    if (arg == "-step=0.1") t_step = 0.1;
    if (arg == "-step=0.01") t_step = 0.01;
  }

  std::vector<double> data;
  double t_min = 0.0;
  double t_max = 100.0;

  size_t n_steps = (size_t) ( (t_max - t_min + 0.1 * t_step) / t_step);
  if (debug) {
    std::cerr << "DEBUG: " << "t_min   = " << t_min << std::endl;
    std::cerr << "DEBUG: " << "t_max   = " << t_max << std::endl;
    std::cerr << "DEBUG: " << "t_step  = " << t_step << std::endl;
    std::cerr << "DEBUG: " << "n_steps = " << n_steps << std::endl;
  }
  double Nyquist_frequency = 0.5 / t_step;
  if (debug) std::cerr << "DEBUG: "
                       << "Nyquist_frequency = "
                       << Nyquist_frequency << std::endl;
  double min_freq_cut = 0.0;
  double max_freq_cut = Nyquist_frequency;
  if (debug) std::cerr << "DEBUG: "
                       << "fft_real::main: argc="
                       << argc_ << std::endl;

  /*
    if (argc_ >= 2) {

    if (debug) std::cerr << "DEBUG: "
    << "fft_real::main: argv[1]='"
    << argv_[1] << "'" << std::endl;
    std::string arg (argv_[1]);
    if (debug) std::cerr << "DEBUG: "
    << "fft_real::main: arg='"
    << arg << "'" << std::endl;
    std::istringstream iss (arg);
    iss >> max_freq_cut;
    if (!iss) {
    th row std::runtime_error ("fft_real::main: Format error!");
    }
    }
  */

  if (debug) std::cerr << "DEBUG: "
                       << "Maximum frequency cut= "
                       << max_freq_cut << std::endl;
  data.assign(n_steps, 0.0);
  for (size_t i = data.size() / 5; i < 2 * data.size() / 4; i++) {
    data[i] = 1.0;
  }
  
  mygsl::fft_real fft;
  fft.init(data, t_min, t_step, min_freq_cut, max_freq_cut);
  fft.process();
  fft.dump(std::cout);
  if (debug) fft.dump(std::cerr, false);
  return 0; 
}
