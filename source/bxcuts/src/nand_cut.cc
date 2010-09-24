// -*- mode: c++ ; -*- 
/* nand_cut.cc
 */

#include <cuts/nand_cut.h>

namespace cuts {
  
  // ctor:
  nand_cut::nand_cut () : i_binary_cut ()
  {
  }

  // ctor:
  nand_cut::nand_cut (i_cut & cut_1_, i_cut & cut_2_) : 
    i_binary_cut (cut_1_, cut_2_)
  {
  }
  
  // dtor:
  nand_cut::~nand_cut ()
  {
  }

  bool nand_cut::_accept ()
  {
    if (_cut_1 == 0) 
      {
	throw std::runtime_error ("nand_cut::_accept: Null 'cut 1' !");
      }
    if (_cut_2 == 0) 
      {
	throw std::runtime_error ("nand_cut::_accept: Null 'cut 2' !");
      }
    int c1 = (_cut_1->accept () == ACCEPT) ? 1 : 0;
    int c2 = (_cut_2->accept () == ACCEPT) ? 1 : 0;
    if ((c1 + c2) == 2) 
      {
	return (REJECT);
      }
    else 
      {
	return (ACCEPT);
      }
  }

  // static method used within a cut factory:
  i_cut * nand_cut::create (const properties & configuration_, 
			    cut_dict_t * cut_dict_,
			    void * user_)
  {
    using namespace std;
    
    // create a new parameterized 'nand_cut' instance:
    nand_cut * cut_ptr = new nand_cut;
    i_binary_cut::install_cuts (configuration_, cut_dict_, *cut_ptr);
    return cut_ptr;	 
  }
  
  // register this creator:   
  i_cut::creator_registration<nand_cut> nand_cut::__CR;
 
  string nand_cut::cut_id () const
  {
    return "cuts::nand_cut";
  }
  
  cut_creator_t nand_cut::cut_creator () const
  {
    return nand_cut::create;
  }
  
} // end of namespace cuts

// end of nand_cut.cc
