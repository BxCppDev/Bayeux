// -*- mode: c++ ; -*-
/* pg_tools.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-02-25
 * Last modified: 2013-02-25
 *
 */

#ifndef GENBB_DETAIL_PG_TOOLS_H
#define GENBB_DETAIL_PG_TOOLS_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/bit_mask.h>
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/factory.h>

namespace datatools {
  class service_manager;
}

namespace mygsl {
  class rng;
}

namespace genbb {

  /// Class forward declaration
  class i_genbb;
  //class i_genbb::factory_register_type;

  /// Handle on GENBB particle generator
  typedef datatools::handle<i_genbb> pg_handle_type;

  class manager;

  namespace detail {

    class pg_entry_type
    {
    public:

      enum status_type
      {
        STATUS_BLANK             = 0,
        STATUS_CREATED           = datatools::bit_mask::bit00,
        STATUS_INITIALIZED       = datatools::bit_mask::bit01
      };

    public:

      uint32_t get_status() const;

      void update_status(uint32_t);

      void reset_status(uint32_t);

      const datatools::properties & get_config () const;

      datatools::properties & grab_config ();

      void set_config (const datatools::properties &);

      const std::string & get_name () const;

      void set_name (const std::string &);

      const std::string & get_id () const;

      void set_id (const std::string &);

      bool has_manager () const;

      void set_manager (manager &);

      void reset_manager ();

      const manager & get_manager() const;

      manager & grab_manager();

      void set_blank ();

      void set_created ();

      void set_initialized ();

      void set_uninitialized ();

      bool is_created () const;

      bool is_initialized () const;

      /// Constructor
      pg_entry_type ();

      /// Check is the pg is referenced in the embedded handle
      bool has () const;

      const i_genbb & get () const;

      i_genbb & grab ();

      const pg_handle_type & get_handle () const;

      pg_handle_type & grab_handle ();

      pg_handle_type & grab_initialized_handle ();

      virtual void tree_dump (std::ostream & out_         = std::clog,
                              const std::string & title_  = "",
                              const std::string & indent_ = "",
                              bool inherit_               = false) const;


    private:

      std::string           _name_;    //!< The name of the PG
      std::string           _id_;      //!< The ID (type) of the PG
      datatools::properties _config_;  //!< The configuration of the PG
      uint32_t              _status_;  //!< The status of the PG
      pg_handle_type        _handle_;  //!< The handle for the allocated PG
      manager             * _manager_; //!< Particle generator manager

    };

    typedef std::map<std::string, pg_entry_type> pg_dict_type;

    void create(pg_entry_type & entry_,
                datatools::factory_register<i_genbb> * factory_,
                mygsl::rng * external_random_);

    void initialize(pg_entry_type & entry_,
                    datatools::service_manager * service_manager_,
                    detail::pg_dict_type * dictionary_,
                    datatools::factory_register<i_genbb> * factory_,
                    mygsl::rng * external_random_);

    void reset(pg_entry_type& entry_);

  } // end of namespace detail

} // end of namespace genbb

#endif // GENBB_DETAIL_PG_TOOLS_H
