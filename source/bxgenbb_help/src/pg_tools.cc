#include <genbb_help/i_genbb.h>
#include <genbb_help/detail/pg_tools.h>

namespace genbb {
  namespace detail {
 
      
      bool pg_entry_type::has_manager () const
      {
        return _manager_ != 0;
      }

      void pg_entry_type::set_manager (manager & mgr_)
      {
        _manager_ = &mgr_;
        return;
      }

      void pg_entry_type::reset_manager ()
      {
        _manager_ = 0;
        return;
      }

    uint32_t pg_entry_type::get_status() const
      {
        return _status_;
      }
 
    void pg_entry_type::update_status(uint32_t bits)
    {
      _status_ |= bits;
      return;
    }

    void pg_entry_type::reset_status(uint32_t bits)
    {
      _status_ ^= bits;
      return;
    }

    const datatools::properties & pg_entry_type::get_config () const
    {
      return _config_;
    }

    datatools::properties & pg_entry_type::grab_config ()
      {
        if (_status_ != STATUS_BLANK)
          {
            std::ostringstream message;
            message << "genbb::detail::pg_entry_type::grab_config: "
                    << "Cannot modify the configuration of PG named '" 
                    << get_name () << "' !";
            throw std::logic_error(message.str());
          }
        return _config_;
      }
  
    void pg_entry_type::set_config (const datatools::properties & sc_)
    {
      if (_status_ != STATUS_BLANK)
        {
          std::ostringstream message;
          message << "genbb::detail::pg_entry_type::set_config: "
                  << "Cannot modify the configuration of PG named '" 
                  << get_name () << "' !";
          throw std::logic_error(message.str());
        }
      _config_ = sc_; 
      return;
    }

    const std::string & pg_entry_type::get_id () const
    {
      return _id_;
    }

    void pg_entry_type::set_id (const std::string & sid_)
    {
      if (sid_.empty())
        {
          std::ostringstream message;
          message << "genbb::detail::pg_entry_type::set_id: "
                  << "Empty PG ID is not allowed !";
          throw std::logic_error(message.str());
        }  
      _id_ = sid_;
      return;
    }

    const std::string & pg_entry_type::get_name () const
    {
      return _name_;
    }

    void pg_entry_type::set_name (const std::string & sn_)
    {
      if (sn_.empty())
        {
          std::ostringstream message;
          message << "genbb::detail::pg_entry_type::set_name: "
                  << "Empty PG name is not allowed !";
          throw std::logic_error(message.str());
        }
      _name_ = sn_;
      return; 
    }
  

    const pg_handle_type & pg_entry_type::get_handle() const
    {
      return _handle_;
    }

    pg_handle_type & pg_entry_type::grab_handle()
      {
        return _handle_;
      }


    pg_entry_type::pg_entry_type() {
      _status_ = STATUS_BLANK;
      _handle_.reset();
      return;
    }

    bool pg_entry_type::is_created() const
      {
        return _status_ & STATUS_CREATED;
      }

    bool pg_entry_type::is_initialized() const
      {
        return _status_ & STATUS_INITIALIZED;
      }

    void pg_entry_type::tree_dump(std::ostream& out, 
                             const std::string& title,
                             const std::string& a_indent,
                             bool a_inherit) const {
      std::string indent;
      if (!a_indent.empty()) indent = a_indent;

      if (!title.empty()) out << indent << title << std::endl;

      out << indent << datatools::i_tree_dumpable::tag 
          << "name     : '" 
          << _name_ 
          << "'" << std::endl;

      out << indent << datatools::i_tree_dumpable::tag 
          << "ID       : '" 
          << _id_ 
          << "'" << std::endl;


      out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit) 
          << "Status   : " 
          << _status_;
      {
        size_t count = 0;
        std::ostringstream status_info;
        if (_status_ & pg_entry_type::STATUS_CREATED) {
          if (count) status_info << ',';
          status_info << "created";
          count++;
        }
        if (_status_ & pg_entry_type::STATUS_INITIALIZED) {
          if (count) status_info << ',';
          status_info << "initialized";
          count++;
        }
        if (count) {
          out << ' ' << '(' 
              << status_info.str() 
              << ')';
        }
      }
      out << std::endl;
    }

  }  // end of namespace detail
}  // end of namespace genbb

