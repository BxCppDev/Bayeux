// -*- mode: c++; -*- 
/* i_composite_shape_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Basic interface for composite 3D shapes (union, intersection, difference)
 *
 * History: 
 * 
 */

#ifndef __geomtools__i_composite_shape_3d_h
#define __geomtools__i_composite_shape_3d_h 1

#include <string>

#include <geomtools/i_shape_3d.h>
#include <geomtools/placement.h>

namespace geomtools {

  using namespace std;

  class i_composite_shape_3d : public i_shape_3d
  {
      
  public:

    class shape_t : public datatools::utils::i_tree_dumpable
    {
      bool         _delete_;
      i_shape_3d * _shape_;
      placement    _placement_;
          
    private:

      // forbid implementation of:
      shape_t (const shape_t &);

      shape_t & operator=(const shape_t &);

    public:

      void copy (shape_t &);

      void reset ();

      bool is_valid () const;

      shape_t ();

      virtual ~shape_t ();

      bool is_delete () const;

      const i_shape_3d & get_shape () const; 

      i_shape_3d & get_shape (); 

      const placement & get_placement () const; 

      placement & get_placement (); 

      // factory methods:
      static void make_shape (i_shape_3d &, 
                              const placement &, 
                              shape_t &);

      static void make_shape (i_shape_3d *, 
                              const placement &, 
                              shape_t &);


      virtual void tree_dump (ostream & out_         = clog, 
                              const string & title_  = "", 
                              const string & indent_ = "", 
                              bool inherit_          = false) const;

      void dump (std::ostream & out_ = clog) const;

    };


    bool is_composite () const;

    i_composite_shape_3d (double skin_ = DEFAULT_SKIN);

    virtual ~i_composite_shape_3d ();

    void set_shape1 (i_shape_3d &, const placement &);

    void set_shape2 (i_shape_3d &, const placement &);

    void set_shapes (i_shape_3d &, i_shape_3d &, const placement &);

    void set_shape1 (i_shape_3d *, const placement &);

    void set_shape2 (i_shape_3d *, const placement &);

    void set_shapes (i_shape_3d *, i_shape_3d *, const placement &);

    const shape_t & get_shape1 () const;

    const shape_t & get_shape2 () const;

    const shape_t & get_shape (int i_) const;

    void dump (ostream & out_ = clog) const;

    virtual void tree_dump (ostream & out_         = clog, 
                            const string & title_  = "", 
                            const string & indent_ = "", 
                            bool inherit_          = false) const;
       
  private:

    shape_t _shape1_;
    shape_t _shape2_;
      
  };
    
} // end of namespace geomtools

#endif // __geomtools__i_composite_shape_3d_h

// end of i_composite_shape_3d.h
