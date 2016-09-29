#ifndef MYGSL_RECTANGULAR_FUNCTION_H
#define MYGSL_RECTANGULAR_FUNCTION_H

// Third party:
// - Bayeux/datatools :
#include <datatools/i_cloneable.h>

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief Rectangular function (unit pulse)
  //!
  //!  @code
  //!                   ^
  //!                  1:
  //!             +-----:-----+
  //!             |     :     |
  //!             |     :     |
  //!             |     :     |
  //!  -----------+.....+.....+------------> x
  //!           -1/2   O:   +1/2
  //!                   :
  //!  @endcode
  class rectangular_function
    : public i_unary_function,
      public datatools::i_cloneable
  {
  public:

    enum convention_type {
      CONVENTION_INVALID          = 0,
      CONVENTION_HALF_MAXIMUM     = 1,
      CONVENTION_INNER_CONTINUOUS = 2,
      CONVENTION_OUTER_CONTINUOUS = 3,
      CONVENTION_CONTINUOUS       = 4
    };

    //! Default constructor
    rectangular_function(convention_type convention_ = CONVENTION_INVALID);

    //! Destructor
    virtual ~rectangular_function();

    //! Set the convention
    void set_convention(convention_type convention_);

    //! Return the convention
    convention_type get_convention() const;

    //! Check initialization status
    virtual bool is_initialized() const;

    //! Initialization from a container of parameters and a dictionary of functors
    virtual void initialize(const datatools::properties & config_,
                            unary_function_dict_type & functors_);

    //! Reset the function
    virtual void reset();

    //! The minimum bound of the non-zero domain (default is plus infinity)
    double get_non_zero_domain_min() const;

    //! The maximum bound of the non-zero domain (default is plus infinity)
    double get_non_zero_domain_max() const;

    //! Smart printing
    virtual void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

  protected:

    //! Evaluation
    double _eval(double x_) const;

    //! Set default attributes values
    void _set_defaults();

  private:

    convention_type _convention_ = CONVENTION_HALF_MAXIMUM; //!< Convention used at edges

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(rectangular_function)

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(rectangular_function)

  };

} // namespace mygsl

#endif // MYGSL_RECTANGULAR_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
