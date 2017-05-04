// stl_tools.cc

// Ourselves:
#include <geomtools/stl_tools.h>

// Standard library:
#include <stdexcept>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project:
#include <geomtools/tessellation.h>

namespace geomtools {

  namespace stl {

    // Vertex :

    vertex::vertex ()
    {
      x = std::numeric_limits<double>::quiet_NaN ();
      y = std::numeric_limits<double>::quiet_NaN ();
      z = std::numeric_limits<double>::quiet_NaN ();
      return;
    }

    std::ostream & operator<<( std::ostream & out_, const vertex & vtx_)
    {
      out_ << "geomtools::stl::vertex{"
           << vtx_.x << ';'  << vtx_.y << ';'  << vtx_.z << '}' ;
      return out_;
    }

    bool operator== (const vertex & vtx1_, const vertex & vtx2_)
    {
      return vtx1_.x == vtx2_.x && vtx1_.y == vtx2_.y && vtx1_.z == vtx2_.z;
    }

    bool operator< (const vertex & vtx1_, const vertex & vtx2_)
    {
      if (vtx1_.z < vtx2_.z) return true;
      else if (vtx1_.z > vtx2_.z) return false;
      else if (vtx1_.y < vtx2_.y) return true;
      else if (vtx1_.y > vtx2_.y) return false;
      else if (vtx1_.x < vtx2_.x) return true;
      return false;
    }

    // Facet :

    facet::facet ()
    {
      nx = std::numeric_limits<double>::quiet_NaN ();
      ny = std::numeric_limits<double>::quiet_NaN ();
      nz = std::numeric_limits<double>::quiet_NaN ();
      vertices.reserve(3);
      return;
    }

    std::ostream & operator<<( std::ostream & out_, const facet & fct_)
    {
      out_ << "geomtools::stl::facet{normal={"
           << fct_.nx << ';'  << fct_.ny << ';' << fct_.nz
           << "};nvtx=" << fct_.vertices.size () << ";"
           << "v0=" << fct_.vertices[0] << ";"
           << "v1=" << fct_.vertices[1] << ";"
           << "v2=" << fct_.vertices[2] << '}' ;
      return out_;
    }

    // Solid :

    solid::solid ()
    {
      facets.reserve(100);
      return;
    }

    std::ostream & operator<<(std::ostream & out_, const solid & sld_)
    {
      out_ << "geomtools::stl::solid{name='" << sld_.name << "'"
           << ";number_of_facets="  << sld_.facets.size()
           << ";dummy_name='" << sld_.name << "'}";
      return out_;
    }

    // Converter :

    bool stl_to_geomtools_converter::is_debug () const
    {
      return _debug_;
    }

    void stl_to_geomtools_converter::set_debug (bool d_)
    {
      _debug_ = d_;
      return;
    }

    bool stl_to_geomtools_converter::is_check_normal () const
    {
      return _check_normal_;
    }

    void stl_to_geomtools_converter::set_check_normal (bool d_)
    {
      _check_normal_ = d_;
      return;
    }

    bool stl_to_geomtools_converter::is_fix_attempt () const
    {
      return _fix_attempt_;
    }

    void stl_to_geomtools_converter::set_fix_attempt (bool d_)
    {
      _fix_attempt_ = d_;
      return;
    }

    bool stl_to_geomtools_converter::is_lock_ts () const
    {
      return _lock_ts_;
    }


    void stl_to_geomtools_converter::set_lock_ts (bool d_)
    {
      _lock_ts_ = d_;
      return;
    }

    double stl_to_geomtools_converter::get_length_unit () const
    {
      return _length_unit_;
    }

    void stl_to_geomtools_converter::set_length_unit (double u_)
    {
      DT_THROW_IF (u_ <= .0, std::logic_error, "Invalid unit value (" << u_ << ") !");
      _length_unit_ = u_;
      return;
    }

    stl_to_geomtools_converter::stl_to_geomtools_converter ()
    {
      _debug_ = false;
      _check_normal_ = false;
      _fix_attempt_ = false;
      _lock_ts_ = false;
      _length_unit_ = CLHEP::mm;
      return;
    }

    void stl_to_geomtools_converter::fix_broken_facets (tessellated_solid & ts_, bool verbose_)
    {
      if (verbose_) {
        DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Entering...");
      }
      bool was_locked = ts_.is_locked ();
      int pass_counter = 0;
      if (was_locked) ts_.unlock ();
      do {
        // Search for mis-referenced vertices :
        std::vector<int> suspicious_vertex_indexes;
        for (geomtools::tessellated_solid::vertices_col_type::const_iterator
               i = ts_.vertices ().begin ();
             i != ts_.vertices ().end ();
             i++) {
          const geomtools::facet_vertex & fvtx = i->second;
          if (fvtx.ref_facets.size () < 3) {
            suspicious_vertex_indexes.push_back (i->first);
          }
        }

        // Check the number of suspicious vertices :
        size_t nsvtx = suspicious_vertex_indexes.size ();
        if (nsvtx == 0) {
          break;
        }
        pass_counter++;

        // If there are ones, we delete all facets that use them :
        for (size_t ivertex = 0; ivertex < nsvtx; ivertex++) {
          DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                         "Suspicious vertex #"
                         << suspicious_vertex_indexes[ivertex]);

          int vtxkey = suspicious_vertex_indexes[ivertex];

          // Check if the vertex exists in the dictionary of vertices :
          tessellated_solid::vertices_col_type::iterator vtx_found = ts_.grab_vertices ().find (vtxkey);
          DT_THROW_IF (vtx_found == ts_.grab_vertices ().end (),
                       std::logic_error,
                       "Vertex #" << vtxkey << "  does not exist !");

          // Get a reference to the vertex :
          facet_vertex & the_vertex = vtx_found->second;
          std::vector<int> suspicious_facet_indexes;
          for (std::map<int, int>::iterator ref_facet_iter = the_vertex.ref_facets.begin ();
               ref_facet_iter != the_vertex.ref_facets.end ();
               ref_facet_iter++) {
            suspicious_facet_indexes.push_back (ref_facet_iter->first);
          }

          for (size_t ifacet = 0; ifacet < suspicious_facet_indexes.size (); ifacet++) {
            DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                           "Remove suspicious facet #" << suspicious_facet_indexes[ifacet]);
            ts_.remove_facet (suspicious_facet_indexes[ifacet]);
          }
          ts_.grab_vertices ().erase (vtx_found);
          DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                         "Remove suspicious vertex #" << suspicious_vertex_indexes[ivertex]);
        }

