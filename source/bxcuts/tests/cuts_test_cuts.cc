// -*- mode: c++ ; -*- 
/* test_cuts.cc
 */

#include <cuts_test_cuts.h>

#include <stdexcept>
#include <sstream>

namespace cuts {

  namespace test {
 
    void range_cut::set_mode (int mode_)
    {
      __mode = mode_;
    }
  
    void range_cut::set_range (double min_, double max_)
    {
      __min = min_;
      __max = max_;
    }
  
    void range_cut::set_reversed (bool reversed_)
    {
      __reversed = reversed_;
    }
  
    // ctor:
    range_cut::range_cut () : i_cut ()
    {
      __mode = MODE_X;
      __min = 0.0;
      __max = 1.0;
      __reversed = false;
      return;
    }
  
    // dtor:
    range_cut::~range_cut ()
    {
    }

    bool range_cut::_accept ()
    {
      data * a_data = static_cast<data *> (_get_user_data ());
      double t;
      if (__mode == MODE_X)
	{
	  t = a_data->x;
	}
      else if (__mode == MODE_Y)
	{
	  t = a_data->y;
	}
      else if (__mode == MODE_Z)
	{
	  t = a_data->z;
	}
    
      bool result = i_cut::ACCEPT;
      if (t < __min) 
	{
	  result = i_cut::REJECT;
	}
      else if (t > __max) 
	{
	  result = i_cut::REJECT;
	}
      if (__reversed)
	{
	  result = (result == i_cut::REJECT)? i_cut::ACCEPT: i_cut::REJECT;
	}
      return result;
    } 

    // static method used within a cut factory:
    i_cut * range_cut::create (const properties & configuration_, 
			       cut_dict_t * cut_dict_,
			       void * user_)
    {
      using namespace std;

      int mode      = range_cut::MODE_X;
      double min    = 0.0;
      double max    = 1.0;
      bool reversed = false;

      if (configuration_.has_key ("mode"))
	{
	  string mode_str = configuration_.fetch_string ("mode");
	  if (mode_str == "x")
	    {
	      mode = range_cut::MODE_X;
	    }
	  else if (mode_str == "y")
	    {
	      mode = range_cut::MODE_Y;
	    }
	  else if (mode_str == "Z")
	    {
	      mode = range_cut::MODE_Z;
	    }
	  else
	    {
	      ostringstream message;
	      message << "range_cut::create: "
		      << "Invalid mode '" << mode_str << "' !";
	    }
	}

      if (configuration_.has_key ("min"))
	{
	  min = configuration_.fetch_real ("min");
	}

      if (configuration_.has_key ("max"))
	{
	  max = configuration_.fetch_real ("max");
	}

      if (configuration_.has_flag ("reversed"))
	{
	  reversed = true;
	}

      // create a new parameterized 'range_cut' instance:
      range_cut * cut_ptr = new range_cut;
      cut_ptr->set_mode (mode);
      cut_ptr->set_range (min, max);
      cut_ptr->set_reversed (reversed);
      return cut_ptr;	
    }
  
    // register this creator:   
    i_cut::creator_registration<range_cut> range_cut::__CR;
 
    string range_cut::cut_id () const
    {
      return "cuts::test::range_cut";
    }
  
    cut_creator_t range_cut::cut_creator () const
    {
      return range_cut::create;
    }
  
    /*******************************************************/

    void sphere_cut::set_radius (double radius_)
    {
      __radius = radius_;
    }
  
    void sphere_cut::set_center (double x0_, double y0_, double z0_)
    {
      __x0 = x0_;
      __y0 = y0_;
      __z0 = z0_;
    }
  
    void sphere_cut::set_reversed (bool reversed_)
    {
      __reversed = reversed_;
    }
  
    // ctor:
    sphere_cut::sphere_cut () : i_cut ()
    {
      __radius = 1.0;
      __x0 = 0.0;
      __y0 = 0.0;
      __z0 = 0.0;
      __reversed = false;
      return;
    }
  
    // dtor:
    sphere_cut::~sphere_cut ()
    {
    }

