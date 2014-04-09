// -*- mode: c++; -*-
#ifndef ELECTRONICS_COMPONENT_UTILS_H
#define ELECTRONICS_COMPONENT_UTILS_H

// Standard library
#include <string>
#include <map>

// Third party
// - Boost
#include <boost/cstdint.hpp>

// - Bayeux/datatools
#include <datatools/handle.h>
#include <datatools/bit_mask.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

namespace electronics {

  /// Build a name from a prefix and an integral index
  std::string make_indexed_name(const std::string & prefix_,
                                uint32_t index_,
                                const std::string & suffix_= "");

  /// Build a name from a prefix and two integral indexes
  std::string make_double_indexed_name(const std::string & prefix_,
                                       uint32_t index1_,
                                       uint32_t index2_,
                                       const std::string & sep_ = "_",
                                       const std::string & suffix_= "");

  class component_model_base;
  typedef datatools::handle<component_model_base> component_model_handle_type;

  class component_manager;

  //! \brief Internal entry for component objects stored in the component manager class
  class component_model_entry : public datatools::i_tree_dumpable  {
  public:

    //! \brief Status of the component
    enum status_type {
      STATUS_BLANK        = 0x0,
      STATUS_CREATED      = datatools::bit_mask::bit00,
      STATUS_INITIALIZED  = datatools::bit_mask::bit01,
    };

  public:

    /// Return the component name
    const std::string & get_component_name () const;

    /// Set the component name
    void set_component_name (const std::string &);

    /// Return the component ID
    const std::string & get_component_id () const;

    /// Set the component ID
    void set_component_id (const std::string &);

    /// Return a non mutable reference to the component configuration
    const datatools::properties & get_component_config () const;

    /// Return a mutable reference to the component configuration
    datatools::properties & grab_component_config ();

    /// Set the component configuration
    void set_component_config (const datatools::properties &);

    /// Check if there is the component manager
    bool has_component_manager() const;

    /// Install a handle to the component manager
    void set_component_manager(component_manager &);

    /// Reset the handle to the component manager
    void reset_component_manager();

    /// Default constructor
    component_model_entry();

    /// Destructor
    ~component_model_entry();

    /// Return the component status
    uint32_t get_component_status() const;

    /// Update the component status
    void update_component_status(uint32_t);

    /// Reset the component status
    void reset_component_status(uint32_t);

    /// Check if the component is created
    bool is_created() const;

    /// Check if the component is initialized
    bool is_initialized() const;

    /// Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherits_ = false) const;

    /// Return a non mutable handle to the component
    const component_model_handle_type & get_component_model_handle() const;

    /// Return a mutable handle to the component
    component_model_handle_type & grab_component_model_handle();

    /// Return a mutable handle to the initialized component
    component_model_handle_type & grab_initialized_component_model_handle ();

  private:

    std::string _component_name_;             //!< The name of the component
    std::string _component_id_;               //!< The ID (type) of the component
    datatools::properties _component_config_; //!< The configuration of the component
    uint32_t _component_status_;              //!< The status of the component
    component_model_handle_type _component_model_handle_; //!< The handle for the allocated component
    component_manager * _component_manager_;  //!< Handle to a component manager

  };

  /// Dictionary of component model handles by label
  typedef std::map<std::string, component_model_handle_type> component_model_dict_type;

  /// Dictionary of component model handles by index
  typedef std::map<uint32_t, component_model_handle_type> indexed_component_model_dict_type;

  /// Dictionary of component model handles by index
  typedef std::map<uint32_t, std::string> indexed_labels_dict_type;

  /// Dictionary of component model entries
  typedef std::map<std::string, component_model_entry> component_model_pool_type;

  // Forward declaration
  class logical_component;

  /// Handle on logical component
  //typedef datatools::handle<logical_component> logical_component_handle_type;

  /// Dictionary of logical component handles
  //typedef std::map<std::string, const logical_component *> logical_component_dict_type;

  // Forward declaration
  class physical_component;

  /// Handle on physical component
  typedef datatools::handle<physical_component> physical_component_handle_type;

  /// Dictionary of physical component handles
  typedef std::map<std::string, physical_component_handle_type> physical_component_dict_type;

  /// Predicate to find a value in a map
  template<class T>
  struct map_data_compare : public std::binary_function<typename T::value_type,
                                                        typename T::mapped_type,
                                                        bool>
  {
  public:
    bool operator() (typename T::value_type &pair_,
                     typename T::mapped_type value_) const {
      return pair_.second == value_;
    }
  };

} // end of namespace electronics


#endif // ELECTRONICS_COMPONENT_UTILS_H
