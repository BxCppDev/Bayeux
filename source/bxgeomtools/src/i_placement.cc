// i_placement.cc

// Ourselves:
#include <geomtools/i_placement.h>

// This project:
#include <geomtools/placement.h>

namespace geomtools {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(i_placement, "geomtools::i_placement")

  i_placement::i_placement ()
  {
    return;
  }

  i_placement::~i_placement ()
  {
    return;
  }

  bool i_placement::has_only_one_rotation () const
  {
    return false;
  }

  bool i_placement::is_multiple () const
  {
    return get_number_of_items () > 0;
  }

  placement i_placement::get_placement (int item_) const
  {
    placement p;
    get_placement (item_, p);
    return p;
  }

  placement i_placement::get_placement_by_index (int item_) const
  {
    placement p;
    get_placement (item_, p);
    return p;
  }

  void i_placement::compute_placement (int item_, placement & p_) const
  {
    this->get_placement(item_,p_);
  }

  void i_placement::tree_dump (std::ostream & out_,
                               const std::string & title_,
                               const std::string & indent_,
                               bool inherit_) const
  {
    if (! title_.empty ()) {
      out_ << indent_ << title_ << std::endl;
    }
    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Number of items  = " << get_number_of_items() << std::endl;
    return;
  }

} // end of namespace geomtools
