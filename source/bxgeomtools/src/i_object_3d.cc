// -*- mode: c++; -*- 
/* i_object_3d.cc
 */

#include <geomtools/i_object_3d.h>

#include <geomtools/box.h>

namespace geomtools {

  using namespace std;

  const double i_object_3d::DEFAULT_TOLERANCE      = constants::DEFAULT_TOLERANCE;
  const double i_object_3d::USING_PROPER_TOLERANCE = 0.0;
  
  const uint32_t i_object_3d::ALL_SURFACES = 0xFFFFFFFF;
  const uint32_t i_object_3d::NO_SURFACES  = 0;

  void i_object_3d::compute_bounding_box (box & bb_)
  {
    bb_.reset ();
  }
   
  double i_object_3d::get_tolerance () const
  {
    return __tolerance;
  }

  void i_object_3d::set_tolerance (double tolerance_)
  {
    if (tolerance_ <= 0.0)
      {
	ostringstream message;
	message << "i_object_3d::set_tolerance: tolerance value '"
		<< tolerance_ << "' must be positive !";
	throw runtime_error (message.str ());
      }
    __tolerance = tolerance_;
  }

  datatools::utils::properties & i_object_3d::properties ()
  {
    return __properties;
  }
  
  const datatools::utils::properties & i_object_3d::properties () const
  {
    return __properties;
  }

  bool i_object_3d::is_composite () const
  {
    return false;
  }

  // ctor:
  i_object_3d::i_object_3d ()
  {
    __tolerance = DEFAULT_TOLERANCE;
  }

  i_object_3d::i_object_3d (double tolerance_)
  {
    __tolerance = DEFAULT_TOLERANCE;
    if (tolerance_ <= 0.0)
      {
	__tolerance = DEFAULT_TOLERANCE;	
      }
    else 
      {
	/*
	clog << "DEVEL: i_object_3d::i_object_3d: i_object_3d::DEFAULT_TOLERANCE= " << i_object_3d::DEFAULT_TOLERANCE << endl;
	clog << "DEVEL: i_object_3d::i_object_3d: i_object_3d::USING_PROPER_TOLERANCE= " << i_object_3d::USING_PROPER_TOLERANCE << endl;
	clog << "DEVEL: i_object_3d::i_object_3d: tolerance_ = " << tolerance_ << endl;
	*/
	set_tolerance (tolerance_);
      }
  }

  // dtor:
  i_object_3d::~i_object_3d ()
  {
    i_object_3d::reset ();
  }

  void i_object_3d::reset ()
  {
    __properties.clear ();
  }
  

  void i_object_3d::tree_dump (ostream & out_, 
				  const string & title_, 
				  const string & indent_, 
				  bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) 
      {
        out_ << indent << title_ << endl;
      }
    out_ << indent << i_tree_dumpable::tag 
	 << "Shape name : \"" << get_shape_name () << "\"" << endl;

    out_ << indent << i_tree_dumpable::tag 
	 << "Tolerance  = " << __tolerance / CLHEP::mm << " mm" << endl;

    {
      out_ << indent << du::i_tree_dumpable::tag
	   << "Properties : ";
      if ( __properties.size () == 0) 
        {
          out_ << "<empty>"; 
        }
      out_ << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << du::i_tree_dumpable::skip_tag;
        __properties.tree_dump (out_,"",indent_oss.str ());
      }
    }

    {
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	   << "Dimensionality : " << get_dimensional () <<  endl;
    }

    return;
  }

} // end of namespace geomtools

// end of i_object_3d.cc
