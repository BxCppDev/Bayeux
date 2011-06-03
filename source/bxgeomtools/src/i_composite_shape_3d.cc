// -*- mode: c++; -*- 
/* i_composite_shape_3d.cc 
 */

#include <geomtools/i_composite_shape_3d.h>

namespace geomtools {

  void 
  i_composite_shape_3d::shape_t::copy (shape_t & sh_)
  {
    __delete = false;
    __shape = sh_.__shape;
    __placement = sh_.__placement;
    if (sh_.__delete) 
      {
	__delete = true;
	sh_.__delete = false;
      }
    return;
  }

  i_composite_shape_3d::shape_t::shape_t ()
  {
    __delete = false;
    __shape = 0;
    return;
  }

  i_composite_shape_3d::shape_t::~shape_t ()
  {
    reset ();
    return;
  }

  void 
  i_composite_shape_3d::shape_t::reset ()
  {
    if (__delete)
      {
	delete __shape;
	__shape = 0;
      }    
    return;
  }

  bool
  i_composite_shape_3d::shape_t::is_delete () const
  {
    return __delete;
  }

  bool
  i_composite_shape_3d::shape_t::is_valid () const
  {
    return __shape != 0;
  }

  const i_shape_3d & 
  i_composite_shape_3d::shape_t::get_shape () const
  {
    return *__shape;
  } 

  i_shape_3d & 
  i_composite_shape_3d::shape_t::get_shape ()
  {
    return *__shape;
  } 

  const placement & 
  i_composite_shape_3d::shape_t::get_placement () const 
  {
    return __placement;
  } 

  placement & 
  i_composite_shape_3d::shape_t::get_placement () 
  {
    return __placement;
  } 

  void 
  i_composite_shape_3d::shape_t::make_shape (i_shape_3d & sh3d_, 
					     const placement & p_, 
					     i_composite_shape_3d::shape_t & sh_)
  {
    sh_.reset ();
    sh_.__delete = false;
    sh_.__shape = &sh3d_;
    sh_.__placement = p_;
    return;
  }

  void 
  i_composite_shape_3d::shape_t::make_shape (i_shape_3d * sh3d_, 
					     const placement & p_, 
					     i_composite_shape_3d::shape_t & sh_)
  {
    sh_.reset ();
    sh_.__delete = true;
    sh_.__shape = sh3d_;
    sh_.__placement = p_;
    return;
  }
  
  void i_composite_shape_3d::shape_t::tree_dump (ostream & out_, 
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

    {
      if (__shape != 0)
	{
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Shape = ";
	  out_ << "'" << __shape->get_shape_name () << "' " 
	       << (__delete? "(owned)": "(not owned)") << endl;
	  ostringstream oss_indent;
	  oss_indent << indent << du::i_tree_dumpable::skip_tag;
	  __shape->tree_dump (out_, 
			      "", 
			      oss_indent.str (), 
			      false);
	}
      else
	{
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Shape = " << "<no shape>" << endl;
	}
    }

    {
      out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)  
	   << "Placement : " << endl;
      ostringstream oss_indent;
      oss_indent << indent << du::i_tree_dumpable::inherit_skip_tag (inherit_);
      __placement.tree_dump (out_, 
			     "", 
			     oss_indent.str (), 
			     false);
    }

    return;
  }

  void i_composite_shape_3d::shape_t::dump (std::ostream & out_) const
  {
    out_ << "i_composite_shape_3d::shape_t" << std::endl;
    out_ << "|-- delete: " << __delete << std::endl;
    out_ << "|-- shape: " << std::hex << (void *) __shape << std::dec << std::endl;
    out_ << "`-- placement: " << std::endl; 
    return;
  }

  /***************************************/

  void 
  i_composite_shape_3d::dump (std::ostream & out_) const
  {
    out_ << "i_composite_shape_3d::i_composite_shape_3d: " << get_shape_name () << std::endl;
    out_ << "|-- shape1: " << std::endl;
    __shape1.dump (out_);
    out_ << "|-- shape2: " << std::endl;
    __shape2.dump (out_);
    out_ << "`-- end." << std::endl;
    return;
  }
     
  i_composite_shape_3d::i_composite_shape_3d (double skin_)
  {
    set_skin (skin_);
    return;
  }
  
  i_composite_shape_3d::~i_composite_shape_3d ()
  {
    return;
  }

  bool 
  i_composite_shape_3d::is_composite () const
  {
    return true;
  }

  void 
  i_composite_shape_3d::set_shape1 (i_shape_3d & sh3d_, const placement & p_)
  {
    shape_t::make_shape (sh3d_, p_, __shape1);
    return;
  }
  
  void 
  i_composite_shape_3d::set_shape2 (i_shape_3d & sh3d_, const placement & p_)
  {
    shape_t::make_shape (sh3d_, p_, __shape2);
    return;
  }
    
  void 
  i_composite_shape_3d::set_shape1 (i_shape_3d * sh3d_, const placement & p_)
  {
    shape_t::make_shape (sh3d_, p_, __shape1);
    return;
  }
    
  void 
  i_composite_shape_3d::set_shapes (i_shape_3d & sh3d1_, 
				    i_shape_3d & sh3d2_, const placement & p2_)
  {
    placement p1;
    shape_t::make_shape (sh3d1_, p1, __shape1);
    shape_t::make_shape (sh3d2_, p2_, __shape2);
    return;
  }
    
  void 
  i_composite_shape_3d::set_shapes (i_shape_3d * sh3d1_, 
				    i_shape_3d * sh3d2_, const placement & p2_)
  {
    placement p1;
    shape_t::make_shape (sh3d1_, p1, __shape1);
    shape_t::make_shape (sh3d2_, p2_, __shape2);
    return;
  }
    
  void 
  i_composite_shape_3d::set_shape2 (i_shape_3d * sh3d_, const placement & p_)
  {
    shape_t::make_shape (sh3d_, p_, __shape2);
    return;
  }

  const i_composite_shape_3d::shape_t & 
  i_composite_shape_3d::get_shape1 () const
  {
    return __shape1;
  }
  
  const i_composite_shape_3d::shape_t & 
  i_composite_shape_3d::get_shape2 () const
  {
    return __shape2;
  }

  const i_composite_shape_3d::shape_t & 
  i_composite_shape_3d::get_shape (int i_) const
  {
    if (i_ == 0) return __shape1;
    return __shape2;
  }

  
  void i_composite_shape_3d::tree_dump (ostream & out_, 
					const string & title_, 
					const string & indent_, 
					bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    {
     out_ << indent << du::i_tree_dumpable::tag 
	  << "Shape 1 : " << endl;
     ostringstream oss_indent;
     oss_indent << indent << i_tree_dumpable::skip_tag;
     get_shape1 ().tree_dump (out_, 
			      "", 
			      oss_indent.str (), 
			      false);
    }

    {
     out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)   
	  << "Shape 2 : " << endl;
     ostringstream oss_indent;
     oss_indent << indent << i_tree_dumpable::inherit_skip_tag (inherit_);
     get_shape2 ().tree_dump (out_, 
			      "", 
			      oss_indent.str (), 
			      false);
    }

    return;
  }

} // end of namespace geomtools

// end of i_composite_shape_3d.cc
