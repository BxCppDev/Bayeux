/// \file mctools/simulated_data.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-15
 * Last modified: 2013-03-08
 *
 * License:
 *
 * Description:
 *   Simulation data based on the 'genbb::primary_event'
 *   class from the 'genbb_help' package.
 *
 * History:
 *
 */

#ifndef MCTOOLS_SIMULATED_DATA_H
#define MCTOOLS_SIMULATED_DATA_H 1

// Standard Library:
#include <string>
#include <map>
#include <vector>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.h>
#include <datatools/i_clear.h>
#include <datatools/i_tree_dump.h>
#include <datatools/handle.h>
#include <datatools/bit_mask.h>
#include <datatools/properties.h>
// - Bayeux/genbb_help :
#include <genbb_help/primary_event.h>

// This project:
#include <mctools/base_step_hit.h>

namespace mctools {

  /// The container of collections of MC hits
  class simulated_data :
    DATATOOLS_SERIALIZABLE_CLASS,
    public datatools::i_tree_dumpable,
    public datatools::i_clear
  {
  public:

    /// Type of the memory layout of the collection of MC hits
    enum collection_type
      {
        INVALID_HIT_COLLECTION_TYPE = -1,
        PLAIN_HIT_COLLECTION_TYPE   =  0,
        HANDLE_HIT_COLLECTION_TYPE  =  1,
      };

    /// Categories of MC hits
    enum hit_category_type
      {
        HIT_CATEGORY_TYPE_PUBLIC  = datatools::bit_mask::bit00,
        HIT_CATEGORY_TYPE_PRIVATE = datatools::bit_mask::bit01,
        HIT_CATEGORY_TYPE_PREFIX  = datatools::bit_mask::bit02,
        HIT_CATEGORY_TYPE_ALL     = HIT_CATEGORY_TYPE_PUBLIC | HIT_CATEGORY_TYPE_PRIVATE
      };

    /// Alias for the MC base step hit
    typedef datatools::handle<base_step_hit>  hit_handle_type;
    typedef std::vector<hit_handle_type>      hit_handle_collection_type;
    typedef std::vector<base_step_hit>        hit_collection_type;

    /* Dictionary of lists of handles for base step hits.
     * Each list has its own string key which represents
     * the 'category' of simulated hits.
     */
    typedef std::map<std::string, hit_handle_collection_type> step_hits_dict_type;
    typedef std::map<std::string, hit_collection_type>        plain_step_hits_dict_type;

    typedef ::genbb::primary_event primary_event_type;

  public:

    /// Reset the memory layout for hit storage
    void reset_collection_type ();

    /// Set the memory layout for hit storage (collection of plain MC hits/collections of MC hits handles)
    void set_collection_type (int a_collection_type);

    /// Check if the memory layout for hit storage uses collection of plain MC hits
    bool use_plain_hit_collection () const;

    /// Check if the memory layout for hit storage uses collection of MC hits handles
    bool use_handle_hit_collection () const;

    /// Check if some collections of MC hits exist
    bool has_data () const;

    /// Check if a vertex vertex is defined
    bool has_vertex () const;

    /// Get a reference to the non mutable vertex
    const geomtools::vector_3d & get_vertex () const;

    /// Get a reference to the  mutable vertex
    geomtools::vector_3d & grab_vertex ();

    /// Set the vertex
    void set_vertex (const geomtools::vector_3d &);

    /// Get a reference to the non mutable primary event
    const primary_event_type & get_primary_event () const;

    /// Get a reference to the mutable primary event
    primary_event_type & grab_primary_event ();

    /// Set the primary event
    void set_primary_event (const primary_event_type &);

    /// Get a reference to the non mutable collection of auxiliary properties
    const datatools::properties & get_properties () const;

    /// Get a reference to the mutable collection of auxiliary properties
    datatools::properties & grab_properties ();

    // Set the collection of auxiliary properties
    void set_properties(const datatools::properties &);

    /// Get a reference to the mutable collection of MC hits handles
    step_hits_dict_type & grab_step_hits_dict ();

    /// Get a reference to the non mutable collection of MC hits handles
    const step_hits_dict_type & get_step_hits_dict () const;

    /// Get a reference to the mutable collection of plain MC hits
    plain_step_hits_dict_type & grab_plain_step_hits_dict ();

    /// Get a reference to the non mutable collection of plain MC hits
    const plain_step_hits_dict_type & get_plain_step_hits_dict () const;

    /// Get a list of categories associated to existing collections of MC hits
    void get_step_hits_categories (std::vector<std::string> & the_categories,
                                   unsigned int a_mode = HIT_CATEGORY_TYPE_ALL,
                                   const std::string & a_prefix = "") const;

    /// Add a new collection of MC hits with some given category and a default capacity for memory allocation
    simulated_data & add_step_hits (const std::string & a_category, size_t a_capacity = 0);

    /// Remove a collection of MC hits with some given category
    simulated_data & remove_step_hits (const std::string & a_category);

    /// Add/append a new MC hit in a collection of MC hits with some given category
    base_step_hit & add_step_hit (const std::string & a_category);

    /// Check is some MC hits exists in some given category
    bool has_step_hits (const std::string & a_category) const;

    /// Get the number of MC hits within a given category
    size_t get_number_of_step_hits (const std::string & a_category) const;

    /// Get a reference to the non mutable MC hit within a given category and index
    const base_step_hit & get_step_hit (const std::string & a_category, int a_hit_index) const;

    /// Get a reference to the mutable MC hit within a given category and index
    base_step_hit & grab_step_hit (const std::string & a_category, int a_hit_index);

    /// Get a reference to the mutable collection of MC hits handles with a given category
    hit_handle_collection_type & grab_step_hits (const std::string & a_category);

    /// Get a reference to the non mutable collection of MC hits handles with a given category
    const hit_handle_collection_type & get_step_hits (const std::string & a_category) const;

    /// Get a reference to the mutable collection of plain MC hits with a given category
    hit_collection_type & grab_plain_step_hits (const std::string & a_category);

    /// Get a reference to the non mutable collection of plain MC hits with a given category
    const hit_collection_type & get_plain_step_hits (const std::string & a_category) const;

    /// Reset the internal data
    simulated_data & reset (bool a_reset_collection_type = false);

    /// Default constructor
    simulated_data ();

    /// Constructor
    simulated_data (int a_collection_type);

    /// Destructor
    virtual ~simulated_data ();

    /*** interface i_clear ***/

    /// Reset the internal data
    virtual void clear ();

    /*** interface i_tree_dumpable ***/

    /// Smart print
    virtual void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const;

  private:

    // Attributes :
    geomtools::vector_3d         _vertex_;               //!< Common primary vertex
    primary_event_type           _primary_event_;        //!< Primary generated event
    datatools::properties        _properties_;           //!< List of properties
    int8_t                       _collection_type_;      //!< Storage type (handle/plain hits)
    step_hits_dict_type          _step_hits_dict_;       //!< Dictionary of collection of handle of hits (default type)
    plain_step_hits_dict_type    _plain_step_hits_dict_; //!< Dictionary of collection of plain hits

    /*** datatools/Boost/brio serialization ***/
    DATATOOLS_SERIALIZATION_DECLARATION();

#if MCTOOLS_WITH_REFLECTION == 1
    //! Reflection interface
    DR_CLASS_RTTI();
#endif

  };

} // end of namespace mctools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(mctools::simulated_data, "mctools::simulated_data")

#if MCTOOLS_WITH_REFLECTION == 1
// Activate reflection layer for the mctools::simulated_data class :
DR_CLASS_INIT(::mctools::simulated_data);
#endif // MCTOOLS_WITH_REFLECTION

#endif // MCTOOLS_SIMULATED_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
