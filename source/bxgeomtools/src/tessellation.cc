// -*- mode: c++; -*- 
/* tessellation.cc
 */

#include <geomtools/tessellation.h>

#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <stdexcept>

#include <geomtools/utils.h>

namespace geomtools {

  /*** facet_vertex ***/

  facet_vertex::facet_vertex (double x_, 
                              double y_,
                              double z_)
  {
    position.set (x_, y_, z_);
    return;
  }

  void facet_vertex::add_ref_facet (int facet_index_, int facet_node_)
  {
    ref_facets[facet_index_] = facet_node_;
    return;
  }
  
  void facet_vertex::print_xyz (ostream & out_) const
  {
    out_ << position.x () << ' ' << position.y () << ' ' << position.z () << endl;
    return;
  }
  
  void facet_vertex::print (ostream & out_) const
  {
    out_ << '(' << position.x () << ' ' << position.y () << ' ' << position.z () << ')' 
         << " #ref=" << ref_facets.size  () << endl;
    return;
  }

  /*** i_facet ***/

  void i_facet::print (ostream & out_) const
  {
    out_ << "#vertices=" << get_number_of_vertices () << endl;
    return;
  }

  /*** facet3 ***/

  size_t facet3::get_number_of_vertices () const
  {
    return 3;
  }
  
  const facet_vertex & facet3::get_vertex (int i_) const 
  {
    if ((i_ < 0) || (i_ >= get_number_of_vertices ()))
      {
        ostringstream message;
        message << "facet3::get_vertex: "
                << "Invalid vertex index " << i_ << " !";
        throw runtime_error (message.str());
      }
    return *_vertices_[i_];
  }
  
  facet3::facet3 ()
  {
    _vertices_[0] = 0;
    _vertices_[1] = 0;
    _vertices_[2] = 0;
  }
  
  facet3::facet3 (const facet_vertex & v0_, 
                  const facet_vertex & v1_, 
                  const facet_vertex & v2_)
  {
    _vertices_[0] = &v0_;
    _vertices_[1] = &v1_;
    _vertices_[2] = &v2_;
    return;
  }

  /*** facet4 ***/
  
  bool facet4::__is_valid () const
  {
    // missing checks...
    return true;
  }
  
  size_t facet4::get_number_of_vertices () const
  {
    return 4;
  }

  const facet_vertex & facet4::get_vertex (int i_) const 
  {
    if ((i_ < 0) || (i_ >= get_number_of_vertices ()))
      {
        ostringstream message;
        message << "facet4::get_vertex: "
                << "Invalid vertex index " << i_ << " !";
        throw runtime_error (message.str());
      }
    return *_vertices_[i_];
  }
  
  facet4::facet4 ()
  {
    _vertices_[0] = 0;
    _vertices_[1] = 0;
    _vertices_[2] = 0;
    _vertices_[3] = 0;
    return;
  }
 
  facet4::facet4 (const facet_vertex & v0_, 
                  const facet_vertex & v1_, 
                  const facet_vertex & v2_, 
                  const facet_vertex & v3_)
  {
    _vertices_[0] = &v0_;
    _vertices_[1] = &v1_;
    _vertices_[2] = &v2_;
    _vertices_[3] = &v3_;
    if (! __is_valid ())
      {
        throw runtime_error ("facet4::facet4: Invalid set of vertices !");
      }
    return;
  }

  /*** tessellated_solid ***/
  
  bool tessellated_solid::is_locked () const
  {
    return _locked_;
  }
  
  bool tessellated_solid::is_consistent () const
  {
    return _consistent_;
  }
  
  bool tessellated_solid::__check ()
  {
    _consistent_ = true;
    for (vertices_col_t::const_iterator i = _vertices_.begin ();
         i != _vertices_.end ();
         i++)
      {
        if (i->second.ref_facets.size () == 0)
          {
            clog << "WARNING: tessellated_solid::__check: Vertex "
                 << i->first << " is not used at all !" << endl;
            _consistent_ = false;
          }
        else if (i->second.ref_facets.size () < 3) 
          {
            clog << "WARNING: tessellated_solid::__check: Vertex "
                 << i->first << " is not properly referenced !" << endl;
            _consistent_ = false;
            return _consistent_;
          }
      }
    // missing check:
    
    return _consistent_;
  }

  void tessellated_solid::lock ()
  {
    if (! __check ())
      {
        throw runtime_error ("tessellated_solid::lock: Solid is not validated !");
      }
    _locked_ = true;
    return;
  }
  
  void tessellated_solid::unlock ()
  {
    _locked_ = false;
    return;
  }
  
  bool tessellated_solid::validate_index (int i_)
  {
    return (i_ > INVALID_VERTEX) && (i_ < MAX_VERTEX);
  }
  
  tessellated_solid::tessellated_solid ()
  {
    _locked_ = false;
    _consistent_ = false;
    return;
  }
  
  tessellated_solid::~tessellated_solid ()
  {
    for (facets_col_t::iterator i = _facets_.begin ();
         i != _facets_.end ();
         i++)
      {
        delete i->second;
      }
    _facets_.clear ();
    _vertices_.clear ();
    return;
  }
  
