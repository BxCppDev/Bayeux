//! \file my/algo.h Header file for the my::algo configurable class

#ifndef MY_ALGO_H
#define MY_ALGO_H

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools::

//! The Bayeux/datatools library top-level namespace
namespace datatools {
  // Forward declaration:
  class properties;
}

//! This is a tutorial for adding OCD support to a class
//!
//! This tutorial illustrates the programming techniques used to add \a object \a configuration \a description (OCD)
//! support to a class, using dedicated macros available from the Bayeux/datatools library.
//!
//! A simple class my::algo is provided with its my::algo::initialize method
//! which is dedicated to the initialization of any instance of type my::algo
//! through a container of configuration parameters (see the \b datatools::properties class
//! from the Bayeux/datatools library). In this example, only four parameters of various type
//! are used to configure any instance of the my::algo type:
//!  - \b debug : a boolean flag (with default value set to \a false),
//!  - \b max_nloops : a positive integer (with default value set to \a 100),
//!  - \b epsilon : a positive dimensionless real number (without default value),
//!  - \b output_filename : the name of an optional output data file (with default value
//!       set to the \a empty string).
//!
//! The process implies adding specific code at two levels:
//!  1. From the header file my/algo.h (declaration of the class):
//!
//!     The DOCD_CLASS_DECLARATION(my::algo) macro
//!     (from the \b datatools/ocd_macros.h header file) is used to declare the OCD support for
//!     the my::algo class,
//!  2. From the source file my/algo.cc (definition of the class):
//!
//!     The DOCD_CLASS_IMPLEMENT_LOAD_BEGIN() and
//!     DOCD_CLASS_IMPLEMENT_LOAD_END() macros define a block of instructions
//!     that describes the class and its configuration parameters (name, type, default value...),
//!     filling a \b datatools::object_configuration_description object
//!     (from the \b datatools/object_configuration_description.h header).
//!
//!     Finally, the DOCD_CLASS_SYSTEM_REGISTRATION()
//!     macro is used to register the OCD support data in the Bayeux OCD
//!     system registry.

//! \brief User namespace for the algo class
namespace my {

  //! \brief A simple configurable algorithm class
  class algo
  {
  public:

    //! Default constructor
    /** The algorithm object is constructed with its status set to \a not-initialized.
     */
    algo();

    //! Main initialization from a container of configuration parameters
    /** The algorithm object can be initialized only if its status is \a not-initialized.
     */
    void initialize(const datatools::properties & config_);

    //! Run the algorithm
    /** The algorithm object can run only if its status is \a initialized.
     */
    void run();

    //! Reset the algorithm
    /** The algorithm object can be reset only if its status is \a initialized.
     */
    void reset();

  private:

    // Configuration data:
    bool        _debug_;           //!< Debug mode activation flag (configuration parameter)
    int         _max_nloops_;      //!< Maximum number of loops (configuration parameter)
    double      _epsilon_;         //!< Relative tolerance of the algorithm results (configuration parameter)
    std::string _output_filename_; //!< Name of the output data file (configuration parameter)

    // Working data:
    bool _initialized_; //!< Initialization flag (working data)

  };

} // end of namespace my

// OCD support from Bayeux/datatools:
#include <datatools/ocd_macros.h>

//! \brief Declaration of the OCD support for the my::algo class
//!
//! @arg my::algo the name the class
//!
//! This macro must be used outside of any namespace.
DOCD_CLASS_DECLARATION(my::algo);

#endif // MY_ALGO_H
