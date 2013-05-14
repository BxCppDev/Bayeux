// -*- mode: c++; -*-
/* tabulated_function.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2007-12-07
 * Last modified: 2013-04-23
 *
 * License:
 *
 *
 * Description:
 *   Tabulated function
 * History:
 *
 */

#ifndef MYGSL_TABULATED_FUNCTION_H_
#define MYGSL_TABULATED_FUNCTION_H_ 1

#include <iostream>
#include <map>
#include <string>

#include <mygsl/i_unary_function.h>

namespace mygsl {

  class tabulated_function : public i_unary_function {
  public:
    typedef std::map<double,double> points_map_type;

    static const std::string LINEAR_INTERP_NAME;
    static const std::string POLYNOMIAL_INTERP_NAME;
    static const std::string CSPLINE_INTERP_NAME;
    static const std::string CSPLINE_PERIODIC_INTERP_NAME;
    static const std::string AKIMA_INTERP_NAME;
    static const std::string AKIMA_PERIODIC_INTERP_NAME;
    static const std::string DEFAULT_INTERP_NAME;

  public:

    /// Check if the function has an explicit domain of definition (default: false)
    virtual bool has_explicit_domain_of_definition() const;

    /// Check if a value is in the domain of definition of the function (default: true)
    virtual bool is_in_domain_of_definition(double x_) const;

    /// Constructor
    tabulated_function(const std::string& interp_name_ = DEFAULT_INTERP_NAME);

    tabulated_function(const tabulated_function& tab_func_);

    virtual ~tabulated_function();

    tabulated_function& operator=(const tabulated_function& tab_func_);

    bool is_verbose() const;
    void set_verbose(bool v_ = true);
    bool is_valid(double x_) const;

    const std::string& interpolator_name() const;

    const points_map_type& points() const;

    static const std::string& default_interpolator_name();

    static bool interpolator_name_is_valid(const std::string& name_);

    void scale(double s_);

    size_t size() const;

    bool is_table_locked() const;

    void lock_table(const std::string& interp_name_ = "");

    void unlock_table();

    void relock_table(const std::string& interp_name_ = "");

    void add_point(double x_, double y_, bool lock_after_ = false);

    double x_min() const;

    double x_max() const;

    void reset();

    virtual void tabfunc_load(std::istream& in_, void * context_ = 0);

    virtual void tabfunc_store(std::ostream& out_, void * context_ = 0) const;

    void print_points(std::ostream& out_,
                      const std::string& header_comment_ = "",
                      const std::string& footer_comment_ = "") const;
  protected:

    virtual double _eval(double x_) const;

  private:
    struct tabfunc_impl;
    tabfunc_impl *pImpl;
  };

} // end of namespace mygsl

#endif // MYGSL_TABULATED_FUNCTION_H_

// end of tabulated_function.h
