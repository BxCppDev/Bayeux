// -*- mode: c++; -*- 
/* i_composite_shape_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Interface:
 *
 * History: 
 * 
 */

#ifndef __geomtools__i_composite_shape_3d_h
#define __geomtools__i_composite_shape_3d_h 1

#include <string>

#include <datatools/utils/properties.h>

#include <geomtools/i_shape_3d.h>
#include <geomtools/placement.h>

namespace geomtools {

  class i_composite_shape_3d : public i_shape_3d
    {
      
    public:
      class shape_t
	{
	  bool         __delete;
	  i_shape_3d * __shape;
	  placement    __placement;
	  
	private:
	  // forbid implemtation of:
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

	  void dump (std::ostream & out_ = std::clog) const;
	};


      bool is_composite () const;

      i_composite_shape_3d (double skin_ = DEFAULT_SKIN);

      virtual ~i_composite_shape_3d ();

      void set_shape1 (i_shape_3d &, const placement &);

      void set_shape2 (i_shape_3d &, const placement &);

      void set_shape1 (i_shape_3d *, const placement &);

      void set_shape2 (i_shape_3d *, const placement &);

      const shape_t & get_shape1 () const;

      const shape_t & get_shape2 () const;

      const shape_t & get_shape (int i_) const;

      void dump (std::ostream & out_ = std::clog) const;

    private:
      shape_t __shape1;
      shape_t __shape2;
      
  };
    
} // end of namespace geomtools

#endif // __geomtools__i_composite_shape_3d_h

// end of i_composite_shape_3d.h
