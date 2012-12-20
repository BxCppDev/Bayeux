/* datatools/i_invokable.h */
//! \file datatools/i_invokable.h
#ifndef DATATOOLS_I_INVOKABLE_H_
#define DATATOOLS_I_INVOKABLE_H_

namespace datatools {

class command;

//! \brief An abstract class for inherited invokable classes.
class i_invokable {
 public:
  virtual ~i_invokable() {}

  static int invoke(i_invokable&, datatools::command&);

  virtual void do_invoke(datatools::command&);

  int invoke(datatools::command&);
};

} // end of namespace datatools 

#endif // DATATOOLS_I_INVOKABLE_H_

