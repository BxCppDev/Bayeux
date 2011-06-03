// -*- mode: c++ ; -*- 
/* and_cut.cc
 */

#include <cuts/and_cut.h>

#include <stdexcept>

namespace cuts { 
  
  // ctor:
  and_cut::and_cut () : i_binary_cut ()
  {
  }

  // ctor:
  and_cut::and_cut (i_cut & cut_1_, i_cut & cut_2_) : 
    i_binary_cut (cut_1_, cut_2_)
  {
  }
  
  // dtor:
  and_cut::~and_cut ()
  {
  }

  bool and_cut::_accept ()
  {
    if (_cut_1 == 0) 
      {
	throw std::runtime_error ("and_cut::_accept: Null 'cut 1' !");
      }
    if (_cut_2 == 0) 
      {
	throw std::runtime_error ("and_cut::_accept: Null 'cut 2' !");
      }
    int c1 = (_cut_1->accept () == ACCEPT) ? 1 : 0;
    int c2 = (_cut_2->accept () == ACCEPT) ? 1 : 0;
    if ((c1 + c2) == 2) 
      {
	return (ACCEPT);
      }
    else 
      {
	return (REJECT);
      }
  }

  // static method used within a cut factory:
  i_cut * and_cut::create (const properties & configuration_, 
			   cut_dict_t * cut_dict_,
			   void * user_)
  {
    using namespace std;

    // create a new parameterized 'and_cut' instance:
    and_cut * cut_ptr = new and_cut;
    i_binary_cut::install_cuts (configuration_, cut_dict_, *cut_ptr);
    return cut_ptr;	 
  }

  // register this creator:   
  i_cut::creator_registration<and_cut> and_cut::__CR;
 
  string and_cut::cut_id () const
  {
    return "cuts::and_cut";
  }
  
  cut_creator_t and_cut::cut_creator () const
  {
    return and_cut::create;
  }
  
} // end of namespace cuts

// end of and_cut.cc
