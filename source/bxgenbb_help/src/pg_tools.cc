#include <genbb_help/i_genbb.h>
#include <genbb_help/detail/pg_tools.h>

#include <datatools/service_manager.h>
#include <datatools/ioutils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

#include <mygsl/rng.h>

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

    const manager & pg_entry_type::get_manager() const
    {
      return *_manager_;
    }

    manager & pg_entry_type::grab_manager()
    {
      return *_manager_;
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
      DT_THROW_IF (_status_ != STATUS_BLANK,
                   std::logic_error,
                   "Cannot modify the configuration of PG named '"
                   << get_name () << "' !");
      return _config_;
    }

    void pg_entry_type::set_config (const datatools::properties & sc_)
    {
      DT_THROW_IF (_status_ != STATUS_BLANK,
                   std::logic_error,
                   "Cannot modify the configuration of PG named '"
                   << get_name () << "' !");
      _config_ = sc_;
      return;
    }

    const std::string & pg_entry_type::get_id () const
    {
      return _id_;
    }

    void pg_entry_type::set_id (const std::string & sid_)
    {
      DT_THROW_IF (sid_.empty(),
                   std::logic_error,
                   "Empty particle generator ID is not allowed !");
      _id_ = sid_;
      return;
    }

    const std::string & pg_entry_type::get_name () const
    {
      return _name_;
    }

    void pg_entry_type::set_name (const std::string & sn_)
    {
      DT_THROW_IF (sn_.empty(),
                   std::logic_error,
                   "Empty particle generator ID is not allowed !");
      _name_ = sn_;
      return;
    }

    const i_genbb & pg_entry_type::get () const
    {
      return _handle_.get();
    }

    i_genbb & pg_entry_type::grab ()
    {
      return _handle_.grab();
    }

    bool pg_entry_type::has () const
    {
      return _handle_.has_data();
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

    void create(pg_entry_type & entry_,
                datatools::factory_register<i_genbb> * factory_,
                mygsl::rng * external_random_)
    {
      if (entry_.is_created()) return;
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                    "Creating the particle generator entry with name '"
                    <<  entry_.get_name()
                    << "'...");

      datatools::factory_register<i_genbb> * fact_reg = factory_;
      if (fact_reg == 0) {
        fact_reg = &DATATOOLS_FACTORY_GRAB_SYSTEM_REGISTER(genbb::i_genbb);
      }
      // if (entry_.get_config ().has_flag("debug")) {
      //   fact_reg->tree_dump(std::clog, "The current i_genbb factory : ", "DEBUG: genbb::detail::create: ");
      // }
      // Search for the PG's label in the factory dictionary:
      DT_THROW_IF (!fact_reg->has(entry_.get_id()),
                   std::logic_error,
                   "Cannot find PG factory with ID '"
                   << entry_.get_id()
                   << "' for particle generator named '"
                   << entry_.get_name() << "' !");
      typedef datatools::factory_register<i_genbb>::factory_type FactoryType;
      const FactoryType & the_factory = fact_reg->get(entry_.get_id());
      i_genbb* ptr = the_factory();
      entry_.grab_handle().reset(ptr);
      entry_.update_status(detail::pg_entry_type::STATUS_CREATED);

      if (external_random_ != 0 && ptr->can_external_random()) {
        DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                      "Set the external PRNG for the particle generator '"
                      <<  entry_.get_name()
                      << "'");
        ptr->set_external_random(*external_random_);
      }
      return;
    }

    void initialize(pg_entry_type& entry_,
                    datatools::service_manager * service_manager_,
                    detail::pg_dict_type * dictionary_,
                    datatools::factory_register<i_genbb> * factory_,
                    mygsl::rng * external_random_)
    {
      if (entry_.is_initialized()) return;
      if (!entry_.is_created()) {
        create(entry_,factory_,external_random_);
      }
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                    "Initializing the particle generator entry with name '"
                    <<  entry_.get_name()
                    << "'...");
      i_genbb& the_pg = entry_.grab();
      if (service_manager_ != 0 && dictionary_ != 0) {
        the_pg.initialize(entry_.get_config(),
                          *service_manager_,
                          *dictionary_);
      }
      if (service_manager_ == 0 && dictionary_ != 0) {
        the_pg.initialize_with_dictionary_only(entry_.get_config(),
                                               *dictionary_);
      }
      if (service_manager_ == 0 && dictionary_ == 0) {
        the_pg.initialize_standalone(entry_.get_config());
      }
      entry_.update_status(detail::pg_entry_type::STATUS_INITIALIZED);
      return;
    }

    void reset(pg_entry_type&entry_)
    {
      if (entry_.is_initialized()) {
        i_genbb& the_pg = entry_.grab_handle().grab();
        the_pg.reset();
        entry_.reset_status(detail::pg_entry_type::STATUS_INITIALIZED);
      }
      return;
    }

  }  // end of namespace detail
}  // end of namespace genbb