    bool sphere_cut::_accept ()
    {
      data * a_data = static_cast<data *> (_get_user_data ());
      double hx = a_data->x - __x0;
      double hy = a_data->y - __y0;
      double hz = a_data->z - __z0;
      double r2 = (hx * hx + hy * hy + hz * hz);
      double rlim2 = __radius * __radius;
      bool result = i_cut::ACCEPT;
      if (r2 > rlim2) 
	{
	  result = i_cut::REJECT;
	}
     if (__reversed)
	{
	  result = (result == i_cut::REJECT)? i_cut::ACCEPT: i_cut::REJECT;
	}
      return result;
    } 

    // static method used within a cut factory:
    i_cut * sphere_cut::create (const properties & configuration_, 
			       cut_dict_t * cut_dict_,
			       void * user_)
    {
      using namespace std;

      double radius = 1.0;
      double center[3];
      bool reversed = false;
      center[0] = 0.0;
      center[1] = 0.0;
      center[2] = 0.0;

      if (configuration_.has_key ("center"))
	{
	  vector<double> v;
	  configuration_.fetch ("center", v);
	  if (v.size () != 3)
	    {
	      ostringstream message;
	      message << "sphere_cut::create: "
		      << "Invalid vector size for center !" 
		      << endl;
	      throw runtime_error (message.str ());
	    }
	  center[0] = v[0];
	  center[1] = v[1];
	  center[2] = v[2];
	}

     if (configuration_.has_key ("radius"))
	{
	  radius = configuration_.fetch_real ("radius");
	  if (radius <= 0.0)
	    {
	      ostringstream message;
	      message << "sphere_cut::create: "
		      << "Invalid radius !" 
		      << endl;
	      throw runtime_error (message.str ());
	    }
	}

      if (configuration_.has_flag ("reversed"))
	{
	  reversed = true;
	}

      // create a new parameterized 'sphere_cut' instance:
      sphere_cut * cut_ptr = new sphere_cut;
      cut_ptr->set_radius (radius);
      cut_ptr->set_center (center[0], center[1], center[2]);
      cut_ptr->set_reversed (reversed);
      return cut_ptr;	
    }
  
    // register this creator:   
    i_cut::creator_registration<sphere_cut> sphere_cut::__CR;
 
    string sphere_cut::cut_id () const
    {
      return "cuts::test::sphere_cut";
    }
  
    cut_creator_t sphere_cut::cut_creator () const
    {
      return sphere_cut::create;
    }
  
    /*******************************************************/

    void color_cut::set_color (int color_)
    {
      __color = color_;
    }
  
    // ctor:
    color_cut::color_cut () : i_cut ()
    {
      __color = data::BLACK;
      return;
    }
  
    // dtor:
    color_cut::~color_cut ()
    {
    }

    bool color_cut::_accept ()
    {
      data * a_data = static_cast<data *> (_get_user_data ());
      int color = a_data->color;
      bool result = i_cut::ACCEPT;
      if (color != __color) 
	{
	  result = i_cut::REJECT;
	}
      return result;
    } 

    // static method used within a cut factory:
    i_cut * color_cut::create (const properties & configuration_, 
			       cut_dict_t * cut_dict_,
			       void * user_)
    {
      using namespace std;

      int color = data::BLACK;

      if (configuration_.has_key ("color"))
	{
	  string color_str = configuration_.fetch_string ("color");
	  if (color_str == "black")
	    {
	      color = data::BLACK;
	    }
	  else if (color_str == "red")
	    {
	      color = data::RED;
	    }
	  else if (color_str == "green")
	    {
	      color = data::GREEN;
	    }
	  else if (color_str == "blue")
	    {
	      color = data::BLUE;
	    }
	  else 
	    {
	      ostringstream message;
	      message << "color_cut::create: "
		      << "Invalid color label !" 
		      << endl;
	      throw runtime_error (message.str ());
	    }
	}

      // create a new parameterized 'color_cut' instance:
      color_cut * cut_ptr = new color_cut;
      cut_ptr->set_color (color);
      return cut_ptr;	
    }
  
    // register this creator:   
    i_cut::creator_registration<color_cut> color_cut::__CR;
 
    string color_cut::cut_id () const
    {
      return "cuts::test::color_cut";
    }
  
    cut_creator_t color_cut::cut_creator () const
    {
      return color_cut::create;
    }

  } // end of namespace test
  
} // end of namespace cuts

// end of test_cuts.cc
