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
  private:
    bool _owned_;
    T *  _ref_;
    
  public:
    
    bool is_owned () const
    {
      return _owned_;
    }

    bool has_ref () const
    {
      return _ref_ != 0;
    }

    T * get_ptr ()
    {
      return _ref_;
    }
    
    const T * get_ptr () const
    {
      return _ref_;
    }

    T & get_ref ()
    {
      return *_ref_;
    }
    
    const T & get_ref () const
    {
      return *_ref_;
    }

    smart_ref () : _owned_ (false), _ref_(0)
    {
    }
    
    virtual ~smart_ref ()
    {
      if (_ref_ != 0)
        {
            if (_owned_) 
              {
                //cerr << "DEVEL: smart_ref::dtor: Deleting ref..." << endl;
                delete _ref_;
              }
            else
              {
                //cerr << "DEVEL: smart_ref::dtor: Detaching ref..." << endl;
              }
            _ref_ = 0;
        }
    }
    
    void set_ref (T * ref_)
    {
      if (_ref_ != 0)
        {
          if (_owned_) delete _ref_;
          _ref_ = 0;
        }
      _ref_ = ref_;
      _owned_ = true;
    }
    
    void set_ref (T & ref_)
    {
      if (_ref_ != 0)
        {
          if (_owned_) delete _ref_;
          _ref_ = 0;
        }
      _ref_ = &ref_;
      _owned_ = false;
    }
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
