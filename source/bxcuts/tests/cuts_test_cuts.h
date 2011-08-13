// -*- mode: c++ ; -*- 
/* test_cuts.h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-20
 * Last modified: 2010-09-20
 * 
 * License: 
 */

/** 
 * Description: 
 * A simple test data structure to perform 
 *  simple cuts.
 * 
 */

#ifndef __cuts__test__test_cuts_h
#define __cuts__test__test_cuts_h 1

#include <iostream>

#include <cuts/i_cut.h>

#include <cuts_test_data.h>

namespace cuts {

  namespace test {

    using namespace std;

    class range_cut : public i_cut
    {
    public:
      enum mode_t
      {
	MODE_X = 0,
	MODE_Y = 1,
	MODE_Z = 2
      };

    private:

      int    __mode;
      double __min, __max;
      bool   __reversed;
 
    protected:

      virtual bool _accept (); 

    public:
 
      void set_mode (int mode_);
 
      void set_range (double min_, double max_);
 
      void set_reversed (bool reversed_);

      range_cut ();

      virtual ~range_cut ();

      virtual string cut_id () const;

      virtual cut_creator_t cut_creator () const;

      static i_cut * create (const properties & configuration_, 
			     cut_dict_t * cut_dict_,
			     void * user_ = 0);

    private:

      static creator_registration<range_cut> __CR;
 
    };

    /************************************/
    class sphere_cut : public i_cut
    {

    private:

      double __x0, __y0, __z0;
      double __radius;
      bool   __reversed;
 
    protected:

      virtual bool _accept (); 

    public:
 
      void set_radius (double radius_);
 
      void set_center (double x0_, double y0_, double z0_);
 
      void set_reversed (bool reversed_);

      sphere_cut ();

      virtual ~sphere_cut ();

      virtual string cut_id () const;

      virtual cut_creator_t cut_creator () const;

      static i_cut * create (const properties & configuration_, 
			     cut_dict_t * cut_dict_,
			     void * user_ = 0);

    private:

      static creator_registration<sphere_cut> __CR;
 
    };

    /************************************/
    class color_cut : public i_cut
    {

    private:

      int __color;
 
    protected:

      virtual bool _accept (); 

    public:
 
      void set_color (int color_);

      color_cut ();

      virtual ~color_cut ();

      virtual string cut_id () const;

      virtual cut_creator_t cut_creator () const;

      static i_cut * create (const properties & configuration_, 
			     cut_dict_t * cut_dict_,
			     void * user_ = 0);

    private:

      static creator_registration<color_cut> __CR;
 
    };

  } // end of namespace test

} // end of namespace cuts

#endif // __cuts__test__test_cuts_h

// end of test_cuts.h
