// -*- mode: c++; -*- 
/* histos.cc
 * Author(s):     
 *   Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 *                
 * Creation date: 2009-04-29
 * Last modified: 2011-10-29
 * 
 * License: 
 * 
 * Description: 
 * 
 * Construct 3d histograms.
 *
 *
 * History: 
 * 
 */
#ifndef DATATOOLS_UTILS_HISTOS_H_
#define DATATOOLS_UTILS_HISTOS_H_
// Standard Library
#include <iostream>
#include <vector>

// Third Party
// - A

// This Project

namespace datatools {
namespace utils {

class histos {
 public:
  // ctor
  histos();
  explicit histos(bool );

  // dtor
  virtual ~histos();

  // get
  bool   get_debug() const;
  double get_x_min() const;
  double get_x_max() const;
  int    get_x_step() const;
  double get_y_min() const;
  double get_y_max() const;
  int    get_y_step() const;

  // set
  void set_debug(bool status);
  void set_x_min(double x);
  void set_x_max(double x);
  void set_x_step(int steps);
  void set_y_min(double y);
  void set_y_max(double y);
  void set_y_step(int steps);

  void set_x_range(double from, double to);
  void set_y_range(double from, double to);
  void set_x_range(double from, double to, int steps);
  void set_y_range(double from, double to, int steps);

  void init();
  void build(double x, double y, double weight = 1.);

  void x_normalize();
  void y_normalize();

  void do_ratio(histos& other, double n = 1., double m = 1.);
  void do_ratio_sqrt(histos& other, double n = 1., double m = 1.);
  void compute_significance(histos& other, double n = 1., double m = 1.);

  void print(std::ostream& out);
  void help(std::ostream& out, std::string file = "");
  void print2(std::ostream& out);
  void help2(std::ostream& out, std::string file = "" );

  void get_x_values(std::vector<double>& output);
  void get_y_values(std::vector<double>& output);
  void get_z_values(std::vector<double>& output);

  double get_z(int i, int j);
  void set_z(int i, int j, double val);


 private:
  bool debug_;
  double x_min_; 
  double x_max_;
  int x_step_;
  double y_min_; 
  double y_max_;
  int y_step_;

  // Need a typedef!!!
  std::vector<std::vector<double> > histo_3d_;
};

} // end of namespace utils
} // end of namespace datatools

#endif // DATATOOLS_UTILS_HISTOS_H_

