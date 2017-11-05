/// \file datatools/smart_ref.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-16
 * Last modified: 2010-03-16
 *
 * License:
 *
 * Description:
 *   Smart reference to an existing instance
 *   The referenced type must have a default constructor.
 *
 */

#ifndef DATATOOLS_SMART_REF_H
#define DATATOOLS_SMART_REF_H

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <stdexcept>
#include <sstream>

// Third Party:
// - Boost:
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

// This Project:
#include <datatools/properties.h>

namespace datatools {

  //! \brief A template class that stores a reference to an object and additional associated meta-data
  template <class T>
  class smart_ref
  {
  public:
    typedef T                        instance_type;
    typedef instance_type&           reference_type;
    typedef const instance_type&     const_reference_type;
    typedef instance_type*           pointer_type;
    typedef const instance_type*     const_pointer_type;
    typedef std::list<instance_type> col_type;
    typedef smart_ref<instance_type> smart_ref_type;

    /// Default constructor
    smart_ref()
      : _ref_(nullptr)
      , _properties_()
    {
      return;
    }

    /// Constructor
    smart_ref(const_reference_type obj)
    {
      this->set(obj);
      return;
    }

    /// Destructor
    virtual ~smart_ref()
    {
      return;
    }

    /// Set the internal reference to the target object
    void set(const_reference_type obj)
    {
      _ref_ = const_cast<pointer_type>(&obj);
      return;
    }

    /// Return the reference to the target object
    const_reference_type get() const
    {
      return *_ref_;
    }

    /// Return a non mutable reference to the container of properties
    const datatools::properties& get_properties() const
    {
      return _properties_;
    }

    /// Return a mutable reference to the container of properties
    datatools::properties& grab_properties()
    {
      return _properties_;
    }

    /// \deprecated Return a mutable reference to the container of properties
    datatools::properties& get_properties()
    {
      return _properties_;
    }

    /// Set the container of properties
    void set_properties(const datatools::properties & props)
    {
      _properties_ = props;
      return;
    }

    /// Reset the internal reference
    void reset()
    {
      _ref_ = 0;
      return;
    }

    /// Check the validity of the smart reference
    bool is_valid() const
    {
      return _ref_ != 0;
    }

    //! \brief Predicate used by the smart_ref template class
    class has_flag
      : public std::unary_function<smart_ref_type, bool>
    {
    public:

      explicit has_flag(const std::string & flag_)
        : _flag_(flag_)
      {
        return;
      }

      bool operator()(const smart_ref_type & s_) const
      {
        if (!s_.is_valid()) return false;
        return (s_.get_properties().has_flag(_flag_));
      }

    private:

      std::string _flag_;

    };

  private:

    pointer_type          _ref_;        //!< Handle to the target object
    datatools::properties _properties_; //!< Container of auxiliary properties

    friend class boost::serialization::access;

    /// Boost serialization method
    template<class Archive>
    void serialize(Archive& ar_, const unsigned int /*version*/)
    {
      ar_ & boost::serialization::make_nvp("properties", _properties_);
      ar_ & boost::serialization::make_nvp("ref", _ref_);
      return;
    }

  };

} // end of namespace datatools

#endif // DATATOOLS_SMART_REF_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
