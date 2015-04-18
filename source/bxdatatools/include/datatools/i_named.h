#ifndef DATATOOLS_I_NAMED_H
#define DATATOOLS_I_NAMED_H

// Standard Library:
#include <string>

//! \file datatools/i_named.h
namespace datatools {

  //! \brief A pure abstract class (interface) for inherited named objects.
  class i_named {
  public:

    struct constants {
      static const std::string & anonymous_label();
    };

    /** Compute the name of an object
     * @return the name associated to the object
     */
    virtual std::string get_name() = 0;

    /// Check if a named object has a valid name
    static bool has_a_name(const i_named& a_named);

    /// Check is a named object is anonymous
    static bool is_anonymous(const i_named& a_named);

  };

} // end of namespace datatools

#endif // DATATOOLS_I_NAMED_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
