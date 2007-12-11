// demo_fft_real.cxx

#include <iostream>
#include <fstream>
#include <vector>

#include <mygsl/fft_real.h>

int main( int argc_ , char ** argv_ )
{

  double t_min  = 0.0; // time start
  double t_step = 1.0; // sampling period

  std::vector<double> data; // sampled functions
  
  // read sampled signal from a file:
  std::ifstream f_data("demo_signal.samples");
  while( f_data ) {
    double t, signal;
    f_data >> t >> signal >> std::ws;
    data.push_back(signal);
  }

  // print sampled signal function:
  std::cout << "# sampled signal function:" <<  std::endl;
  for ( size_t i=0; i<data.size(); i++ ) {
    std::cout <<  t_min+i*t_step << ' ' << data[i] << std::endl;
  } 
  std::cout << std::endl;
  std::cout << std::endl;

  // upper limit of bandwidth:
  double Nyquist_frequency = 0.5/t_step;

  // bandwidth filter:
  double min_freq_cut=0.0;
  //double max_freq_cut=0.1*Nyquist_frequency;
  double max_freq_cut=Nyquist_frequency;

  mygsl::fft_real fft;
  fft.init(data,t_min,t_step,min_freq_cut,max_freq_cut);
  fft.process();
  fft.dump(std::cerr,false);


  // print filtered function:
  std::vector<double> filtered_data; 
  fft.compute_filtered_data(filtered_data);

  std::cout << "# filtered function:" <<  std::endl;
  for ( size_t i=0; i<filtered_data.size(); i++ ) {
    std::cout <<  t_min+i*t_step << ' ' << filtered_data[i] << std::endl;
  } 
  std::cout << std::endl;
  std::cout << std::endl;


  // print fourier_spectrum:
  std::vector<double> fourier_spectrum; 
  fft.compute_fourier_spectrum(fourier_spectrum);

  std::cout << "# fourier spectrum:" <<  std::endl;
  for ( size_t i=0; i<fourier_spectrum.size(); i++ ) {
    std::cout <<  i*fft.get_frequency_step() << ' ' << fourier_spectrum[i] << std::endl;
  } 
  std::cout << std::endl;
  std::cout << std::endl;

  return(0);
}

// end of demo_fft_real.cxx
