// i_wires_drawer.cc

// Ourselves:
#include <geomtools/i_wires_drawer.h>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/gnuplot_draw.h>

namespace geomtools {

  /*
  base_wires_drawer::base_wires_drawer ()
  {
    _options_ = 0;
    return;
  }

  base_wires_drawer::~base_wires_drawer ()
  {
    return;
  }

  void base_wires_drawer::generate_wires_self(std::list<polyline_3d> & wires_,
                                              uint32_t options_) const
  {
    if (!has_wires()) {
    }
    return;
  }

  bool base_wires_drawer::has_wires() const
  {
    return _cached_wires_.get() != 0;
  }

  void base_wires_drawer::clear_wires()
  {
    _cached_wires_.reset();
    return;
  }

  void base_wires_drawer::update_wires()
  {
    _cached_wires_.reset(new std::list<polyline_3d>);
    std::list<polyline_3d> & wires = *_cached_wires_.get();
    generate_wires_self(wires, _options_);
    return;
  }

  const std::list<polyline_3d> & base_wires_drawer::get_wires() const
  {
    DT_THROW_IF(!has_wires(), std::logic_error,
                "Missing cached wires!");
    return *_cached_wires_.get();
  }
*/

} // end of namespace geomtools
