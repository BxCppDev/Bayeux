// -*- mode: c++ ; -*- 
/* tools.h
 * Author (s) : Benoit Guillon  <guillon@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2010-09-22
 *
 * Description : Some typedef definitions
 *
 */
 
#ifndef MATERIALS_DETAIL_TOOLS_H_
#define MATERIALS_DETAIL_TOOLS_H_ 1

#include <string>
#include <map>

namespace materials {
 
  template <class T> 
  class smart_ref
  {
  public:
    
    bool is_owned () const
    {
      return _owned_;
    }

    void set_owned(bool owned_)
    {
      _owned_ = owned_;
      return;
    }

    void set_alias_of(const std::string & name_)
    {
      _alias_of_ = name_;
      return;
    }

    const std::string & get_alias_of() const
    {
      return _alias_of_;
    }

    bool is_alias () const
    {
      return ! is_owned () && ! _alias_of_.empty();
    }

    void reset_alias_of()
    {
      _alias_of_.clear();
      return;
    }

    bool has_ref () const
    {
      return _ref_ != 0;
    }

    T * grab_ptr ()
    {
      return _ref_;
    }
    
    const T * get_ptr () const
    {
      return _ref_;
    }

    T & grab_ref ()
    {
      return *_ref_;
    }
    
    const T & get_ref () const
    {
      return *_ref_;
    }

    smart_ref () : _owned_ (false), _ref_(0)
    {
      return;
    }
    
    virtual ~smart_ref ()
    {
      reset_ref ();
      return;
    }
    
    void reset_ref ()
    {
      reset_alias_of();
      if (_ref_ != 0)
        {
          if (_owned_) delete _ref_;
          _ref_ = 0;
        }
       return;
    }

    void set_ref (T * ref_)
    {
      reset_ref ();
      _ref_ = ref_;
      _owned_ = true;
    }
    
    void set_ref (T & ref_)
    {
      reset_ref ();
      _ref_ = &ref_;
      _owned_ = false;
      return;
    }
   
  private:

    bool _owned_;
    T *  _ref_;
    std::string _alias_of_;
    
  };

  class isotope;
  class element;
  class material;

  typedef std::map<std::string, smart_ref<isotope> >  isotope_dict_type;
  typedef std::map<std::string, smart_ref<element> >  element_dict_type;
  typedef std::map<std::string, smart_ref<material> > material_dict_type;

} // end of namespace materials

#endif // MATERIALS_DETAIL_TOOLS_H_

// end of tools.h
