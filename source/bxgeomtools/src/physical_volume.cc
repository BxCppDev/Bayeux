// -*- mode: c++ ; -*- 
/* physical_volume.cc
 */

#include <geomtools/physical_volume.h>

#include <stdexcept>
#include <sstream>
#include <geomtools/logical_volume.h>
#include <geomtools/i_placement.h>

namespace geomtools {

  using namespace std;  

  bool physical_volume::is_locked () const
  {
    return _locked_;
  }

  void physical_volume::lock ()
  {
    _locked_ = true;
    return;
  }

  void physical_volume::unlock ()
  {
    _locked_ = false;
    return;
  }
 
  const string & physical_volume::get_name () const
  {
    return _name_;
  }
  
  void physical_volume::set_name (const string & name_)
  {
    _name_ = name_;
    return;
  }
  
  const properties & physical_volume::parameters () const
  {
    return _parameters_;
  }
  
  properties & physical_volume::parameters ()
  {
    return _parameters_;
  }

  bool physical_volume::has_placement () const
  {
    return _placement_ != 0;
  }

  void physical_volume::set_placement (const i_placement * p_)
  {
    _clear_placement_ ();
    if (p_ != 0)
      {
        _own_placement_ = true;
        _placement_ = p_;
      }
    return;
  }

  void physical_volume::set_placement (const i_placement & p_)
  {
    _clear_placement_ ();
    _own_placement_ = false;
    _placement_ = &p_;
    return;
  }
  
  const i_placement & physical_volume::get_placement () const
  {
    if (! _placement_)
      {
        throw logic_error ("physical_volume::get_placement: Missing placement !");
      }
    return *_placement_;
  }

  bool physical_volume::has_logical () const
  {
    return _logical_ != 0;
  }

  void physical_volume::_clear_placement_ ()
  {
    if (_placement_ != 0)
      {
        if (_own_placement_)
          {
            delete _placement_;
          }
        _placement_ = 0;
        _own_placement_ = false;
      }
    return;
  }
 
  void physical_volume::_clear_logical_ ()
  {
    if (_logical_ != 0)
      {
        if (_own_logical_)
          {
            delete _logical_;
          }
        _logical_ = 0;
        _own_logical_ = false;
      }
    return;
  }
    
  void physical_volume::set_logical (const logical_volume & logical_)
  {
    _clear_logical_ ();
    _own_logical_ = false;
    _logical_ = &logical_;
    return;
  }

  void physical_volume::set_logical (const logical_volume * logical_)
  {
    _clear_logical_ ();
    if (logical_ != 0)
      {
        _own_logical_ = true;
        _logical_ = logical_;
      }
    return;
  }

  const logical_volume & physical_volume::get_logical () const
  {
    if (! _logical_)
      {
        throw logic_error ("physical_volume::get_logical: Missing logical !");
      }
    return *_logical_;
  }

  bool physical_volume::has_mother () const
  {
    return _mother_ != 0;
  }
    
  void physical_volume::set_mother (const logical_volume & mother_)
  {
    _mother_ = &mother_;
    logical_volume & the_mother = const_cast<logical_volume &> (mother_);
    the_mother.add_physical (*this, this->get_name ());
    return;
  }
  
  physical_volume::physical_volume ()
  {
    _locked_ = false;
    _own_placement_ = false;
    _placement_ = 0;
    _own_logical_ = false;
    _logical_ = 0;
    _mother_ = 0;
    return;
  }
  
  physical_volume::physical_volume (const string & name_)
  {
    _locked_ = false;
    _own_placement_ = false;
    _placement_ = 0;
    _own_logical_ = false;
    _logical_ = 0;
    _mother_ = 0;
    set_name (name_);
    return;
  }

  physical_volume::physical_volume (const string & name_, 
                                    const logical_volume & logical_,
                                    const logical_volume & mother_,
                                    const i_placement    & placement_)
  {
    _locked_ = false;
    _own_placement_ = false;
    _placement_ = 0;
    _own_logical_ = false;
    _logical_ = 0;
    _mother_ = 0;
    set_name (name_);
    set_logical (logical_);    
    set_mother (mother_);    
    set_placement (placement_);    
    return;
  }
  
  physical_volume::physical_volume (const string & name_, 
                                    const logical_volume * logical_,
                                    const logical_volume & mother_,
                                    const i_placement    & placement_)
  {
    _locked_ = false;
    _own_placement_ = false;
    _placement_ = 0;
    _own_logical_ = false;
    _logical_ = 0;
    set_name (name_);
    set_logical (logical_);    
    set_mother (mother_);    
    set_placement (placement_);    
    return;
  }

  physical_volume::physical_volume (const string & name_, 
                                    const logical_volume & logical_,
                                    const logical_volume & mother_,
                                    const i_placement    * placement_)
  {
    _locked_ = false;
    _own_placement_ = false;
    _placement_ = 0;
    _own_logical_ = false;
    _logical_ = 0;
    set_name (name_);
    set_logical (logical_);    
    set_mother (mother_);    
    set_placement (placement_);    
    return;
  }
  
  physical_volume::physical_volume (const string & name_, 
                                    const logical_volume * logical_,
                                    const logical_volume & mother_,
                                    const i_placement    * placement_)
  {
    _locked_ = false;
    _own_logical_ = false;
    _logical_ = 0;
    set_name (name_);
    set_logical (logical_);    
    set_mother (mother_);    
    set_placement (placement_);    
    return;
  }
 
  physical_volume::~physical_volume ()
  {
    _locked_ = false;
    _clear_placement_ ();
    _clear_logical_ ();
    return;
  }

  void physical_volume::tree_dump (ostream & out_, 
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

    out_ << indent << i_tree_dumpable::tag 
         << "Name      : \"" << _name_ << "\"" << std::endl;

    out_ << indent << i_tree_dumpable::tag 
         << "Locked    : " << (_locked_? "Yes": "No") << std::endl;

    {
      // parameters:
      out_ << indent << du::i_tree_dumpable::tag
           << "Parameters : ";
      if ( _parameters_.size () == 0) 
        {
          out_ << "<empty>"; 
        }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << du::i_tree_dumpable::skip_tag;
        _parameters_.tree_dump (out_,"",indent_oss.str ());
      }      
    }

    {
      // Logical:
      out_ << indent << i_tree_dumpable::tag 
           << "Logical : ";
      if (has_logical ())
        {
          out_ << "\"" << _logical_->get_name () << "\" " 
               << (_own_logical_? "(owned)": "(not owned)");
        }
      else
        {
          out_ << "<no logical>";
        }
      out_ << std::endl; 
    }

    {
      out_ << indent << i_tree_dumpable::tag 
           << "Placement : " << endl;
      ostringstream oss_indent;
      oss_indent << indent << i_tree_dumpable::skip_tag;
      _placement_->tree_dump (out_, 
                              "", 
                              oss_indent.str (), 
                              false);
    }

    {
      // Mother:
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
           << "Mother = ";
      if (has_mother ())
        {
          out_ << "\"" << _mother_->get_name () << "\"";
        }
      else
        {
          out_ << "<no mother>";
        }
      out_ << std::endl; 
    }
    return;
  }

} // end of namespace geomtools

// end of physical_volume.cc
