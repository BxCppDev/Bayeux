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
  }

  i_composite_shape_3d::shape_t::shape_t ()
  {
    __delete = false;
    __shape = 0;
  }

  i_composite_shape_3d::shape_t::~shape_t ()
  {
    reset ();
  }

  void 
  i_composite_shape_3d::shape_t::reset ()
  {
    if (__delete)
      {
	delete __shape;
	__shape = 0;
      }    
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
  }

  void 
  i_composite_shape_3d::shape_t::dump (std::ostream & out_) const
  {
    out_ << "i_composite_shape_3d::shape_t" << std::endl;
    out_ << "|-- delete: " << __delete << std::endl;
    out_ << "|-- shape: " << std::hex << (void *) __shape << std::dec << std::endl;
    out_ << "`-- placement: " << std::endl; // << __placement << std::endl;
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
  }
     
  i_composite_shape_3d::i_composite_shape_3d (double skin_)
  {
    set_skin (skin_);
  }
  
  i_composite_shape_3d::~i_composite_shape_3d ()
  {
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
  }
  
  void 
  i_composite_shape_3d::set_shape2 (i_shape_3d & sh3d_, const placement & p_)
  {
    shape_t::make_shape (sh3d_, p_, __shape2);
  }
    
  void 
  i_composite_shape_3d::set_shape1 (i_shape_3d * sh3d_, const placement & p_)
  {
    shape_t::make_shape (sh3d_, p_, __shape1);
  }
    
  void 
  i_composite_shape_3d::set_shape2 (i_shape_3d * sh3d_, const placement & p_)
  {
    shape_t::make_shape (sh3d_, p_, __shape2);
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

} // end of namespace geomtools

// end of i_composite_shape_3d.cc
