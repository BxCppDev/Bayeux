// -*- mode: c++; -*- 
/* tessellation.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-06-04
 * Last modified: 2010-06-04
 * 
 * License: 
 * 
 * Description: 
 * 
 * History: 
 * 
 */

#ifndef __geomtools__tessellation_h
#define __geomtools__tessellation_h 1

#include <iostream>
#include <string>
#include <map>

#include <boost/cstdint.hpp>

#include <geomtools/i_shape_3d.h>

namespace geomtools {

  using namespace std;

  struct facet_vertex
  {
    vector_3d position;
    map<int, int> ref_facets;

  public:

    facet_vertex (double x_ = 0.0, double y_ = 0.0, double z_ = 0.0);

    void print_xyz (ostream & out_) const;

    void print (ostream & out_) const;

    void add_ref_facet (int facet_index_, int facet_node_);

  };


  class i_facet
  {
  public:

    virtual size_t get_number_of_vertices () const = 0;

    virtual const facet_vertex & get_vertex (int i_) const = 0;

    void print (ostream & out_) const;

  };


  class facet3 : public i_facet
  {
  private:
    const facet_vertex * _vertices_[3];

  public:
    virtual size_t get_number_of_vertices () const;
    virtual const facet_vertex & get_vertex (int i_) const;
    facet3 ();
    facet3 (const facet_vertex & v1_, 
            const facet_vertex & v2_, 
            const facet_vertex & v3_);

  };

  class facet4 : public i_facet
  {
  private:
    const facet_vertex * _vertices_[4];
  private:
    bool __is_valid () const;
  public:
    virtual size_t get_number_of_vertices () const;
    virtual const facet_vertex & get_vertex (int i_) const;
    facet4 ();
    facet4 (const facet_vertex & v1_, 
            const facet_vertex & v2_, 
            const facet_vertex & v3_, 
            const facet_vertex & v4_);

  };

  /*** tessellated_solid ***/

  class tessellated_solid : public i_shape_3d 
  {
  public:
    static const int32_t FACE_ALL= 0xFFFFFFFF;

    static const std::string TESSELLATED_LABEL;
    typedef map<unsigned int, facet_vertex> vertices_col_t;
    typedef map<unsigned int, i_facet *> facets_col_t;
    static const int INVALID_VERTEX = -1.0;
    static const int MAX_VERTEX     = 0x0FFFFFFF;
    
  private:
    bool           _locked_;
    bool           _consistent_;
    vertices_col_t _vertices_;
    facets_col_t   _facets_;
    
  public:
      
    virtual std::string get_shape_name () const;

    static bool validate_index (int);
    bool is_consistent () const;
    bool is_locked () const;
    tessellated_solid ();
    virtual ~tessellated_solid ();
    const vertices_col_t & vertices () const;
    const facets_col_t & facets () const;
    int add_vertex (unsigned int vtx_key_, const facet_vertex & vtx_);
    int add_vertex (unsigned int vtx_key_, 
                    double x_, double y_, double z_);
    void add_facet (unsigned int facet_key_, 
                    int ivtx0_, int ivtx1_, int ivtx2_, int ivtx3_ = INVALID_VERTEX);
    void add_facet3 (unsigned int facet_key_, 
                     int ivtx0_, int ivtx1_, int ivtx2_);
    void add_facet4 (unsigned int facet_key_, 
                     int ivtx0_, int ivtx1_, int ivtx2_, int ivtx3_);
    void lock ();
    void unlock ();
    void print_xyz (ostream & out_) const;
    void dump (ostream & out_ = clog) const;

    //void initialize (const string & filename_);
      
    virtual bool is_inside (const vector_3d &, 
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    // if 'skin' < 0 no skin is taken into account:
    virtual bool is_on_surface (const vector_3d & , 
                                int index_   = FACE_ALL , 
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;
    
    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;
    
    virtual bool find_intercept (const vector_3d & from_, 
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;
    
  private:
    bool __check ();

  };
  
} // end of namespace geomtools

#endif // __geomtools__tessellation_h

// end of dummy_tessellation.h
