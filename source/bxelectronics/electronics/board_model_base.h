// -*- mode: c++; -*-
#ifndef ELECTRONICS_BOARD_MODEL_BASE_H
#define ELECTRONICS_BOARD_MODEL_BASE_H

#include <electronics/component_model_base.h>

namespace electronics {

  /// \brief Generic base model of a electronic board
  /**
   *
   *  Board model:
   *  +--------------------------------+
   *  |                                |
   *  |     Slot 0 +-------------+     |
   *  |            | Mezzanine 0 |     |
   *  |            |             |     |
   *  |            |             |     |
   *  |            +-------------+     |
   *  |                                |
   *  |     Slot 1 +-------------+     |
   *  |            | Mezzanine 1 |     |
   *  |            |             |     |
   *  |            |             |     |
   *  |            +-------------+     |
   *  |                                |
   *  |     Slot 2 + - - - - - - +     |
   *  |            :   <empty>   :     |
   *  |            :             :     |
   *  |            :             :     |
   *  |            + - - - - - - +     |
   *  |                                |
   *  +--------------------------------+
   *
   *
   *
   */
  class board_model_base : public component_model_base
  {
  public:

    /// Default constructor
    board_model_base();

    /// Destructor
    virtual ~board_model_base();

    /// Return the slot width
    uint32_t get_slot_width() const;

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
    physical_component &
    add_mezzanine_board(uint32_t mezzanine_slot_,
                        const component_model_base & mezzanine_model_,
                        const std::string & mezzanine_label_ = "");

    /// Remove a mezzanine board
    void remove_mezzanine_board(uint32_t mezzanine_slot_);

    /// Return a non mutable reference to an embedded mezzanine_board by slot
    const physical_component & get_mezzanine_board(uint32_t mezzanine_slot_);

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

    /// Common initialization of the board
    void _board_initialize(const datatools::properties & config_,
                           component_model_pool_type& components_);

    /// Common termination of the board
    void _board_reset();

    /// Post remove
    virtual void _post_remove_embedded_physical(const std::string & embedded_label_);

  private:

    std::string _format_; //!< Format of the board
    uint32_t    _slot_width_; //!< Width of the board in slot unit
    uint32_t    _max_number_of_mezzanine_boards_; //!< Maximum number of mezzanine boards
    std::string _mezzanine_format_; //!< Format of the embedded mezzanine boards
    indexed_labels_dict_type _mezzanine_labels_; //!< List of mezzanine labels by slot index

    // Automated registration in the system factory register
    ELECTRONICS_COMPONENT_REGISTRATION_INTERFACE(board_model_base);

  };

} // end of namespace electronics

#endif // ELECTRONICS_BOARD_MODEL_BASE_H
