// i_wires_drawer.cc

// Ourselves:
#include <geomtools/i_wires_drawer.h>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/gnuplot_draw.h>

namespace geomtools {

  i_wires_drawer::i_wires_drawer ()
  {
    return;
  }

  i_wires_drawer::~i_wires_drawer ()
  {
    return;
  }

  void i_wires_drawer::generate_wires_self(std::ostream & out_)
  {
    geomtools::vector_3d pos(0., 0., 0.);
    geomtools::rotation_3d rot;
    geomtools::create(rot, 0., 0., 0.);
    generate_wires(out_, pos, rot);
    return;
  }

} // end of namespace geomtools
