/* datatools/utils/i_clear.h */

//! \file datatools/utils/i_clear.h
#ifndef DATATOOLS_UTILS_I_CLEAR_H_
#define DATATOOLS_UTILS_I_CLEAR_H_


namespace datatools {
namespace utils {

//! \brief A pure abstract class (interface) for inherited clearable classes.
class i_clear {
 public:
  //virtual ~i_clear() {}

  /**
   * A pure virtual member.
   * Clear (reset) the object's internals (typically empty a container).
   */
  virtual void clear() = 0;
};

} // end of namespace utils 
} // end of namespace datatools 

#endif // DATATOOLS_UTILS_I_CLEAR_H_

