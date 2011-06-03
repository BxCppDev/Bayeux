// -*- mode: c++ ; -*- 
/* nor_cut.cc 
 */

#include <cuts/nor_cut.h>

#include <stdexcept>

namespace cuts {
  
  // ctor:
  nor_cut::nor_cut () : i_binary_cut ()
  {
  }

  // ctor:
  nor_cut::nor_cut (i_cut & cut_1_, i_cut & cut_2_) : 
    i_binary_cut (cut_1_, cut_2_)
  {
  }
  
  // dtor:
  nor_cut::~nor_cut ()
  {
  }

  bool nor_cut::_accept ()
  {
    if (_cut_1 == 0) 
      {
	throw std::runtime_error ("nor_cut::_accept: Null 'cut 1' !");
      }
    if (_cut_2 == 0) 
      {
	throw std::runtime_error ("nor_cut::_accept: Null 'cut 2' !");
      }
    int c1 = (_cut_1->accept () == ACCEPT) ? 1 : 0;
    int c2 = (_cut_2->accept () == ACCEPT) ? 1 : 0;
    if ((c1 + c2) == 0) 
      {
	return (ACCEPT);
      }
    else 
      {
	return (REJECT);
      }
  }
 
  // static method used within a cut factory:
  i_cut * nor_cut::create (const properties & configuration_, 
			   cut_dict_t * cut_dict_,
			   void * user_)
  {
    using namespace std;
    
    // create a new parameterized 'nor_cut' instance:
    nor_cut * cut_ptr = new nor_cut;
    i_binary_cut::install_cuts (configuration_, cut_dict_, *cut_ptr);
    return cut_ptr;	 
  }
  
  // register this creator:   
  i_cut::creator_registration<nor_cut> nor_cut::__CR;
 
  string nor_cut::cut_id () const
  {
    return "cuts::nor_cut";
  }
  
  cut_creator_t nor_cut::cut_creator () const
  {
    return nor_cut::create;
  }
 
} // end of namespace cuts

// end of nor_cut.cc
