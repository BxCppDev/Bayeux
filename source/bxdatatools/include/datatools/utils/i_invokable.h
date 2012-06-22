/* datatools/utils/i_invokable.h */
//! \file datatools/utils/i_invokable.h
#ifndef DATATOOLS_UTILS_I_INVOKABLE_H_
#define DATATOOLS_UTILS_I_INVOKABLE_H_

namespace datatools {
namespace utils {

class command;

//! \brief An abstract class for inherited invokable classes.
class i_invokable {
 public:
  virtual ~i_invokable() {}

  static int invoke(i_invokable&, datatools::utils::command&);

  virtual void do_invoke(datatools::utils::command&);

  int invoke(datatools::utils::command&);
};

} // end of namespace utils 
} // end of namespace datatools 

#endif // DATATOOLS_UTILS_I_INVOKABLE_H_

