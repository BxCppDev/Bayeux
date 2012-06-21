// -*- mode: c++; -*- 
/* histos.cc
 *  
 */
// Ourself
#include <datatools/utils/histos.h>

// Standard Library
#include <cstdlib>
#include <cmath>
#include <string>
#include <exception>

// Third Party
// - A

// This Project

namespace datatools {

namespace utils {

histos::histos() : debug_(false) {}


histos::histos(bool debug) : debug_(debug) {}


histos::~histos() {}


bool histos::get_debug() const {
  return debug_;
}


double histos::get_x_min() const {
  return x_min_;
}


double histos::get_x_max() const {
  return x_max_;
}


int histos::get_x_step() const {
  return x_step_;
}


double histos::get_y_min() const {
  return y_min_;
}


double histos::get_y_max() const {
  return y_max_;
}


int histos::get_y_step() const {
  return y_step_;
}


void histos::set_debug(bool status) {
  debug_ = status;
}


void histos::set_x_min(double x) {
  x_min_ = x;
}


void histos::set_x_max(double x) {
  x_max_ = x;
}


void histos::set_x_step(int steps) {
  x_step_ = steps;
}


void histos::set_y_min(double y) {
  y_min_ = y;
}


void histos::set_y_max(double y) {
  y_max_ = y;
}


void histos::set_y_step(int steps) {
  y_step_ = steps;
}


double histos::get_z(int i, int j) {
  std::vector<double> z_values;
  this->get_z_values(z_values);
  return z_values.at(i * this->get_y_step() + j);
}


void histos::set_z( int i, int j, double val) {
  histo_3d_[i][j] = val;
}


void histos::set_x_range(double from, double to) {
  this->set_x_min(from);
  this->set_x_max(to);
}


void histos::set_y_range(double from, double to) {
  this->set_y_min(from);
  this->set_y_max(to);
}


void histos::set_x_range(double from, double to, int steps) {
  this->set_x_range(from, to);
  this->set_x_step(steps);
}


void histos::set_y_range(double from, double to, int steps) {
  this->set_y_range(from, to);
  this->set_y_step(steps);
}


void histos::init() {
  histo_3d_.resize(x_step_);

  for (size_t i = 0; i < histo_3d_.size(); ++i) {
    histo_3d_[i].resize(y_step_);

    for (size_t j = 0; j < histo_3d_[i].size(); ++j) {
      histo_3d_[i][j] = 0;
    }
  }
}


void histos::build( double x, double y, double weight) {
  double x_step_size = (x_max_ - x_min_) / (1.0*x_step_);
  double y_step_size = (y_max_ - y_min_) / (1.0*y_step_);

  for (int i = 0; i < x_step_; ++i) {
    for (int j = 0; j < y_step_; ++j) {
      if (x    >  (x_min_ + (double)(i)   * x_step_size )
          && x <= (x_min_ + (double)(i+1) * x_step_size )
          && y >  (y_min_ + (double)(j)   * y_step_size )
          && y <= (y_min_ + (double)(j+1) * y_step_size ) ) {
        histo_3d_[i][j] += weight;
      }
    }
  }
}


void histos::do_ratio(histos& other, double n, double m) {
  if (x_min_  == other.get_x_min()  &&
      x_max_  == other.get_x_max()  &&
      x_step_ == other.get_x_step() &&
      y_min_  == other.get_y_min()  &&
      y_max_  == other.get_y_max()  &&
      y_step_ == other.get_y_step()) {
    for (int i = 0; i < x_step_; ++i) {
      for (int j = 0; j < y_step_; ++j) {
        if (other.get_z(i, j) != 0.) {
          histo_3d_[i][j] /= other.get_z(i, j) * m / n ;
        } else {
          histo_3d_[i][j] = -1.;
        }
      }
    }
  }
}


void histos::x_normalize() {
  for (int i = 0; i < x_step_; ++i) {
    double i_sum = 0;

    for (int j = 0; j < y_step_; ++j) i_sum += histo_3d_[i][j];

    for (int j = 0; j < y_step_; ++j) {
      if (histo_3d_[i][j] != 0) histo_3d_[i][j] /= i_sum;
    }
  }
}


void histos::do_ratio_sqrt(histos& other, double n, double m) {
  if (x_min_  == other.get_x_min()  &&
      x_max_  == other.get_x_max()  &&
      x_step_ == other.get_x_step() &&
      y_min_  == other.get_y_min()  &&
      y_max_  == other.get_y_max()  &&
      y_step_ == other.get_y_step()) {
    for (size_t i = 0; i < x_step_; ++i) {
      for (size_t j = 0; j < y_step_; ++j) {
        if (other.get_z(i, j) != 0.) {
          histo_3d_[i][j] /= (sqrt(other.get_z(i, j) * m) / n);
        } else {
          histo_3d_[i][j] = -1.;
        }
      }
    }
  }
}


void histos::compute_significance(histos& other, double n, double m) {
  if (x_min_  == other.get_x_min()  &&
      x_max_  == other.get_x_max()  &&
      x_step_ == other.get_x_step() &&
      y_min_  == other.get_y_min()  &&
      y_max_  == other.get_y_max()  &&
      y_step_ == other.get_y_step()) {
    for (size_t i = 0; i < x_step_; ++i) {
      for (size_t j = 0; j < y_step_; ++j) {
        if (other.get_z(i, j) != 0. ) {
          double current_val = histo_3d_[i][j];
          histo_3d_[i][j] /= (sqrt(other.get_z(i, j)*m + current_val*n) / n);
        } else {
          histo_3d_[i][j] = -1.;
        }
      }
    }
  }
}


void histos::y_normalize() {
  for (int j = 0; j < y_step_; ++j)
  {
    double j_sum(0);

    for (int i = 0; i < x_step_; ++i) j_sum += histo_3d_[i][j];

    for (int i = 0; i < x_step_; ++i) {
      if (histo_3d_[i][j] != 0) histo_3d_[i][j] /= j_sum;
    }
  }
}


void histos::get_x_values(std::vector<double>& output)
{
  std::vector<double> x_values;
  double x_step_size = (x_max_ - x_min_) / (1.0*x_step_);

  for (int i = 0; i < x_step_; ++i) {
    for (int j = 0; j < y_step_; ++j) {
      x_values.push_back(x_min_ + (1.0*i) * x_step_size);
    }
  }
  output = x_values;
}


void histos::get_y_values(std::vector<double>& output) {
  std::vector<double> y_values;
  double y_step_size = (y_max_ - y_min_) / (1.0*y_step_);

  for (int i = 0; i < x_step_; ++i) {
    for (int j = 0; j < y_step_; ++j) {
      y_values.push_back(y_min_ + (1.0*j) * y_step_size);
    }
  }
  output = y_values;
}  


void histos::get_z_values(std::vector<double>& output) {
  std::vector<double> z_values;

  for (int i = 0; i < x_step_; ++i)
    for (int j = 0; j < y_step_; ++j)
      z_values.push_back(histo_3d_[i][j]);
  output = z_values;
}


void histos::print(std::ostream& out) {
  double x_step_size = (x_max_ - x_min_) / (1.0*x_step_);
  double y_step_size = (y_max_ - y_min_) / (1.0*y_step_);

  out << "# Result" << std::endl;

  if (debug_) {
    std::cerr << std::endl << "histos::print: DEBUG: Result : " << std::endl;
  }

  for (int i = 0; i < x_step_; ++i) {
    for (int j = 0; j < y_step_; ++j) {
      out << (x_min_ + 1.0*i * x_step_size) << " "
          << (y_min_ + 1.0*j * y_step_size) << " "
          << histo_3d_[i][j]
          << std::endl;

      if (debug_) std::cerr << histo_3d_[i][j] << '\t';
    }
    if (debug_ ) std::cerr << std::endl;
  }
}


void histos::print2(std::ostream& out) {
  double x_step_size = (x_max_ - x_min_) / (1.0*x_step_);
  double y_step_size = (y_max_ - y_min_) / (1.0*y_step_);

  out << "# Result"
      << std::endl;

  for (int i = 0; i < x_step_; ++i) {
    for (int j = 0; j < y_step_; ++j) {
      out << (x_min_ + (double)(i) * x_step_size) << " "
          << (y_min_ + (double)(j) * y_step_size) << " "
          << histo_3d_[i][j]
          << std::endl
          << (x_min_ + (double)(i) * x_step_size) << " "
          << (y_min_ + ((double)(j)+1.) * y_step_size) << " "
          << histo_3d_[i][j]
          << std::endl;
    }
    out << std::endl;
    for (int j = 0; j < y_step_; ++j) {
      out << (x_min_ + ((double)(i)+1.) * x_step_size) << " "
          << (y_min_ + (double)(j) * y_step_size) << " "
          << histo_3d_[i][j]
          << std::endl
          << (x_min_ + ((double)(i)+1.) * x_step_size) << " "
          << (y_min_ + ((double)(j)+1.) * y_step_size) << " "
          << histo_3d_[i][j]
          << std::endl;
    }
    out << std::endl;
  }
}


void histos::help(std::ostream& out, std::string file) {
  std::string out_txt = "histo_3d.his";
  if (file != "") out_txt = file;

  out << std::endl
      << "** histos: gnuplot example: **" << std::endl
      << "------------------------------" << std::endl;
  out << "set autoscale"  << std::endl
      << "set xlabel 'x'" << std::endl
      << "set ylabel 'y'" << std::endl
      << "set zlabel 'N'" << std::endl
      << "set pm3d"       << std::endl
      << "set palette rgbformulae -31, -36, -32" << std::endl
      << "set view 0, 0"  << std::endl
      << "unset surface"  << std::endl
      << "set dgrid3d " << y_step_ << "," << x_step_ << ",2" << std::endl
      << "splot [" << x_min_ << ":" << x_max_ << "]"
      << "[" << y_min_ << ":" << y_max_ << "][0:*] '" << out_txt
      << "' u 1:2:3 with dots title 'histogram 3d'"
      << std::endl << std::endl;
}


void histos::help2(std::ostream& out, std::string file) {
  std::string out_txt = "histo_3d.his";
  if (file != "") out_txt = file;

  out << std::endl
      << "** histos: gnuplot example: **" << std::endl
      << "------------------------------" << std::endl;
  out << "set autoscale"  << std::endl
      << "set xlabel 'x'" << std::endl
      << "set ylabel 'y'" << std::endl
      << "set zlabel 'N'" << std::endl
      << "set hidden3d"       << std::endl
      << "splot [" << x_min_ << ":" << x_max_ << "]"
      << "[" << y_min_ << ":" << y_max_ << "][0:*] '" << out_txt
      << "' u 1:2:3 with lines title 'histogram 3d'"
      << std::endl << std::endl;
}

} // end of namespace utils
} // end of namespace datatools

