/* datatools/utils/i_named.h */
#ifndef DATATOOLS_UTILS_I_NAMED_H_
#define DATATOOLS_UTILS_I_NAMED_H_
// Standard Library
#include <string>

// Third Party
// - A

// This Project

//! \file datatools/utils/i_named.h

namespace datatools {
namespace utils {

//! \brief A pure abstract class (interface) for inherited named objects.
class i_named {
 public:
  static const std::string ANONYMOUS_LABEL;

  /**
   * A pure virtual member.
   * @return the name associated to the object.
   */
  virtual std::string get_name() = 0;

  static bool has_a_name(const i_named& a_named);

  static bool is_anonymous(const i_named& a_named);
};

} // end of namespace utils 
} // end of namespace datatools 

#endif // DATATOOLS_UTILS_I_NAMED_H_

