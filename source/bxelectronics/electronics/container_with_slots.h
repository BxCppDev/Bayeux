// -*- mode: c++; -*-
#ifndef FLSNELECTRONICS_CONTAINER_WITH_SLOTS_H
#define FLSNELECTRONICS_CONTAINER_WITH_SLOTS_H


namespace snemo {
namespace electronics {

  class container_with_slots : public datatools::i_tree_dumpable
  {
  public:

    /// Default constructor
    container_with_slots();

    /// Destructor
    virtual ~container_with_slots();

    /// Return the number of slots
    uint32_t get_number_of_slots() const;

    /// Set the slot width
    void set_number_of_slots(uint32_t);

    /// Check if there is a component label
    bool has_component_label() const;

    /// Return the component label
    const std::string & get_component_label() const;

    /// Set the component label
    void set_component_label(const std::string & label_);

    /// Check if there is a component at given slot
    bool has_component(uint32_t slot_) const;

    /// Add a component
    void add_component(uint32_t slot_,
                       component_handle_type & compo_);

    /// Remove a component
    void remove_component(uint32_t slot_);

    /// Return a mutable handle to the component at given slot
    component_handle_type & grab_component(uint32_t slot_);

    /// Return a non mutable handle to the component at given slot
    const component_handle_type & get_component(uint32_t slot_) const;

    /// Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const;

    /// Check initialization flag
    bool is_initialized() const;

    /// Initialization
    void initialize();

    /// Reset
    void reset();

    /// Check lock flag
    bool is_locked() const;

    /// Lock
    void lock();

    /// Unlock
    void unlock();


  private:

    bool _initialized_; //!< Initialization flag
    bool _locked_; //!< Lock flag
    component_type _component_types_; //!< Type of embedded components
    std::string _component_label_; //!< Label used to address embedded components
    uint32_t _number_of_slots_; //!< Number of available slots
    indexed_component_dict_type _components_; //!< Embedded components addressed by slot


  };

} // end of namespace electronics
} // end of namespace snemo

#endif // FLSNELECTRONICS_CONTAINER_WITH_SLOTS_H
