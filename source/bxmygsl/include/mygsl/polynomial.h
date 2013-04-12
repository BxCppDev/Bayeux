// polynomial.h

#ifndef MYGSL_POLYNOMIAL_H_ 
#define MYGSL_POLYNOMIAL_H_ 1

#include <vector>

// Serialization interfaces :
#include <datatools/i_serializable.h>
#include <mygsl/i_unary_function.h>

namespace mygsl {

  /// \brief A real polynomial of arbitrary degree
  class polynomial : 
    public i_unary_function,
    DATATOOLS_SERIALIZABLE_CLASS {
  public:
    
    bool is_valid() const;

    polynomial();

    explicit polynomial(unsigned int sz_);

    explicit polynomial(double c0_);

    polynomial(double c0_, double c1_);

    polynomial(double c0_, double c1_, double c2_);

    polynomial(const std::vector<double>& c_);

    polynomial(const polynomial& p_);

    virtual ~polynomial();

    void set_coefficients(const std::vector<double>& c_);

    unsigned int get_degree() const;

    unsigned int get_ncoeffs() const;
    double get_coeff(unsigned int i_) const;
    void set_coeff(unsigned int i_, double c_);
  
 
    void print (std::ostream& out_ = std::clog, 
		int format_ = 0, 
		bool eol_ = false) const;

    /*
     * L(x) = p0 + p1 * x^1
     *
     */
    static bool solve_linear(double p0_, double p1_, unsigned int& nsols_,
			     double& x0_);

    /*
     * Q(x) = p0 + p1 * x^1 + p2 * x^2
     *
     */
    static bool solve_quadratic(double p0_, double p1_, double p2_, 
				unsigned int & nsols_, double & x0_, double & x1_);

    /*
     * C(x) =  p0 + p1 * x^1 + p2 * x^2 + x^3
     *
     */
    static bool solve_cubic(double p0_, double p1_, double p2_, 
			    unsigned int & nsols_, 
			    double & x0_, double & x1_, double & x2_);

    /*
     * C(x) = p0 + p1 * x^1 + p2 * x^2 + p3 * x^3
     *
     */
    static bool solve_cubic(double p0_, double p1_, double p2_, double p3_,
			    unsigned int & nsols_,
			    double & x0_, double & x1_, double & x2_);
 
  protected :

    double _eval(double x_) const;

  public:

    class solver {
    public:
      solver(unsigned int sz_ = 0);
      virtual ~solver();
      bool solve(const polynomial& p_);
 
    private:
      void _init_(unsigned int);
      void _reset_();

    private:
      struct solver_impl;
      solver_impl *pImpl;
    };

  private:
    std::vector<double> _c_; /// Coefficients of the polynomial

    DATATOOLS_SERIALIZATION_DECLARATION();
  };
}

#endif // MYGSL_POLYNOMIAL_H_

