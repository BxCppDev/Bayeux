// -*- mode: c++ ; -*- 
/* cache_vg.cc
 */

#include <genvtx/cache_vg.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

namespace genvtx {
  
  const geomtools::cache & cache_vg::get_cache () const
  {
    return _cache_;
  }
  
  geomtools::cache & cache_vg::get_cache ()
  {
    return _cache_;
  }
  
  void cache_vg::set_cache (const geomtools::cache & new_value_)
  {
    _cache_ = new_value_;
    return;
  }

  bool cache_vg::has_vg () const
  {
    return _vg_ != 0;
  }

  bool cache_vg::is_owned () const
  {
    return _owned_;
  }
  
  const i_vertex_generator & cache_vg::get_vg () const
  {
    if (_vg_ == 0)
      {
        throw runtime_error ("genvtx::cache_vg::get_vg: Missing vertex generator !");
      }
    return *_vg_;
  }

  void cache_vg::_clear_vg_ ()
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

  void cache_vg::set_vg (i_vertex_generator & new_value_)
  {
    _clear_vg_ ();
    _owned_ = false;
    _vg_ = &new_value_;
    return;
  }
  
  void  cache_vg::set_vg (i_vertex_generator * new_value_)
  {
    _clear_vg_ ();
    _owned_ = true;
    _vg_ = new_value_;
    return;
  }
  
  
  // ctor/dtor:
  cache_vg::cache_vg ()
  {
    _owned_ = false;
    _vg_ = 0;
    return;
  }
  
  cache_vg::~cache_vg ()
  {
    _clear_vg_ ();
    return;
  }
  
  void cache_vg::_shoot_vertex (mygsl::rng & random_, 
                                    geomtools::vector_3d & vertex_)
  {
    bool devel = g_debug;
    if (_vg_ == 0)
      {
        throw runtime_error ("genvtx::cache_vg::_shoot_vertex: Missing vertex generator !");
      }
    if (! _cache_.is_valid ())
      {
        throw logic_error ("genvtx::cache_vg::_shoot_vertex: Invalid cache !");       
      }
    geomtools::invalidate (vertex_);
    geomtools::vector_3d vertex;
    geomtools::invalidate (vertex);
    
    _vg_->shoot_vertex (random_, vertex);

    if (geomtools::is_valid (vertex))
      {
        _cache_.child_to_mother (vertex, vertex_);
      }
    return;
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  cache_vg::create (const properties & configuration_, void * user_)
  {
    cerr << "DEVEL: genvtx::cache_vg::create: Entering..." << endl;
    configuration_.tree_dump (cerr, "genvtx::cache_vg::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    string path;
    string basename;
    string extension;
    size_t capacity;
    i_vertex_generator * vg = 0;

    // parameters of the cut:
    string generator_id;

    if (configuration_.has_key ("generator.id"))
      {
        generator_id = configuration_.fetch_string ("generator.id");
        if (! get_vertex_generator_db ().has_vertex_generator (generator_id))
          {
            ostringstream message;
            message << "genvtx::cache_vg::create: Invalid vertex generator id '"
                    << generator_id << "' !";
            throw runtime_error (message.str ());
          }
        vg_creator_type vgc = get_vertex_generator_db ().get_vertex_generator (generator_id);
        // vg = 
      }

    if (configuration_.has_key ("cache.path"))
      {
        path = configuration_.fetch_string ("cache.path");
      }

    if (configuration_.has_key ("cache.basename"))
      {
        basename = configuration_.fetch_string ("cache.basename");
      }

    if (configuration_.has_key ("cache.extension"))
      {
        extension = configuration_.fetch_string ("cache.extension");
      }

    if (configuration_.has_key ("cache.capacity"))
      {
        capacity = configuration_.fetch_integer ("cache.capacity");
      }

    // create a new parameterized 'cache_vg' instance:
    cache_vg * ptr = new cache_vg;
    ptr->set_path (path);
    ptr->set_basename (basename);
    ptr->set_extension (extension);
    ptr->set_capacity (capacity);
    ptr->set_vg (vg);
    return ptr; 
  }

  string cache_vg::vg_id () const
  {
    return "genvtx::cache_vg";
  }

  vg_creator_type cache_vg::vg_creator () const
  {
    return cache_vg::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<cache_vg> cache_vg::g_cr_;
  
} // end of namespace genvtx

// end of cache_vg.cc
