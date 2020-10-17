/// \file mygsl/polynomial.h

#ifndef MYGSL_POLYNOMIAL_H
#define MYGSL_POLYNOMIAL_H 1

// Standard library:
#include <vector>

// Third party:
// - Bayeux/datatools :
#include <datatools/i_serializable.h>
#include <datatools/i_cloneable.h>

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief A real polynomial of arbitrary degree
  class polynomial :
    public i_unary_function,
    public datatools::i_serializable,
    public datatools::i_cloneable
  {
  public:

    //! Check validity of the polynomial
    bool is_valid() const;

    //! Default constructor
    polynomial();

    //! Constructor
    explicit polynomial(unsigned int sz_);

    //! Constructor
    explicit polynomial(double c0_);

    //! Constructor
    polynomial(double c0_, double c1_);

    //! Constructor
    polynomial(double c0_, double c1_, double c2_);

    //! Constructor
    polynomial(const std::vector<double>& c_);

    //! Constructor
    polynomial(const polynomial& p_);

    //! Destructor
    ~polynomial() override;

    void set_coefficients(const std::vector<double>& c_);

    const std::vector<double> & get_coefficients() const;

    unsigned int get_degree() const;

    unsigned int get_ncoeffs() const;

    double get_coeff(unsigned int i_) const;

    void set_coeff(unsigned int i_, double coeff_);

    void print(std::ostream& out_ = std::clog,
               int format_ = 0,
               bool eol_ = false) const;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the polynomial
    void reset() override;

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

    /** Given three doublets (x1, y1), (x2, y2), (x3, y3), compute the
     *  coefficients of the polynomial of degree 2 (parabola) that matches
     *  the points:
     *
     *    P(x) = p0 + p1 x^1 + p2 x^2
     */
    static bool compute_parabola(double x1_, double x2_, double x3_,
                                 double y1_, double y2_, double y3_,
                                 double & p2_, double & p1_, double & p0_);

    /** Given two doublets (x1, y1), (x2, y2), compute the
     *  coefficients of the polynomial of degree 1 (line) that matches
     *  the points:
     *
     *    P(x) = p0 + p1 x^1
     */
    static bool compute_line(double x1_, double x2_,
                             double y1_, double y2_,
                             double & p1_, double & p0_);

    bool make_line(double x1_, double x2_,
                   double y1_, double y2_);

    bool make_parabola(double x1_, double x2_, double x3_,
                       double y1_, double y2_, double y3_);

    //! Smart printing
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  protected :

    double _eval(double x_) const override;

  public:

    class solver {
    public:
      solver(unsigned int sz_ = 0);
      virtual ~solver();
      bool is_initialized() const;
      bool solve(const polynomial& p_);
      bool is_success() const;
      size_t fetch_solutions(std::vector<std::complex<double> > & zsols_);
      size_t fetch_real_solutions(std::vector<double> & rsols_, double precision_ = -1.0);

    private:
      void _init_(unsigned int);
      void _reset_();

    private:
      struct solver_impl;
      solver_impl *pImpl;
    };

  private:

    std::vector<double> _c_; //!< Coefficients of the polynomial

    DATATOOLS_SERIALIZATION_DECLARATION()

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(polynomial)

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(polynomial)

  };

  typedef polynomial polynomial_function;

} // namespace mygsl

#endif // MYGSL_POLYNOMIAL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
