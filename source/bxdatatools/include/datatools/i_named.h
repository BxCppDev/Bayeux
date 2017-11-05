//! \file datatools/i_named.h

#ifndef DATATOOLS_I_NAMED_H
#define DATATOOLS_I_NAMED_H

// Standard Library:
#include <string>

namespace datatools {

  //! \brief A pure abstract class (interface) for named objects,
  //!        possibly with dynamic name.
  //! Example: a class with fixed name
  //! \code
  //! class foo_with_name : public datatools::i_named {
  //! public:
  //!   foo_with_named(const std::string & name_) : _name_(name_) {}
  //!   std::string fetch_name() const { return _name_; }
  //! private:
  //!   std::string _name_;
  //! };
  //! \endcode
  //! Example: a class with dynamic name
  //! \code
  //! class foo_with_dynamic_name : public datatools::i_named {
  //! public:
  //!   foo_with_dynamic_name(const std::string & prefix_, std::size_t index_)
  //!     : _prefix_(prefix_), _index_(index_) {}
  //!   std::string fetch_name() const {
  //!     std::ostringstream name_ss;
  //!     name_ss << _prefix_ << _index_;
  //!     return name_ss.str();
  //!   }
  //! private:
  //!   std::string _prefix_;
  //!   std::size_t _index_;
  //! };
  //! \endcode
  class i_named
  {
  public:

    /// Return the explicit anonymous label
    static const std::string & explicit_anonymous_label();

    /** Return the name of an object
     * @return the name associated to the object
     */
    virtual std::string fetch_name() const = 0;

    // Utility functions:

    /// Check if the name is set
    static bool is_named(const i_named &);

    /// Check if the name is not set
    static bool is_unnamed(const i_named &);

    /// Check if the name is not set
    static bool is_nameless(const i_named &);

    /// Check if the name is not set or explicitely anonymous
    static bool is_anonymous(const i_named &);

  };

} // end of namespace datatools

#endif // DATATOOLS_I_NAMED_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
