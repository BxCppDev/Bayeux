// -*- mode: c++ ; -*-
/// materials/detail/tools.h
/* Author(s)    : Benoit Guillon  <guillon@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2010-09-22
 *
 * Description : Some typedef definitions
 *
 */

#ifndef MATERIALS_DETAIL_TOOLS_H
#define MATERIALS_DETAIL_TOOLS_H 1

// Standard library:
#include <string>
#include <map>

namespace materials {

  /// \brief A handler object that is used as a smart reference for an object of which it can have the responsibility of the deletion
  template <class T>
  class smart_ref
  {
  public:

    /// Check the object deletion responsibility
    bool is_owned() const
    {
      return _owned_;
    }

    /// Set the object deletion responsibility flag
    void set_owned(bool owned_)
    {
      _owned_ = owned_;
      return;
    }

    /// Set the alias information
    void set_alias_of(const std::string & name_)
    {
      _alias_of_ = name_;
      return;
    }

    /// Return the alias information
    const std::string & get_alias_of() const
    {
      return _alias_of_;
    }

    /// Check for existing alias information
    bool is_alias() const
    {
      return ! is_owned() && ! _alias_of_.empty();
    }

    /// Reset the alias information
    void reset_alias_of()
    {
      _alias_of_.clear();
      return;
    }

    /// Check for a existing reference
    bool has_ref() const
    {
      return _ref_ != 0;
    }

    /// Return a pointer to the mutable object
    T * grab_ptr()
    {
      return _ref_;
    }

    /// Return a pointer to the non mutable object
    const T * get_ptr() const
    {
      return _ref_;
    }

    /// Return a reference to the mutable object
    T & grab_ref()
    {
      return *_ref_;
    }

    /// Return a reference to the non mutable object
    const T & get_ref() const
    {
      return *_ref_;
    }

    /// Default constructor
    smart_ref() : _owned_(false), _ref_(0)
    {
      return;
    }

    /// Destructor
    virtual ~smart_ref()
    {
      reset_ref();
      return;
    }

    /// Reset the internal reference and delete the object if we have the responsibility of its deletion
    void reset_ref()
    {
      reset_alias_of();
      if (_ref_ != 0) {
        if (_owned_) delete _ref_;
        _ref_ = 0;
      }
      return;
    }

    /// Set the internal reference with the responsibility of the deletion (ala boost::scoped_ptr)
    void set_ref(T * ref_)
    {
      reset_ref();
      _ref_ = ref_;
      _owned_ = true;
    }

    /// Set the internal reference without the responsibility of the deletion
    void set_ref(T & ref_)
    {
      reset_ref();
      _ref_ = &ref_;
      _owned_ = false;
      return;
    }

  private:

    bool _owned_;           /// Flag to indicate if the smart_ref object is responsible of the deletion of the instance
    T *  _ref_;             /// Address of an instance
    std::string _alias_of_; /// Name of another instance of which the smart_ref is an alias (symbolic link)

  };

  class isotope;
  class element;
  class material;

  typedef std::map<std::string, smart_ref<isotope> >  isotope_dict_type;
  typedef std::map<std::string, smart_ref<element> >  element_dict_type;
  typedef std::map<std::string, smart_ref<material> > material_dict_type;

} // end of namespace materials

#endif // MATERIALS_DETAIL_TOOLS_H
