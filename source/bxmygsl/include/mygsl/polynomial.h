// mygsl::polynomial.h

#ifndef __mygsl__polynomial_h 
#define __mygsl__polynomial_h 1

#include <vector>

#include <mygsl/unary_eval.h>

namespace mygsl {
class polynomial : public unary_eval {
 public:
  polynomial(size_t sz_);

  polynomial(double c0_);

  polynomial(double c0_, double c1_);

  polynomial(double c0_, double c1_, double c2_);

  polynomial(const std::vector<double>& c_);

  polynomial(const polynomial& p_);

  virtual ~polynomial();

  size_t get_degree() const;

  size_t get_ncoeffs() const;
  double get_coeff(size_t i_) const;
  void set_coeff(size_t i_, double c_);

  double eval(double x_) const;

  void print (std::ostream& out_ = std::clog, 
              int format_ = 0, 
              bool eol_ = false) const;

  /*
   * L(x) = p0 + p1 * x^1
   *
   */
  static bool solve_linear(double p0_, double p1_, size_t& nsols_,
                           double& x0_);

  /*
   * Q(x) = p0 + p1 * x^1 + p2 * x^2
   *
   */
  static bool solve_quadratic(double p0_, double p1_, double p2_, 
                              size_t & nsols_, double & x0_, double & x1_);

  /*
   * C(x) =  p0 + p1 * x^1 + p2 * x^2 + x^3
   *
   */
  static bool solve_cubic(double p0_, double p1_, double p2_, 
                          size_t & nsols_, 
                          double & x0_, double & x1_, double & x2_);

  /*
   * C(x) = p0 + p1 * x^1 + p2 * x^2 + p3 * x^3
   *
   */
  static bool solve_cubic(double p0_, double p1_, double p2_, double p3_,
                          size_t & nsols_,
                          double & x0_, double & x1_, double & x2_);

  class solver {
   public:
    solver(size_t sz_ = 0);
    virtual ~solver();
    bool solve(const polynomial& p_);
 
   private:
    void _init_(size_t);
    void _reset_();

   private:
    struct solver_impl;
    solver_impl *pImpl;
  };

 private:
  size_t         _size_;
  std::vector<double> _c_;
};
}

#endif // __mygsl__polynomial_h

