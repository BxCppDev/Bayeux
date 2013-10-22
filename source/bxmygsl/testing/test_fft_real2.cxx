// test_fft_real2.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include <datatools/utils.h>

#include <mygsl/fft_real.h>

int main(int argc_ , char ** argv_)
{
  long seed = 12345;

  double t_min  = 0.0; // time start (ns)
  double t_step = 1.0; // sampling period (ns)

  srand48(seed);

  std::vector<double> data; // sampled functions (mV)
  data.reserve(250);

  // read sampled signal from a file:
  double adc_shift = 56.; // (mV)
  std::string fname = "${MYGSL_TESTING_DIR}/config/demo_signal.samples";
  datatools::fetch_path_with_env(fname);
  std::ifstream f_data (fname.c_str());
  double peak = +1.e10; // trick
  while(f_data)
    {
      double t, signal;
      f_data >> t >> signal >> std::ws;
      double adc = signal + adc_shift;
      double noise = -3. + 6. * drand48();
      adc += noise;
      if (adc < peak) peak = adc;
      data.push_back(adc);
    }

  std::cerr << "DEBUG: peak=" << peak <<  std::endl;
  // normalize the signal using peak value and make it positive:
  for (size_t i = 0; i < data.size(); i++)
    {
      data[i] /= peak;
    }

  // print sampled signal function:
  std::cout << "# sampled signal function:" <<  std::endl;
  for (size_t i = 0; i < data.size(); i++)
    {
      std::cout <<  t_min + i * t_step << ' ' << data[i] << std::endl;
    }
  std::cout << std::endl;
  std::cout << std::endl;

  // upper limit of bandwidth:
  double Nyquist_frequency = 0.5 / t_step; // GHz

  // low frequency filter:
  double min_freq_cut = 0.0; // GHz
  double max_freq_cut = Nyquist_frequency; // GHz
  max_freq_cut *= 0.1;

  std::cout << "# data.size = " << data.size() << std::endl;
  mygsl::fft_real fft;
  fft.init(data,t_min,t_step,min_freq_cut,max_freq_cut);
  fft.process();
  fft.dump(std::cerr,false);
  return 0;

  // print filtered function:
  std::vector<double> filtered_data;
  fft.compute_filtered_data(filtered_data);

  std::cout << "# filtered function:" <<  std::endl;
  for (size_t i= 0; i < filtered_data.size(); i++)
    {
      std::cout <<  t_min+i*t_step << ' ' << filtered_data[i] << std::endl;
    }
  std::cout << std::endl;
  std::cout << std::endl;

  // print Fourier spectrum:
  std::vector<double> fourier_spectrum;
  fft.compute_fourier_spectrum(fourier_spectrum);

  std::cout << "# Fourier spectrum:" <<  std::endl;
  for (size_t i = 0; i < fourier_spectrum.size(); i++)
    {
      std::cout <<  i*fft.get_frequency_step() << ' ' << fourier_spectrum[i] << std::endl;
    }
  std::cout << std::endl;
  std::cout << std::endl;

  return(0);
}

// end of test_fft_real2.cxx
