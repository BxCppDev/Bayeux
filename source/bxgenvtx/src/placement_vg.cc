// -*- mode: c++ ; -*- 
/* placement_vg.cc
 */

#include <genvtx/placement_vg.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

namespace genvtx {
 
  using namespace std;
  
  const geomtools::placement & placement_vg::get_placement () const
  {
    return _placement_;
  }
  
  geomtools::placement & placement_vg::get_placement ()
  {
    return _placement_;
  }
  
  void placement_vg::set_placement (const geomtools::placement & new_value_)
  {
    _placement_ = new_value_;
    return;
  }

  bool placement_vg::has_vg () const
  {
    return _vg_ != 0;
  }

  bool placement_vg::is_owned () const
  {
    return _owned_;
  }
  
  const i_vertex_generator & placement_vg::get_vg () const
  {
    if (_vg_ == 0)
      {
        throw runtime_error ("genvtx::placement_vg::get_vg: Missing vertex generator !");
      }
    return *_vg_;
  }

  void placement_vg::_clear_vg_ ()
  {
    if (_vg_ != 0)
      {
        if (_owned_)
          {
            delete _vg_;
          }
        _vg_ = 0;
      }
    _owned_ = false;
    return;
  }

  void placement_vg::set_vg (i_vertex_generator & new_value_)
  {
    _clear_vg_ ();
    _owned_ = false;
    _vg_ = &new_value_;
    return;
  }
  
  void  placement_vg::set_vg (i_vertex_generator * new_value_)
  {
    _clear_vg_ ();
    _owned_ = true;
    _vg_ = new_value_;
    return;
  }
  
  
  // ctor/dtor:
  placement_vg::placement_vg ()
  {
    _owned_ = false;
    _vg_ = 0;
    return;
  }
  
  placement_vg::~placement_vg ()
  {
    _clear_vg_ ();
    return;
  }
  
  void placement_vg::_shoot_vertex (mygsl::rng & random_, 
                                    geomtools::vector_3d & vertex_)
  {
    bool devel = g_debug;
    if (_vg_ == 0)
      {
        throw runtime_error ("genvtx::placement_vg::_shoot_vertex: Missing vertex generator !");
      }
    if (! _placement_.is_valid ())
      {
        throw runtime_error ("genvtx::placement_vg::_shoot_vertex: Invalid placement !");       
      }
    geomtools::invalidate (vertex_);
    geomtools::vector_3d vertex;
    geomtools::invalidate (vertex);
    _vg_->shoot_vertex (random_, vertex);
    if (geomtools::is_valid (vertex))
      {
        _placement_.child_to_mother (vertex, vertex_);
      }
    return;
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  placement_vg::create (const datatools::utils::properties & configuration_, void * user_)
  {
    cerr << "DEVEL: genvtx::placement_vg::create: Entering..." << endl;
    configuration_.tree_dump (cerr, "genvtx::placement_vg::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    double x, y, z;
    double phi, theta, delta;
    x = y = z = 0.0;
    phi = theta = delta = 0.0;
    i_vertex_generator * vg = 0;

    // parameters of the cut:
    string generator_id;

    if (configuration_.has_key ("generator.id"))
      {
        generator_id = configuration_.fetch_string ("generator.id");
        if (! get_vertex_generator_db ().has_vertex_generator (generator_id))
          {
            ostringstream message;
            message << "genvtx::placement_vg::create: Invalid vertex generator id '"
                    << generator_id << "' !";
            throw runtime_error (message.str ());
          }
        vg_creator_type vgc = get_vertex_generator_db ().get_vertex_generator (generator_id);
        // vg = 
      }

    if (configuration_.has_key ("placement.x"))
      {
        x = configuration_.fetch_real ("placement.x");
      }

    if (configuration_.has_key ("placement.y"))
      {
        y = configuration_.fetch_real ("placement.y");
      }

    if (configuration_.has_key ("placement.z"))
      {
        z = configuration_.fetch_real ("placement.z");
      }

    if (configuration_.has_key ("placement.phi"))
      {
        phi = configuration_.fetch_real ("placement.phi");
      }

    if (configuration_.has_key ("placement.theta"))
      {
        theta = configuration_.fetch_real ("placement.theta");
      }

    if (configuration_.has_key ("placement.delta"))
      {
        delta = configuration_.fetch_real ("placement.delta");
      }

    // create a new parameterized 'placement_vg' instance:
    placement_vg * ptr = new placement_vg;
    geomtools::placement pl (x, y, z, phi, theta, delta);
    ptr->set_placement (pl);
    ptr->set_vg (vg);
    return ptr; 
  }

  string placement_vg::vg_id () const
  {
    return "genvtx::placement_vg";
  }

  vg_creator_type placement_vg::vg_creator () const
  {
    return placement_vg::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<placement_vg> placement_vg::g_cr_;
  
} // end of namespace genvtx

// end of placement_vg.cc
