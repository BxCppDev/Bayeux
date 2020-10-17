#ifndef MYGSL_HEAVISIDE_FUNCTION_H
#define MYGSL_HEAVISIDE_FUNCTION_H

// Third party:
// - Bayeux/datatools :
#include <datatools/i_cloneable.h>

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  class heaviside_function
    : public i_unary_function,
      public datatools::i_cloneable
  {
  public:

    enum convention_type {
      CONVENTION_INVALID          = 0,
      CONVENTION_HALF_MAXIMUM     = 1,
      CONVENTION_RIGHT_CONTINUOUS = 2,
      CONVENTION_LEFT_CONTINUOUS  = 3,
      CONVENTION_CONTINUOUS       = 4
    };

    //! Default constructor
    heaviside_function(convention_type convention_ = CONVENTION_INVALID);

    //! Destructor
    ~heaviside_function() override;

    //! Set the convention
    void set_convention(convention_type convention_);

    //! Return the convention
    convention_type get_convention() const;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the function
    void reset() override;

    //! The minimum bound of the non-zero domain (default is plus infinity)
    double get_non_zero_domain_min() const override;

    //! Smart printing
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  protected:

    //! Evaluation
    double _eval(double x_) const override;

    //! Set default attributes values
    void _set_defaults();

  private:

    convention_type _convention_ = CONVENTION_HALF_MAXIMUM;

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(heaviside_function)

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(heaviside_function)

  };

} // namespace mygsl

#endif // MYGSL_HEAVISIDE_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
