// -*- mode: c++ ; -*- 
/* drawer.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2010-02-20
 * 
 * License: 
 * 
 * Description: 
 *   
 * 
 * History: 
 * 
 */

#ifndef __geomtools__drawer_h
#define __geomtools__drawer_h 1

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <unistd.h>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/model_factory.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/visibility.h>

namespace geomtools {
  
  using namespace std;  
  
  class drawer 
  {

  public:

    static const string VIEW_KEY;
    static const string VIEW_2D_XY;
    static const string VIEW_2D_XZ;
    static const string VIEW_2D_YZ;
    static const string VIEW_3D;
  
    static void wait_for_key ();

  private:
    static void __draw (ostream & out_,
			const logical_volume & log_,
			const placement & p_,
			int max_display_level_ = 0);
  
  public:

    static void draw (const logical_volume & log_,
		      const placement & p_,
		      int max_display_level_,
		      const string & name_,
		      const datatools::utils::properties & config_);
    
    static void draw (const model_factory & mf_,
		      const string & name_,
		      const placement & p_,
		      int max_display_level_,
		      const datatools::utils::properties & config_);
  
  public:



  }; // class drawer
  
} // end of namespace snemo

#endif // __geomtools__i_model_h

// end of i_model.h
