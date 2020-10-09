/// \file mygsl/tabulated_function.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2007-12-07
 * Last modified: 2018-12-23
 *
 * License: GPL3
 *
 * Description:
 *
 *   Tabulated function
 *
 */

#ifndef MYGSL_TABULATED_FUNCTION_H
#define MYGSL_TABULATED_FUNCTION_H 1

// Standard library:
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Third party:
// - Bayeux/datatools :
#include <datatools/i_cloneable.h>
#include <datatools/serialization_macros.h>

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  /// \brief Tabulated function
  class tabulated_function
    : public i_unary_function
    , public datatools::i_cloneable
  {
  public:
    typedef std::map<double,double> points_map_type;

    static const std::string & linear_interp_name();
    static const std::string & polynomial_interp_name();
    static const std::string & cspline_interp_name();
    static const std::string & cspline_periodic_interp_name();
    static const std::string & akima_interp_name();
    static const std::string & akima_periodic_interp_name();
    static const std::string & steffen_interp_name();
    static const std::string & default_interp_name();

  public:

    /// Check if the function has an explicit domain of definition (default: false)
    bool has_explicit_domain_of_definition() const override;

    /// Check if a value is in the domain of definition of the function (default: true)
    bool is_in_domain_of_definition(double x_) const override;

    /// Default constructor
    tabulated_function(const std::string & interp_name_ = "");

    /// Copy constructor
    tabulated_function(const tabulated_function & tab_func_);

    /// Destructor
    ~tabulated_function() override;

    /// Assignement
    tabulated_function & operator=(const tabulated_function& tab_func_);

    /// Check verbosity flag
    bool is_verbose() const;

    /// Set verbosity flag
    void set_verbose(bool v_ = true);

    /// Check validity
    bool is_valid(double x_) const;

    /// Return the name of the interpolation algorithm (GSL)
    const std::string & interpolator_name() const;

    /// Return the collection of points
    const points_map_type & points() const;

    /// Export to vectors of doubles
    bool export_to_vectors(std::vector<double> & keys_,
                           std::vector<double> & values_) const;

    /// Return the name of the default interpolation algorithm (GSL)
    static const std::string & default_interpolator_name();

    /// Check the validity of the name of the interpolation algorithm (GSL)
    static bool interpolator_name_is_valid(const std::string & name_);

    /// Scale the function by a given factor
    void scale(double s_);

    /// Return the size of the collection of points
    size_t size() const;

    /// Check the table lock flag
    bool is_table_locked() const;

    /// Lock the table using a given interpolation algorithm (GSL)
    void lock_table(const std::string & interp_name_ = "");

    /// Unlock the table
    void unlock_table();

    /// Relock the table using a given interpolation algorithm (GSL)
    void relock_table(const std::string & interp_name_ = "");

    /// Add a point to the collection
    void add_point(double x_, double y_, bool lock_after_ = false);

    /// Return the minimum x value from the collection of points
    double x_min() const;

    /// Return the maximum x value from the collection of points
    double x_max() const;

    /// Load the collection of points from a file
    void load_from_file(const std::string & filename_, uint32_t options_ = 0);

    /// Load from an input stream
    virtual void tabfunc_load(std::istream & in_, void * context_ = 0);

    /// Store to an output stream
    virtual void tabfunc_store(std::ostream & out_, void * context_ = 0) const;

    /// Print points
    void print_points(std::ostream & out_,
                      const std::string & header_comment_ = "",
                      const std::string & footer_comment_ = "") const;


    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the functor
    void reset() override;

    //! Smart printing
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  protected:

    //! Evaluation
    double _eval(double x_) const override;

  private:

    struct tabfunc_impl;
    tabfunc_impl * pImpl = nullptr; ///< Private implementation (hide GSL stuff)

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(tabulated_function)

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(tabulated_function)
        
    BOOST_SERIALIZATION_BASIC_DECLARATION()

  };

} // end of namespace mygsl

#endif // MYGSL_TABULATED_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
