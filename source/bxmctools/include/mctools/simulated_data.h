/// \file mctools/simulated_data.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-15
 * Last modified: 2018-08-30
 *
 * License: GPL3
 *
 * Description:
 *
 *   Simulation data based on the 'genbb::primary_event'
 *   class from the 'genbb_help' package.
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
  class simulated_data
    : public datatools::i_serializable
    , public datatools::i_tree_dumpable
    , public datatools::i_clear
  {
  public:

    /// Type of the memory layout of the collection of MC hits
    enum collection_type {
      INVALID_HIT_COLLECTION_TYPE = -1, //!< Invalid type of hit collection
      PLAIN_HIT_COLLECTION_TYPE   =  0, //!< @deprecated Collection of plain hits
      HANDLE_HIT_COLLECTION_TYPE  =  1  //!< Collection of hit handles
    };

    /// Categories of MC hits
    enum hit_category_type {
      HIT_CATEGORY_TYPE_PUBLIC  = datatools::bit_mask::bit00,
      HIT_CATEGORY_TYPE_PRIVATE = datatools::bit_mask::bit01,
      HIT_CATEGORY_TYPE_PREFIX  = datatools::bit_mask::bit02,
      HIT_CATEGORY_TYPE_ALL     = HIT_CATEGORY_TYPE_PUBLIC | HIT_CATEGORY_TYPE_PRIVATE
    };

    /// Alias for the MC base step hit handle type
    typedef datatools::handle<base_step_hit>  hit_handle_type;
    
    /// Alias for the collection of MC base step hit handles 
    typedef std::vector<hit_handle_type>      hit_handle_collection_type;
    
    /// Alias for the collection of MC base step hits 
    typedef std::vector<base_step_hit>        hit_collection_type;

    /** Dictionary of lists of handles to base step hits.
     * Each list has its own string key which represents
     * the 'category' of simulated hits.
     */
    typedef std::map<std::string, hit_handle_collection_type> step_hits_dict_type;
    
    /** Dictionary of lists of base step hits.
     * Each list has its own string key which represents
     * the 'category' of simulated hits.
     */
    typedef std::map<std::string, hit_collection_type>        plain_step_hits_dict_type;

    /// Alias for the primary generated event type
    typedef ::genbb::primary_event primary_event_type;

    /// Reset the memory layout for hit storage
    void reset_collection_type();

    /// Set the memory layout for hit storage (collection of plain MC hits/collections of MC hits handles)
    void set_collection_type(const int collection_type_);

    /// Check if the memory layout for hit storage uses collection of plain MC hits
    bool use_plain_hit_collection() const;

    /// Check if the memory layout for hit storage uses collection of MC hits handles
    bool use_handle_hit_collection() const;

    /// Check if some collections of MC hits exist
    bool has_data() const;

    /// Check if a vertex is defined
    bool has_vertex() const;

    /// Get a reference to the non mutable vertex
    const geomtools::vector_3d & get_vertex() const;

    /// Get a reference to the  mutable vertex
    geomtools::vector_3d & grab_vertex();

    /// Set the vertex
    void set_vertex(const geomtools::vector_3d &);

    /// Check if a time is defined
    bool has_time() const;

    /// Get the reference time
    double get_time() const;

    /// Set the reference time
    void set_time(const double);

    /// Get a reference to the non mutable primary event
    const primary_event_type & get_primary_event() const;

    /// Get a reference to the mutable primary event
    primary_event_type & grab_primary_event();

    /// Set the primary event
    void set_primary_event(const primary_event_type &);

    /// Get a reference to the non mutable collection of auxiliary properties
    const datatools::properties & get_properties() const;

    /// Get a reference to the mutable collection of auxiliary properties
    datatools::properties & grab_properties();

    // Set the collection of auxiliary properties
    void set_properties(const datatools::properties &);
    
    /// Get a reference to the mutable collection of MC hits handles
    step_hits_dict_type & grab_step_hits_dict();

    /// Get a reference to the non mutable collection of MC hits handles
    const step_hits_dict_type & get_step_hits_dict() const;

    /// Get a reference to the mutable collection of plain MC hits
    plain_step_hits_dict_type & grab_plain_step_hits_dict();

    /// Get a reference to the non mutable collection of plain MC hits
    const plain_step_hits_dict_type & get_plain_step_hits_dict() const;

    /// Get a list of categories associated to existing collections of MC hits
    void get_step_hits_categories(std::vector<std::string> & categories_,
                                  unsigned int mode_ = HIT_CATEGORY_TYPE_ALL,
                                  const std::string & prefix_ = "") const;

    /// Add a new collection of MC hits with some given category and a default capacity for memory allocation
    simulated_data & add_step_hits(const std::string & category_, size_t capacity_ = 0);

    /// Remove a collection of MC hits with some given category
    simulated_data & remove_step_hits(const std::string & category_);

    /// Add/append a new MC hit in a collection of MC hits with some given category
    base_step_hit & add_step_hit(const std::string & category_);

    /// Check is some MC hits exists in some given category
    bool has_step_hits(const std::string & category_) const;

    /// Get the number of MC hits within a given category
    size_t get_number_of_step_hits(const std::string & category_) const;

    /// Get a reference to the non mutable MC hit within a given category and index
    const base_step_hit & get_step_hit(const std::string & category_, int hit_index_) const;

    /// Get a reference to the mutable MC hit within a given category and index
    base_step_hit & grab_step_hit(const std::string & category_, int hit_index_);

    /// Add/append a new MC hit in a collection of MC hits with some given category
    template <class Hit>
    Hit & add_hit(const std::string & category_)
    {
      Hit * h = nullptr;
      if (use_handle_hit_collection()) {
        step_hits_dict_type::iterator found
          = _step_hits_dict_.find(category_);
        DT_THROW_IF(found == _step_hits_dict_.end(),
                    std::logic_error,
                    "No collection of hits with category '" << category_ << "' !");
        datatools::handle<mctools::base_step_hit> hh(dynamic_cast<mctools::base_step_hit*>(new Hit));
        found->second.push_back(hh);
        h = dynamic_cast<Hit*>(&found->second.back().grab());
      } else {
        DT_THROW(std::logic_error, "Unsupported method for plain hit collection!");
      }
      return *h;
    }

    bool has_hit(const std::string & category_, const int index_) const;

    template <class Hit>
    bool is_hit(const std::string & category_, const int index_) const
    {
      DT_THROW_IF(!use_handle_hit_collection(), std::logic_error, "Unsupported method for plain hit collection!");
      step_hits_dict_type::const_iterator found = _step_hits_dict_.find(category_);
      DT_THROW_IF(found == _step_hits_dict_.end(),
                  std::logic_error,
                  "No collection of handles of hits with category '" << category_ << "' !");
      DT_THROW_IF(index_ < 0 || index_ >=(int)found->second.size(),
                  std::logic_error,
                  "Invalid hit index in category '" << category_ << "' !");
      DT_THROW_IF(! found->second[index_].has_data(),
                  std::logic_error,
                  "Null handle at index " << index_ << " in category '" << category_ << "' !");
      // Extract the hit:
      const base_step_hit * bsh = &found->second[index_].get();
      // Attempt to cast it to the proper type:
      if (dynamic_cast<const Hit *>(bsh) == nullptr) {
        return false;
      }
      return true;
    }
  
    template <class Hit>
    const Hit & get_hit(const std::string & category_, const int index_) const
    {
      DT_THROW_IF(!use_handle_hit_collection(), std::logic_error, "Unsupported method for plain hit collection!");
      step_hits_dict_type::const_iterator found = _step_hits_dict_.find(category_);
      DT_THROW_IF(found == _step_hits_dict_.end(),
                  std::logic_error,
                  "No collection of hits with category '" << category_ << "' !");
      DT_THROW_IF(index_ < 0 || index_ >=(int)found->second.size(),
                  std::logic_error,
                  "Invalid hit index in category '" << category_ << "' !");
      DT_THROW_IF(! found->second[index_].has_data(),
                  std::logic_error,
                  "Null handle at index " << index_ << " in category '" << category_ << "' !");
      // Extract the hit:
      const base_step_hit * bsh = &found->second[index_].get();
      // Attempt to cast it to the proper type:
      const Hit * h = dynamic_cast<const Hit*>(bsh);
      if (h == nullptr) {
        DT_THROW(std::logic_error, "Invalid type for hit #" << index_ << " in category '" << category_ << "'!");
      } 
      return *h;
    }
      
    /// Get a reference to the mutable collection of MC hits handles with a given category
    hit_handle_collection_type & grab_step_hits(const std::string & category_);

    /// Get a reference to the non mutable collection of MC hits handles with a given category
    const hit_handle_collection_type & get_step_hits(const std::string & category_) const;

    /// @deprecated Get a reference to the mutable collection of plain MC hits with a given category
    hit_collection_type & grab_plain_step_hits(const std::string & category_);

    /// @deprecated Get a reference to the non mutable collection of plain MC hits with a given category
    const hit_collection_type & get_plain_step_hits(const std::string & category_) const;

    /// @deprecated Reset the internal data
    simulated_data & reset(bool reset_collection_type_);

    /// Reset data
    void reset();

    /// Default constructor
    simulated_data();

    /// Constructor
    explicit simulated_data(int collection_type_);

    /// Destructor
    ~simulated_data() override;

    /// Reset the internal data
    void clear() override;

    /// \deprecated Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const override;
    
    //! Smart print
    //! 
    //! Supported options:
    //! \code
    //! {
    //!   "title"    : "My title: ",
    //!   "indent"   : "[debug] ",
    //!   "inherit"  : false,
    //!   "list_hits" : false
    //! }
    //! \endcode
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = datatools::i_tree_dumpable::empty_options()) const override;
  

  protected:

    /// Set default values to attributes
    void _set_defaults();

  private:

    // Attributes :
    geomtools::vector_3d      _vertex_;               //!< Common primary vertex (optional)
    double                    _time_;                 //!< Common primary time (optional)
    primary_event_type        _primary_event_;        //!< Primary generated event
    datatools::properties     _properties_;           //!< List of auxiliary properties
    int8_t                    _collection_type_;      //!< Storage type (handle/plain hits)
    step_hits_dict_type       _step_hits_dict_;       //!< Dictionary of collections of handle of hits (default type)
    plain_step_hits_dict_type _plain_step_hits_dict_; //!< Dictionary of collections of plain hits

    // datatools/Boost/brio serialization:
    DATATOOLS_SERIALIZATION_DECLARATION()

#if MCTOOLS_WITH_REFLECTION == 1
    //! Reflection interface
    DR_CLASS_RTTI()
#endif

  };

} // end of namespace mctools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(mctools::simulated_data, "mctools::simulated_data")

#if MCTOOLS_WITH_REFLECTION == 1
// Activate reflection layer for the mctools::simulated_data class :
DR_CLASS_INIT(::mctools::simulated_data)
#endif // MCTOOLS_WITH_REFLECTION

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(mctools::simulated_data, 3)

#endif // MCTOOLS_SIMULATED_DATA_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