        suspicious_vertex_indexes.clear ();
        ts_.compute_bounding_box ();
      } while (true);
      if (verbose_) {
        DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                      "Number of pass : " << pass_counter);
        ts_.dump (std::clog);
      }
      if (was_locked) ts_.lock ();
      return;
    }

    int stl_to_geomtools_converter::convert (const solid & solid_,
                                             tessellated_solid & ts_)
    {
      typedef std::map<vertex,int> vtx_dict_t;
      vtx_dict_t vertices;

      // if (is_debug()) std::clog << "DEBUG: geomtools::stl::stl_to_geomtools_converter::convert: "
      //                                << "Number of facets = " << solid_.facets.size()
      //                                << std::endl;
      for (size_t ifacet = 0; ifacet < solid_.facets.size(); ifacet++) {
          // if (is_debug()) std::clog << "DEBUG: geomtools::stl::stl_to_geomtools_converter::convert: "
          //                        << "Processing facets #" << ifacet << " ..."
          //                        << std::endl;
          const facet & a_facet = solid_.facets[ifacet];
          int ivertices[3];
          // if (is_debug()) std::clog << "DEBUG: geomtools::stl::stl_to_geomtools_converter::convert: "
          //                        << "  Facets #" << ifacet << " has "
          //                        << a_facet.vertices.size () << " vertices."
          //                        << std::endl;
          geomtools::vector_3d facet_normal (a_facet.nx, a_facet.ny, a_facet.nz);
          double facet_normal_mag = facet_normal.mag ();
          facet_normal /= facet_normal_mag;
          geomtools::vector_3d facet_vtx[3];
          DT_THROW_IF (a_facet.vertices.size () != 3,
                       std::logic_error,
                       "Invalid number of vertices (" << a_facet.vertices.size () << ") for facet #" << ifacet << " !");
          for (size_t ivtx = 0; ivtx < a_facet.vertices.size (); ivtx++) {
            const vertex & a_vertex = a_facet.vertices[ivtx];
            double x, y, z;
            x = a_vertex.x;
            y = a_vertex.y;
            z = a_vertex.z;
            facet_vtx[ivtx].set (x, y, z);
            vtx_dict_t::const_iterator found = vertices.find (a_vertex);
            if (found == vertices.end ()) {
              int new_vertex_id = vertices.size ();
              vertices[a_vertex] = new_vertex_id;
              ivertices[ivtx] = new_vertex_id;
              ts_.add_vertex (ivertices[ivtx],
                              a_vertex.x * _length_unit_,
                              a_vertex.y * _length_unit_,
                              a_vertex.z * _length_unit_);
            } else {
              ivertices[ivtx] = found->second;
            }
            // if (is_debug()) std::clog << "DEBUG: geomtools::stl::stl_to_geomtools_converter::convert: "
            //                        << "     vertex #" << ivertices[ivtx]
            //                        << std::endl;
          }

          bool cn = is_check_normal ();
          cn  = true;
          if (cn) {
            geomtools::vector_3d u0 = facet_vtx[1] - facet_vtx[0];
            geomtools::vector_3d u1 = facet_vtx[2] - facet_vtx[1];
            geomtools::vector_3d u2 = (u0.cross (u1)).unit ();
            double u2_mag = u2.mag ();
            u2 /= u2_mag;
            // if (is_debug()) std::clog << "DEVEL: geomtools::stl::stl_to_geomtools_converter::convert: "<< std::endl
            //                        << "     un=" << facet_normal
            //                        << "     u2=" << u2
            //                        << std::endl;
            if (! u2.isNear (facet_normal,1.e-4)) {
              DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                             "STL normal '" << facet_normal << "' does not match facet #"
                             << ifacet << " computed normal '" << u2 << "' !");
            }
          }

          ts_.add_facet(ifacet, ivertices[0], ivertices[1], ivertices[2]);
        }
      // if (is_debug()) std::clog << "DEBUG: geomtools::stl::stl_to_geomtools_converter::convert: "
      //                           << "Number of vertices = " << vertices.size()
      //                           << std::endl;

      if (_fix_attempt_) {
        fix_broken_facets(ts_, is_debug());
      }

      if (_lock_ts_) {
        ts_.lock();
      }

      return EXIT_SUCCESS;
    }

  } // end of namespace stl

} // end of namespace geomtools
