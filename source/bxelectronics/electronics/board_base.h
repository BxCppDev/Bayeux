// -*- mode: c++; -*-
#ifndef ELECTRONICS_BOARD_BASE_H
#define ELECTRONICS_BOARD_BASE_H

#include <electronics/component_base.h>

namespace electronics {

  class board_base : public component_base
  {
  public:

    /// Default constructor
    board_base();

    /// Destructor
    virtual ~board_base();

    /// Return the slot width
    uint32_t get_slot_with() const;

    /// Set the slot width
    void set_slot_width(uint32_t);

    /// Return the max number of mezzanine boards
    uint32_t get_max_number_of_mezzanine_boards() const;

    /// Set the max number of mezzanine boards
    void set_max_number_of_mezzanine_boards(uint32_t);

    /// Check if the board has a known format
    bool has_format() const;

    /// Return the format
    const std::string & get_format() const;

    /// Set the format
    void set_format(const std::string &);

    /// Check if board has a known format for mezzanine boards
    bool has_mezzanine_format() const;

    /// Return the mezzanine format
    const std::string & get_mezzanine_format() const;

    /// Set the mezzanine format
    void set_mezzanine_format(const std::string &);

    /// Check if there is a mezzanine board at given slot
    bool has_mezzanine_board(uint32_t mezzanine_slot_) const;

    /// Add a mezzanine board
    void add_mezzanine_board(uint32_t mezzanine_slot_,
                             component_handle_type & mezzanine_);

    /// Remove a mezzanine board
    void remove_mezzanine_board(uint32_t mezzanine_slot_);

    /// Return a mutable handle to the mezzanine board
    component_handle_type & grab_mezzanine_board(uint32_t mezzanine_slot_);

    /// Return a non mutable handle to the mezzanine board
    const component_handle_type & get_mezzanine_board(uint32_t mezzanine_slot_) const;

    /* ***********
     * INTERFACE *
     *************/

    /// Initialize the board
    virtual void initialize(const datatools::properties& config_,
                            component_pool_type& components_);

    /// Reset the board
    virtual void reset();

    // Smart print
    virtual void tree_dump(std::ostream& out_ = std::clog,
                           const std::string& title_  = "",
                           const std::string& indent_ = "",
                           bool inherit_ = false) const;

  protected:

    /// Common initialization of the board
    void _board_initialize(const datatools::properties & config_,
                           component_pool_type& components_);

    /// Common termination of the board
    void _board_reset();

  private:

    std::string _format_; //!< Format of the board
    uint32_t    _slot_width_; //!< Width of the board in slot units
    uint32_t    _max_number_of_mezzanine_boards_; //!< Maximum number of mezzanine boards
    std::string _mezzanine_format_; //!< Format of the embedded mezzanine boards
    indexed_component_dict_type _mezzanine_boards_; //!< Embedded mezzanine boards addressed by slot

    // Automated registration in the system factory register
    ELECTRONICS_COMPONENT_REGISTRATION_INTERFACE(board_base);

  };

  typedef board_base module_base;

} // end of namespace electronics

#endif // ELECTRONICS_BOARD_BASE_H
