// -*- mode: c++; -*- 
/* regular_grid_placement.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-26
 * Last modified: 2010-02-26
 * 
 * License: 
 * 
 * Description: 
 *  XY grid 2D placement
 *
 *   y^
 *    |   column  0      1      2      3      4                                          
 *    |           :      :      :      :      :     row              
 *    |           x......x......x......x......x......2                           
 *    |           :10    :11    :12    :13    :14                              
 * yc +.......... x......x......c......x......x......1                        
 *    |      dy   :5     :6     :7     :8     :9                        
 * yo +...........o......x......x......x......x......0                        
 *    |           :0 dx   1     :2      3      4                      
 *    |           :             :                                  
 *    +-----------+-------------+--------------------> x           
 *   O           xo             xc
 *
 *
 *
 * History: 
 * 
 */

#ifndef __geomtools__regular_grid_placement_h
#define __geomtools__regular_grid_placement_h 1

#include <iostream>
#include <iomanip>
#include <string>

#include <geomtools/utils.h>
#include <geomtools/i_placement.h>
#include <geomtools/placement.h>

namespace geomtools {

  using namespace std;
 
  class regular_grid_placement
    : public i_placement 
    {
    public:
      enum mode_t
	{
	  MODE_XY = 0,
	  MODE_YZ = 1,
	  MODE_XZ = 2
	};

      static const bool CENTERED = true;
      static const bool UNCENTERED = false;
      
    private:
      placement __basic_placement;
      double    __column_step; // y
      double    __row_step;    // x
      size_t    __number_of_columns;
      size_t    __number_of_rows;
      int       __mode;
      bool      __centered;

    public:
 
      virtual bool has_only_one_rotation () const;

      bool is_replica () const;

      bool is_valid () const;

      void invalidate ();

      int get_mode () const;

      bool is_mode_xy () const;

      bool is_mode_yz () const;

      bool is_mode_xz () const;

      void set_mode (int m_);

      bool is_centered () const;

      void set_centered (bool c_);

      void set_column_step (double dx_);

      void set_row_step (double dx_);

      void set_steps (double dx_, double dy_);

      double get_column_step () const;

      double get_row_step () const;

      double get_x_step () const;

      double get_y_step () const;

      double get_z_step () const;

      void set_basic_placement (const placement & bp_);

      const placement & get_basic_placement () const;

      placement & get_basic_placement ();

      void set_number_of_rows (size_t nr_);

      void set_number_of_columns (size_t nc_);

      void set_dimensions (size_t nc_, size_t nr_);
 
      size_t get_number_of_rows () const;
 
      size_t get_number_of_columns () const;

      void get_placement (int col_, int row_, placement & p_) const;

      placement get_placement (int col_, int row_) const;

    public: 
      // i_placement interface:
      virtual size_t get_number_of_items () const;
   
      virtual void get_placement (int item_, placement & p_) const;

    public: 
      // ctor:
      regular_grid_placement ();
		
      // ctor:
      regular_grid_placement (const placement & basic_placement_, 
			      double column_step_,
			      double row_step_,
			      size_t number_of_columns_,
			      size_t number_of_rows_,
			      int mode_,
			      bool centered_ = true);
      
       // dtor:
      virtual ~regular_grid_placement ();

      void init (const placement & basic_placement_, 
		 double column_step_,
		 double row_step_,
		 size_t number_of_columns_,
		 size_t number_of_rows_,
		 int mode_,
		 bool centered_ = true);
      
      virtual void reset ();

      // i_tree_dump interface:
      virtual void tree_dump (ostream & out_  = clog, 
			      const string & title_ = "geomutils::regular_grid_placement", 
			      const string & indent_ = "", 
			      bool inherit_ = false) const;

    };

} // end of namespace geomtools

#endif // __geomtools__regular_grid_placement_h

// end of regular_grid_placement.h