  const tessellated_solid::vertices_col_t & tessellated_solid::vertices () const
  {
    return _vertices_;
  }
  
  const tessellated_solid::facets_col_t & tessellated_solid::facets () const
  {
    return _facets_;
  }
  
  int tessellated_solid::add_vertex (unsigned int vtx_key_, const facet_vertex & vtx_)
  {
    if (_locked_)
      {
        throw runtime_error ("tessellated_solid::add_vertex: Object is locked !");
      }
    _vertices_[vtx_key_] = vtx_;
    return vtx_key_;
  }

  int tessellated_solid::add_vertex (unsigned int vtx_key_, 
                                     double x_, 
                                     double y_, 
                                     double z_)
  {
    facet_vertex vtx;
    vtx.position.set (x_, y_, z_);
    return add_vertex (vtx_key_, vtx);
  }
  
  void tessellated_solid::add_facet3 (unsigned int facet_key_, 
                                      int ivtx0_, int ivtx1_, int ivtx2_)
  {
    add_facet (facet_key_,  ivtx0_, ivtx1_, ivtx2_, INVALID_VERTEX);
    return;
  }
  
  void tessellated_solid::add_facet4 (unsigned int facet_key_, 
                                      int ivtx0_, int ivtx1_, int ivtx2_, int ivtx3_)
  {
    add_facet (facet_key_,  ivtx0_, ivtx1_, ivtx2_, ivtx3_);
    return;
  }
  
  void tessellated_solid::add_facet (unsigned int facet_key_, 
                                     int ivtx0_, 
                                     int ivtx1_, 
                                     int ivtx2_, 
                                     int ivtx3_)
  {
    if (_locked_)
      {
        throw runtime_error ("tessellated_solid::add_vertex: Object is locked !");
      }
    i_facet * f= 0;
    size_t dim = 3;
    if (validate_index (ivtx3_))
      {
        dim = 4;
      }
    int ivtx[4];
    ivtx[0] = ivtx0_;
    ivtx[1] = ivtx1_;
    ivtx[2] = ivtx2_;
    ivtx[3] = ivtx3_;
    vertices_col_t::iterator vtx_it[4];
    for (int i = 0; i < dim; i++)
      {
        vtx_it[i] = _vertices_.find (ivtx[i]);
        if (vtx_it[i] == _vertices_.end ())
          {
            ostringstream message;
            message << "tessellated_solid::add_facet: "
                    << "Invalid vertex index " << ivtx[i] << " !";
            throw runtime_error (message.str());
          }
      } 
    i_facet * fct = 0;
    if (dim == 3)
      {
        vtx_it[0]->second.add_ref_facet (facet_key_, 0);
        vtx_it[1]->second.add_ref_facet (facet_key_, 1);
        vtx_it[2]->second.add_ref_facet (facet_key_, 2);
        fct = new facet3 ((vtx_it[0]->second),
                          (vtx_it[1]->second),
                          (vtx_it[2]->second));
      }    
    else
      {
        vtx_it[0]->second.add_ref_facet (facet_key_, 0);
        vtx_it[1]->second.add_ref_facet (facet_key_, 1);
        vtx_it[2]->second.add_ref_facet (facet_key_, 2);
        vtx_it[3]->second.add_ref_facet (facet_key_, 3);
        fct = new facet4 ((vtx_it[0]->second),
                          (vtx_it[1]->second),
                          (vtx_it[2]->second),
                          (vtx_it[3]->second));
      } 
    _facets_[facet_key_] = fct;
    return;
  }
  
  void tessellated_solid::dump (ostream & out_) const
  {
    string tag   = "|-- ";
    string stag  = "|   ";
    string ltag  = "`-- ";
    string sltag = "    ";
    out_ << "tessellated_solid" << endl;
    out_ << tag << "Locked: " << is_locked () << endl;
    out_ << tag << "Vertices: " << _vertices_.size () << endl;
    for (vertices_col_t::const_iterator i = _vertices_.begin ();
         i != _vertices_.end ();
         i++)
      {
        string tag2 = tag;
        {
          vertices_col_t::const_iterator j = i;
          if (++j == _vertices_.end ()) tag2 = ltag;
        }
        out_ << stag << tag2 << "Vertex[" << i->first << "] : ";
        i->second.print (out_);
      }
    out_ << ltag << "Facets: " << _facets_.size () << endl;
    for (facets_col_t::const_iterator i = _facets_.begin ();
         i != _facets_.end ();
         i++)
      {
        string tag2 = tag;
        {
          facets_col_t::const_iterator j = i;
          if (++j == _facets_.end ()) tag2 = ltag;
        }
        out_ << sltag << tag2 << "Facet[" << i->first << "] : ";
        i->second->print (out_);
      }
    return;
  }

