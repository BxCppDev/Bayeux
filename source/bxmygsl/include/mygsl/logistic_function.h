#ifndef MYGSL_LOGISTIC_FUNCTION_H
#define MYGSL_LOGISTIC_FUNCTION_H

// Third party:
// - Bayeux/datatools :
#include <datatools/i_cloneable.h>

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief Logistic function
  class logistic_function
    : public i_unary_function,
      public datatools::i_cloneable
  {
  public:

    //! Default constructor
    logistic_function();

    //! Constructor
    logistic_function(double steepness_, double midpoint_, double amplitude_);

    //! Destructor
    ~logistic_function() override;

    //! Set the steepness
    void set_steepness(double steepness_);

    //! Return the steepness
    double get_steepness() const;

    //! Set the midpoint
    void set_midpoint(double midpoint_);

    //! Return the midpoint
    double get_midpoint() const;

    //! Set the amplitude
    void set_amplitude(double amplitude_);

    //! Return the amplitude
    double get_amplitude() const;

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

    double _steepness_; ///< Steepness of the curve
    double _midpoint_;  ///< x value of the midpoint
    double _amplitude_; ///< maximum value

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(logistic_function)

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(logistic_function)

  };

} // namespace mygsl

#endif // MYGSL_LOGISTIC_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
