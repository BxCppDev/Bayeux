// -*- mode: c++; -*-
#ifndef ELECTRONICS_CRATE_MODEL_BASE_H
#define ELECTRONICS_CRATE_MODEL_BASE_H

#include <electronics/component_model_base.h>

namespace electronics {

  /// \brief The base class of an electronic crate
  /**
   *
   *        Crate model:
   *        +-----------------------------------+
   *        |+-+|+-+|   |   |+-+|   |   |   |   |
   *        ||B|||B|| ^ |   ||B|| ^ | ^ | ^ | ^ |
   *        ||o|||o|| e |   ||o|| e | e | e | e |
   *        ||a|||a|| m |   ||a|| m | m | m | m |
   *        ||r|||r|| p |   ||r|| p | p | p | p |
   *        ||d|||d|| t |   ||d|| t | t | t | t |
   *        || ||| || y |   || || y | y | y | y |
   *        ||0|||1|| v |   ||4|| v | v | v | v |
   *        || ||| ||   |   || ||   |   |   |   |
   *        || ||| ||   |   || ||   |   |   |   |
   *        |+-+|+-+|   |   |+-+|   |   |   |   |
   *        +-----------------------------------+
   *   Slot   0   1   2   3   4   5   6   7   8
   *
   *
   */
  class crate_model_base : public component_model_base
  {
  public:

    /// Default constructor
    crate_model_base();

    /// Destructor
    virtual ~crate_model_base();

    /// Set the max number of modules
    void set_max_number_of_modules(uint32_t);

    /// Return the max number of modules
    uint32_t get_max_number_of_modules();

    /// Check if the crate has a known format
    bool has_format() const;

    /// Return the format
    const std::string & get_format() const;

    /// Set the format
    void set_format(const std::string &);

    /// Check if there is a module at given slot
    bool has_module(uint32_t module_slot_) const;

    /// Add a module
    physical_component &
    add_module(uint32_t module_slot_,
               const component_model_base & module_model_,
               const std::string & module_label_ = "");

    /// Remove a module
    void remove_module(uint32_t module_slot_);

    /// Return a non mutable reference to an embedded module by slot
    const physical_component & get_module(uint32_t module_slot_);

    // Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const;

  protected:

    /// Overloaded initialization method
    virtual void _at_initialize(const datatools::properties & config_,
                                component_model_pool_type& components_);

    /// Overloaded reset method
    virtual void _at_reset();

    /// Common initialization of crate
    void _crate_initialize(const datatools::properties & config_,
                           component_model_pool_type& components_);

    /// Common termination of the crate
    void _crate_reset();

    /// Post remove
    virtual void _post_remove_embedded_physical(const std::string & embedded_label_);

  private:

    std::string _format_; //!< Format of the crate
    uint32_t    _max_number_of_modules_; //!< Maximum number of modules
    indexed_labels_dict_type _module_labels_; //!< List of module labels by slot index

    // Automated registration in the system factory register
    ELECTRONICS_COMPONENT_REGISTRATION_INTERFACE(crate_model_base);

  };

} // end of namespace electronics

#endif // ELECTRONICS_CRATE_MODEL_BASE_H
