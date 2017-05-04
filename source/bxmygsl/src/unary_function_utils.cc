// unary_function_utils.cc

// Ourselves:
#include <mygsl/unary_function_utils.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  unary_function_handle::unary_function_handle()
    : _hfunc_()
    , _pfunc_(nullptr)
  {
    return;
  }

  unary_function_handle::unary_function_handle(const i_unary_function & f_)
    : _hfunc_()
    , _pfunc_(&f_)
  {
    _check_();
    return;
  }

  unary_function_handle::unary_function_handle(const const_unary_function_handle_type & cfh_)
    : _hfunc_(cfh_)
    , _pfunc_(nullptr)
  {
    _check_();
    return;
  }

  unary_function_handle::~unary_function_handle()
  {
    return;
  }

  void unary_function_handle::_check_()
  {
    if (!is_null()) {
      DT_THROW_IF(!func().is_initialized(), std::logic_error, "Cannot handle an uninitialized function!");
    }
    return;
  }

  const i_unary_function & unary_function_handle::func() const
  {
    DT_THROW_IF(is_null(), std::logic_error, "Null handle!");
    if (_pfunc_ != nullptr) {
      return *_pfunc_;
    }
    return _hfunc_.get();
  }

  bool unary_function_handle::is_null() const
  {
    if (_pfunc_ != nullptr) {
      return false;
    }
    if (_hfunc_.has_data()) {
      return false;
    }
    return true;
  }

  void unary_function_handle::reset(const i_unary_function & func_)
  {
    if (is_null()) {
      _pfunc_ = &func_;
      return;
    }
    if (&func() == &func_) return;
    reset();
    _pfunc_ = &func_;
    _check_();
    return;
  }

  void unary_function_handle::reset(const const_unary_function_handle_type & cfh_)
  {
    if (is_null()) {
      _hfunc_ = cfh_;
      return;
    }
    if (&func() == &cfh_.get()) return;
    reset();
    _hfunc_ = cfh_;
    _check_();
    return;
  }

  void unary_function_handle::reset()
  {
    if (_pfunc_ != nullptr) {
      _pfunc_ = nullptr;
    }
    if (_hfunc_.has_data()) {
      _hfunc_.reset();
    }
    return;
  }

} // namespace mygsl
