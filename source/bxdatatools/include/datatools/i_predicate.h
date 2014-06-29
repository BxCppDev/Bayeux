/// \file datatools/i_predicate.h
#ifndef DATATOOLS_I_PREDICATE_H
#define DATATOOLS_I_PREDICATE_H

// Standard Library:
#include<functional>


namespace datatools {
//! \file datatools/i_predicate.h

//! \brief A template predicate abstract class
template <class T>
class i_predicate : public std::unary_function<const T&, bool> {
 public:
  virtual bool operator()(const T& obj) const = 0;
};


//! \brief A template mother-to-daughter caster predicate class
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

} // end of namespace datatools

#endif // DATATOOLS_I_PREDICATE_H