  void tessellated_solid::print_xyz (ostream & out_) const
  {
    bool devel = false;
    if (devel)
      {
        clog << "DEVEL: tessellated_solid::print_xyz: # vertices = " 
             << _vertices_.size()
             << endl;
        clog << "DEVEL: tessellated_solid::print_xyz: # facets = " 
             << _facets_.size()
             << endl;
      }
    size_t last = 0;

    // facets:
    int count = 0;
    for (facets_col_t::const_iterator i = _facets_.begin ();
         i != _facets_.end ();
         i++)
      {
        const i_facet * fct = i->second;
        out_ << "# facet " << count 
             << " with " <<  fct->get_number_of_vertices ()
             << " vertices" << endl;
        size_t nvtx = fct->get_number_of_vertices ();
        if (devel)
          {
            clog << "DEVEL: tessellated_solid::print_xyz: # nvtx = " 
                 << nvtx
                 << endl;
          }
        // vertices:
        double dx = fct->get_vertex (1).position.x () - fct->get_vertex (0).position.x ();
        double dy = fct->get_vertex (1).position.y () - fct->get_vertex (0).position.y ();
        double dz = fct->get_vertex (1).position.z () - fct->get_vertex (0).position.z ();
        double m2 = dx * dx + dy * dy + dz * dz;
        double m  = sqrt (m2);
        fct->get_vertex (0).print_xyz (out_);
        fct->get_vertex (1).print_xyz (out_);
        out_ << endl;
        if (nvtx == 3)
          {       
            facet_vertex last;
            last.position.setX (fct->get_vertex (2).position.x () - 1e-6 * dx / m);
            last.position.setY (fct->get_vertex (2).position.y () - 1e-6 * dy / m);
            last.position.setY (fct->get_vertex (2).position.z () - 1e-6 * dz / m);
            last.print_xyz (out_);
          }
        else
          {
            fct->get_vertex (fct->get_number_of_vertices () - 1)
              .print_xyz (out_);
          }
        fct->get_vertex (2).print_xyz (out_);
        out_ << endl;
        out_ << endl;
        if (devel)
          {
            clog << "DEVEL: tessellated_solid::print_xyz: # last = " 
                 << last
                 << endl;
          }
        count++;
      }
    return;
  }
  
  bool tessellated_solid::is_inside (const vector_3d &, 
                                     double skin_) const
  {
    ostringstream message;
    message << "tessellated_solid::is_inside: Not implemented !";
    throw runtime_error (message.str ());
    return false;
  }
  
  // if 'skin' < 0 no skin is taken into account:
  bool tessellated_solid::is_on_surface (const vector_3d & , 
                                         int mask_, 
                                         double skin_) const
  {
    ostringstream message;
    message << "tessellated_solid::surface: Not implemented !";
    throw runtime_error (message.str ());
    
    return false;
  }
  
  vector_3d tessellated_solid::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d v;
    ostringstream message;
    message << "tessellated_solid::get_normal_on_surface: Not implemented !";
    throw runtime_error (message.str ());
    return v;
  }
  
  bool tessellated_solid::find_intercept (const vector_3d & from_, 
                                          const vector_3d & direction_,
                                          intercept_t & intercept_,
                                          double skin_) const
  {
    ostringstream message;
    message << "tessellated_solid::find_intercept: Not implemented !";
    throw runtime_error (message.str ());
     
    return false;
  }

  const std::string tessellated_solid::TESSELLATED_LABEL = "tessellated";

  std::string tessellated_solid::get_shape_name () const
  {
    return tessellated_solid::TESSELLATED_LABEL;
  }

  /*
    void tessellated_solid::initialize (const string & filename_)
    {
    ifstream ifs;
    string filename = filename_;
    ifs.open (filename.c_str ());
    if (! ifs)
    {
    ostringstream message;
    message << "tessellated_solid::initialize: " 
    << "Cannot open data file '"
    << filename << "' !";
    throw runtime_error (message.str ()); 
    }
    size_t count = 0;
    double length_unit = CLHEP::mm;
    double z_factor = 1.0;
    double r_factor = 1.0;
    size_t point_counter = 0;
    while (! ifs.eof ())
    {
    string line;
    getline (ifs, line);
    cerr << "DEVEL: tessellated_solid::initialize: " 
    << "line = '" << line << "'" << endl;
    count++;
    {
    istringstream iss (line);
    string  word;
    iss >> word;
    if (word.empty ()) continue;
    if (word[0] == '#') 
    {
    if (word.size () >= 2)
    {
    if (word == "#@length_unit")
    {
    string unit_str;
    iss >> unit_str;
    if (! iss)
    {
    ostringstream message;
    message << "tessellated_solid::initialize: " 
    << "Invalid format for the length unit directive in data file '"
    << filename << "' at line " << count << " !";
    throw runtime_error (message.str ()); 
    }
    length_unit = datatools::utils::units::get_length_unit_from (unit_str);
    }
    }
    continue;
    }
    }
    {
    istringstream iss (line);
    string token;
    iss >> token;
    if (token == "point")
    {
    }
    }
    }
    return;
    }
  */

} // end of namespace geomtools

// end of tessellation.cc
