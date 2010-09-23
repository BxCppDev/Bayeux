// -*- mode: c++ ; -*- 
/* tools.h
 * Author (s) : Benoit Guillon  <guillon@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2010-09-22
 *
 * Description : Some typedef definitions
 *
 */
 
#ifndef __materials__detail__tools_h
#define __materials__detail__tools_h 1

#include <string>
#include <map>

namespace mat {
  
  using namespace std;
 
  template <class T> 
  class smart_ref
  {
  private:
    bool __owned;
    T *  __ref;
    
  public:
    
    bool is_owned () const
    {
      return __owned;
    }

    bool has_ref () const
    {
      return __ref != 0;
    }

    T & get_ref ()
    {
      return *__ref;
    }
    
    const T & get_ref () const
    {
      return *__ref;
    }

    smart_ref () : __owned (false), __ref(0)
    {
    }
    
    virtual ~smart_ref ()
    {
      if (__ref != 0)
	{
	    if (__owned) 
	      {
		//cerr << "DEVEL: smart_ref::dtor: Deleting ref..." << endl;
		delete __ref;
	      }
	    else
	      {
		//cerr << "DEVEL: smart_ref::dtor: Detaching ref..." << endl;
	      }
	    __ref = 0;
	}
    }
    
    void set_ref (T * ref_)
    {
      if (__ref != 0)
	{
	  if (__owned) delete __ref;
	  __ref = 0;
	}
      __ref = ref_;
      __owned = true;
    }
    
    void set_ref (T & ref_)
    {
      if (__ref != 0)
	{
	  if (__owned) delete __ref;
	  __ref = 0;
	}
      __ref = &ref_;
      __owned = false;
    }
  };

  class isotope;
  class element;
  class material;

  typedef map<string, smart_ref<isotope> >  isotope_dict_t;
  typedef map<string, smart_ref<element> >  element_dict_t;
  typedef map<string, smart_ref<material> > material_dict_t;

} // end of namespace mat

#endif // __materials__detail__tools_h

// end of tools.h
