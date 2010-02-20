// -*- mode: c++; -*- 
/* i_placement.cc 
 */

#include <geomtools/i_placement.h>
#include <geomtools/placement.h>

namespace geomtools {

  // ctor:
  i_placement::i_placement ()
  {
  }

  // dtor:
  i_placement::~i_placement ()
  {
  }

  void i_placement::tree_dump (ostream & out_, 
			       const string & title_, 
			       const string & indent_, 
			       bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) 
      {
        out_ << indent << title_ << std::endl;
      }
    out_ << indent << i_tree_dumpable::inherit_tag (inherit_)  
	 << "Number of items  = " << get_number_of_items () << std::endl;
    return;
  }
 
} // end of namespace geomtools

// end of i_placement.cc
