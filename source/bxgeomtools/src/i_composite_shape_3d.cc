// -*- mode: c++; -*- 
/* i_composite_shape_3d.cc 
 */

#include <geomtools/i_composite_shape_3d.h>

namespace geomtools {

  void 
  i_composite_shape_3d::shape_t::copy (shape_t & sh_)
  {
    _delete_ = false;
    _shape_ = sh_._shape_;
    _placement_ = sh_._placement_;
    if (sh_._delete_) 
      {
        _delete_ = true;
        sh_._delete_ = false;
      }
    return;
  }

  i_composite_shape_3d::shape_t::shape_t ()
  {
    _delete_ = false;
    _shape_ = 0;
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
    if (_delete_)
      {
        delete _shape_;
        _shape_ = 0;
      }    
    return;
  }

  bool
  i_composite_shape_3d::shape_t::is_delete () const
  {
    return _delete_;
  }

  bool
  i_composite_shape_3d::shape_t::is_valid () const
  {
    return _shape_ != 0;
  }

  const i_shape_3d & 
  i_composite_shape_3d::shape_t::get_shape () const
  {
    return *_shape_;
  } 

  i_shape_3d & 
  i_composite_shape_3d::shape_t::get_shape ()
  {
    return *_shape_;
  } 

  const placement & 
  i_composite_shape_3d::shape_t::get_placement () const 
  {
    return _placement_;
  } 

  placement & 
  i_composite_shape_3d::shape_t::get_placement () 
  {
    return _placement_;
  } 

  void 
  i_composite_shape_3d::shape_t::make_shape (i_shape_3d & sh3d_, 
                                             const placement & p_, 
                                             i_composite_shape_3d::shape_t & sh_)
  {
    sh_.reset ();
    sh_._delete_ = false;
    sh_._shape_ = &sh3d_;
    sh_._placement_ = p_;
    return;
  }

  void 
  i_composite_shape_3d::shape_t::make_shape (i_shape_3d * sh3d_, 
                                             const placement & p_, 
                                             i_composite_shape_3d::shape_t & sh_)
  {
    sh_.reset ();
    sh_._delete_ = true;
    sh_._shape_ = sh3d_;
    sh_._placement_ = p_;
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
      if (_shape_ != 0)
        {
          out_ << indent << du::i_tree_dumpable::tag 
               << "Shape = ";
          out_ << "'" << _shape_->get_shape_name () << "' " 
               << (_delete_? "(owned)": "(not owned)") << endl;
          ostringstream oss_indent;
          oss_indent << indent << du::i_tree_dumpable::skip_tag;
          _shape_->tree_dump (out_, 
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
      _placement_.tree_dump (out_, 
                             "", 
                             oss_indent.str (), 
                             false);
    }

    return;
  }

  void i_composite_shape_3d::shape_t::dump (std::ostream & out_) const
  {
    out_ << "i_composite_shape_3d::shape_t" << std::endl;
    out_ << "|-- delete: " << _delete_ << std::endl;
    out_ << "|-- shape: " << std::hex << (void *) _shape_ << std::dec << std::endl;
    out_ << "`-- placement: " << std::endl; 
    return;
  }

  /***************************************/

  void 
  i_composite_shape_3d::dump (std::ostream & out_) const
  {
    out_ << "i_composite_shape_3d::i_composite_shape_3d: " << get_shape_name () << std::endl;
    out_ << "|-- shape1: " << std::endl;
    _shape1_.dump (out_);
    out_ << "|-- shape2: " << std::endl;
    _shape2_.dump (out_);
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
    shape_t::make_shape (sh3d_, p_, _shape1_);
    return;
  }
  
  void 
  i_composite_shape_3d::set_shape2 (i_shape_3d & sh3d_, const placement & p_)
  {
    shape_t::make_shape (sh3d_, p_, _shape2_);
    return;
  }
    
  void 
  i_composite_shape_3d::set_shape1 (i_shape_3d * sh3d_, const placement & p_)
  {
    shape_t::make_shape (sh3d_, p_, _shape1_);
    return;
  }
    
  void 
  i_composite_shape_3d::set_shapes (i_shape_3d & sh3d1_, 
                                    i_shape_3d & sh3d2_, const placement & p2_)
  {
    placement p1;
    shape_t::make_shape (sh3d1_, p1, _shape1_);
    shape_t::make_shape (sh3d2_, p2_, _shape2_);
    return;
  }
    
  void 
  i_composite_shape_3d::set_shapes (i_shape_3d * sh3d1_, 
                                    i_shape_3d * sh3d2_, const placement & p2_)
  {
    placement p1;
    shape_t::make_shape (sh3d1_, p1, _shape1_);
    shape_t::make_shape (sh3d2_, p2_, _shape2_);
    return;
  }
    
  void 
  i_composite_shape_3d::set_shape2 (i_shape_3d * sh3d_, const placement & p_)
  {
    shape_t::make_shape (sh3d_, p_, _shape2_);
    return;
  }

  const i_composite_shape_3d::shape_t & 
  i_composite_shape_3d::get_shape1 () const
  {
    return _shape1_;
  }
  
  const i_composite_shape_3d::shape_t & 
  i_composite_shape_3d::get_shape2 () const
  {
    return _shape2_;
  }

  const i_composite_shape_3d::shape_t & 
  i_composite_shape_3d::get_shape (int i_) const
  {
    if (i_ == 0) return _shape1_;
    return _shape2_;
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
