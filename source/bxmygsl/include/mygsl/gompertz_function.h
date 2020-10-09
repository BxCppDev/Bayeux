#ifndef MYGSL_GOMPERTZ_FUNCTION_H
#define MYGSL_GOMPERTZ_FUNCTION_H

// Third party:
// - Bayeux/datatools :
#include <datatools/i_cloneable.h>

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief Gompertz function
  class gompertz_function
    : public i_unary_function,
      public datatools::i_cloneable
  {
  public:

    //! Default constructor
    gompertz_function();

    //! Default constructor
    gompertz_function(double asymptote_,
                      double displacement_,
                      double growth_rate_);

    //! Destructor
    ~gompertz_function() override;

    //! Set asymptote
    void set_asymptote(double asymptote_);

    //! Return asymptote
    double get_asymptote() const;

    //! Set displacement
    void set_displacement(double displacement_);

    //! Return displacement
    double get_displacement() const;

    //! Set growth rate
    void set_growth_rate(double growth_rate_);

    //! Return growth rate
    double get_growth_rate() const;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the function
    void reset() override;

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

    double _asymptote_;    ///< Asymptote
    double _displacement_; ///< x displacement
    double _growth_rate_;  ///< Growth rate

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(gompertz_function)

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(gompertz_function)

  };

} // namespace mygsl

#endif // MYGSL_GOMPERTZ_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
