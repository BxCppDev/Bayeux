// mygsl::permutation.cc

#include <mygsl/permutation.h>

#include <stdexcept>
#include <sstream>
#include <list>

#include <gsl/gsl_permutation.h>

namespace mygsl {
using namespace std;

//----------------------------------------------------------------------
// Private implementation
//
struct permutation::permutation_impl {
  gsl_permutation *_perm_;
};


bool permutation::_check_() const {
  if (pImpl->_perm_  == 0) return false;
  if (size() == 0) return false;
  return true;
}

void permutation::_check_throw_(size_t i_, const string& where_) const {
  if (!_check_()) {
    ostringstream message;
    message << "permutation::" << where_ << ": "
            << "Permutation is not initialized!";
    throw runtime_error(message.str());
  }
  if (i_ >= size()) {
    ostringstream message;
    message << "permutation::" << where_ << ": "
            << "Invalid permutation index " << i_ 
            << "!";
    throw runtime_error(message.str());
  }
}

bool permutation::is_initialized() const {
  return size() > 0;
}

const size_t* permutation::data() const {
  if (size() == 0) return 0;
  return gsl_permutation_data(pImpl->_perm_);
}

size_t permutation::inversions() const {
  _check_throw_(0, "inversions");
  return gsl_permutation_inversions(pImpl->_perm_);
}

size_t permutation::linear_cycles() const {
  _check_throw_(0, "linear_cycles");
  return gsl_permutation_linear_cycles(pImpl->_perm_);
}

size_t permutation::canonical_cycles() const {
  _check_throw_(0, "canonical_cycles");
  return gsl_permutation_canonical_cycles(pImpl->_perm_);
}

size_t permutation::get(size_t i_) const {
  _check_throw_(i_, "get");
  return gsl_permutation_get(pImpl->_perm_, i_);
}

void permutation::swap(size_t i_, size_t j_) {
  _check_throw_(i_, "swap");
  _check_throw_(j_, "swap");
  if (i_ == j_) return;
  gsl_permutation_swap(pImpl->_perm_, i_, j_);
}

bool permutation::is_valid() const {
  if (!_check_()) return false;
  return gsl_permutation_valid(pImpl->_perm_);
}

size_t permutation::size() const {
  if (pImpl->_perm_ != 0) {
    return gsl_permutation_size(pImpl->_perm_);
  }
  return 0;
}

// ctor:
permutation::permutation() {
  pImpl = new permutation_impl();
  pImpl->_perm_ = 0;
}

// ctor:
permutation::permutation(size_t sz_) {
  pImpl = new permutation_impl();
  pImpl->_perm_ = 0;
  init(sz_);
}

// ctor:
permutation::permutation(const permutation& p_) {
  pImpl = new permutation_impl();
  pImpl->_perm_ = 0;
  size_t sz = p_.size();
  if (sz > 0) pImpl->_perm_ = gsl_permutation_calloc(sz);
}

// dtor:
permutation::~permutation() {
  reset();
}

permutation & permutation::operator=(const permutation& p_) {
  if (this == &p_) return *this;
  size_t sz = p_.size();
  if (size() != sz) reset();
  
  if (sz > 0) gsl_permutation_memcpy(pImpl->_perm_, p_.pImpl->_perm_);

  return *this;
}

void permutation::reset() {
  if (pImpl->_perm_ != 0) {
    gsl_permutation_free(pImpl->_perm_);
    pImpl->_perm_ = 0;
  }
  delete pImpl;
  pImpl = 0;
}

void permutation::init(size_t sz_) {
  if (!pImpl) pImpl = new permutation_impl();
  if (pImpl->_perm_ != 0) reset();
  
  if (sz_ > 0) pImpl->_perm_ = gsl_permutation_calloc(sz_);
  
  if (pImpl->_perm_ == 0) {
    ostringstream message;
    message << "permutation::init: "
            << "Cannot allocate permutation with size " 
            << sz_ << "!";
    throw runtime_error(message.str());
  }
}

void permutation::reverse() {
  _check_throw_(0, "reverse");
  gsl_permutation_reverse(pImpl->_perm_);
}

bool permutation::inverse(permutation& p_) {
  _check_throw_(0, "inverse");
  p_.init(this->size());
  return gsl_permutation_inverse(p_.pImpl->_perm_, this->pImpl->_perm_) == GSL_SUCCESS;
}

bool permutation::canonical_to_linear(permutation& p_) {
  _check_throw_(0, "canonical_to_linear");
  p_.init(this->size());
  return gsl_permutation_canonical_to_linear(p_.pImpl->_perm_, this->pImpl->_perm_) == GSL_SUCCESS;
}

bool permutation::linear_to_canonical(permutation& p_) {
  _check_throw_(0, "linear_to_canonical");
  p_.init(this->size());
  return gsl_permutation_linear_to_canonical(p_.pImpl->_perm_, this->pImpl->_perm_) == GSL_SUCCESS;
}

bool permutation::next() {
  _check_throw_(0, "next");
  return gsl_permutation_next(pImpl->_perm_) == GSL_SUCCESS;
}

bool permutation::previous() {
  _check_throw_(0, "previous");
  return gsl_permutation_prev(pImpl->_perm_) == GSL_SUCCESS;
}

permutation& permutation::operator++() {
  this->next();
  return *this;
}

permutation& permutation::operator--() {
  this->previous();
  return *this;
}

void permutation::print(ostream& out_, bool eol_) const {
  for (int i = 0; i < size(); ++i) {
    if (i != 0) {
      out_ << ' ';
    }
    out_ << dec << get(i);
  }
  if (eol_) out_ << endl;
}

ostream& operator<<(ostream& out_, const permutation& p_) {
  out_ << '(';
  for (int i = 0; i < p_.size(); ++i) {
    if (i != 0) {
      out_ << ", ";
    }
    out_ << p_.get(i);
  }
  out_ << ')';
  return out_;
}

istream& operator>>(istream& in_, permutation& p_) {
  p_.reset();
  char c = 0;
  in_.get(c);
  size_t count = 0;
  list<size_t> values;
  if (!in_) {
    in_.putback(c);
    return in_;
  }
  if (c != '(') {
    in_.putback(c);
    in_.setstate(ios::failbit);
    return in_;
  }
  c = 0;
  in_.get(c);
  if (!in_) {
    in_.putback(c);
    return in_;
  }
  if (c != ')') {
    in_.putback(c);
  }
  else {
    return in_;
  }
  while (true) {
    size_t val;
    in_ >> val;
    if (!in_) {
      return in_;
    }
    count++;
    values.push_back(val);
    c = 0;
    in_.get(c);
    if (!in_) {
      in_.putback(c);
      return in_;
    }
    if ((c != ',' ) && (c != ')')) {
      in_.putback(c);
      in_.setstate(ios::failbit);
      return in_;     
    }
    if (c == ')') {
      break;
    }
  }
  if (count > 0) {
    p_.init(count);
    for (int i = 0; i < count; i++) {
      p_.pImpl->_perm_->data[i] = values.front();
      values.pop_front();
    }
  }
  return in_;
}

}

// end of mygsl::best_value.cc

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
