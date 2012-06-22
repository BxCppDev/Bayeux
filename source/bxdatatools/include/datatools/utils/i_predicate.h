/* i_predicate.h */
#ifndef DATATOOLS_UTILS_I_PREDICATE_H_
#define DATATOOLS_UTILS_I_PREDICATE_H_
// Standard Library
#include<functional>

// Third Party
// - A

// This Project


namespace datatools {
namespace utils {
//! \file datatools/utils/i_predicate.h

template <class T>
class i_predicate : public std::unary_function<const T&, bool> {
 public:
  virtual bool operator()(const T& obj) const = 0;
};


template<class Mother, class Daughter>
class mother_to_daughter_predicate : public i_predicate<Daughter> {
 public:
  mother_to_daughter_predicate(i_predicate<Mother>& a_mother_predicate) {
    mother_predicate_ = &a_mother_predicate;
  }

  virtual bool operator()(const Daughter& obj) const {
    return (*mother_predicate_)(obj);
  }

 private:
  i_predicate<Mother> *mother_predicate_;
};

} // end of namespace utils 
} // end of namespace datatools 

#endif // DATATOOLS_UTILS_I_PREDICATE_H_

