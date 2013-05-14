// -*- mode: c++ ; -*-
/* vg_tools.cc
 */

#include <genvtx/detail/vg_tools.h>
#include <genvtx/i_vertex_generator.h>
#include <genvtx/manager.h>

#include <datatools/properties.h>
#include <sstream>
#include <stdexcept>

namespace genvtx {

  using namespace std;

  const string vg_tools::SHAPE_REF_NONE    = "";
  const string vg_tools::SHAPE_REF_PLAIN   = "<plain>";
  const string vg_tools::SHAPE_REF_GETTER  = "<getter>";

  const datatools::properties & vg_entry_type::get_vg_config () const
  {
    return _vg_config_;
  }

  datatools::properties & vg_entry_type::grab_vg_config ()
  {
    return _vg_config_;
  }

  void vg_entry_type::set_vg_config (const datatools::properties & config_)
  {
    _vg_config_ = config_;
    return;
  }

  const std::string & vg_entry_type::get_vg_name () const
  {
    return _vg_name_;
  }

  const std::string & vg_entry_type::get_vg_id () const
  {
    return _vg_id_;
  }

  void vg_entry_type::set_ptr (i_vertex_generator * ptr_)
  {
    _vg_handle_.reset (ptr_);
    return;
  }

  void vg_entry_type::set_vg_name (const std::string & n_)
  {
    _vg_name_ = n_;
    return;
  }

  void vg_entry_type::set_vg_id (const std::string & id_)
  {
    _vg_id_ = id_;
    return;
  }

  void vg_entry_type::set_blank ()
  {
    _vg_status_ = 0;
    return;
  }

  void vg_entry_type::set_created ()
  {
    _vg_status_ |= vg_entry_type::STATUS_CREATED;
    return;
  }

  void vg_entry_type::set_initialized ()
  {
    _vg_status_ |= vg_entry_type::STATUS_INITIALIZED;
    return;
  }

  void vg_entry_type::set_uninitialized ()
  {
    _vg_status_ ^= vg_entry_type::STATUS_INITIALIZED;
    return;
  }

  bool vg_entry_type::has_manager () const
  {
    return _manager_ != 0;
  }

  void vg_entry_type::set_manager (::genvtx::manager & mgr_)
  {
    _manager_ = &mgr_;
    return;
  }

  void vg_entry_type::reset_manager ()
  {
    _manager_ = 0;
    return;
  }

  vg_entry_type::vg_entry_type ()
  {
    _vg_status_ = 0;
    _manager_ = 0;
    return;
  }

  bool vg_entry_type::is_created () const
  {
    return _vg_status_ & STATUS_CREATED;
  }

  bool vg_entry_type::is_initialized () const
  {
    return _vg_status_ & STATUS_INITIALIZED;
  }

  bool vg_entry_type::has_vg () const
  {
    return _vg_handle_;
  }

  const i_vertex_generator & vg_entry_type::get_vg () const
  {
    return _vg_handle_.get ();
  }

  i_vertex_generator & vg_entry_type::grab_vg ()
  {
    return _vg_handle_.grab ();
  }

  const vg_handle_type & vg_entry_type::get_vg_handle () const
  {
    return _vg_handle_;
  }

  vg_handle_type & vg_entry_type::grab_vg_handle ()
  {
    return _vg_handle_;
  }

  vg_handle_type & vg_entry_type::grab_initialized_vg_handle ()
  {
    if (! has_vg ())
      {
        if (_manager_ == 0)
          {
            std::ostringstream message;
            message << "genvtx::vg_entry_type::grab_initialized_vg_handle: "
                    << "No manager is available to create the vg '" << this->_vg_name_
                    << "' ! ";
            throw std::logic_error (message.str ());
          }
        _manager_->create_vg (*this);
      }
    else
      {
        if (_manager_ == 0)
          {
            std::ostringstream message;
            message << "genvtx::vg_entry_type::vg_entry_type::grab_initialized_vg_handle: "
                    << "No manager is available to initialize the vg '" << this->_vg_name_
                    << "' ! ";
            throw std::logic_error (message.str ());

          }
        _manager_->initialize_vg (*this);
      }
    return _vg_handle_;
  }

  void vg_entry_type::tree_dump (std::ostream & out_ ,
                                 const std::string & title_,
                                 const std::string & indent_,
                                 bool inherit_) const
  {
    namespace du = datatools;
    std::string indent;
    if (! indent_.empty ())
      {
        indent = indent_;
      }
    if ( ! title_.empty () )
      {
        out_ << indent << title_ << std::endl;
      }

    out_ << indent << du::i_tree_dumpable::tag
         << "Vg name     : '" << _vg_name_ << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Vg ID       : '" << _vg_id_ << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Vg config   : " << _vg_config_.size () << " properties" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Vg status   : " << _vg_status_;
    {
      size_t count = 0;
      std::ostringstream status_info;
      if (is_created ())
        {
          if (count) status_info << ',';
          status_info << "created";
          count++;
        }
      if (is_initialized ())
        {
          if (count) status_info << ',';
          status_info << "initialized";
          count++;
        }
      if (count)
        {
          out_ << ' ' << '(' << status_info.str () << ')';
        }
    }
    out_ << std::endl;

    out_ << indent << du::i_tree_dumpable::tag
         << "Vg handle   : ";
    if (has_vg ())
      {
        const i_vertex_generator & bm = _vg_handle_.get ();
        out_ << "'" << "yes" << "'";
      }
    else
      {
        out_ << "<null>";
      }
    out_ << std::endl;

    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
         << "Manager : " << _manager_ << std::endl;

    return;
  }

} // end of namespace genvtx

// end of vg_tools.cc
