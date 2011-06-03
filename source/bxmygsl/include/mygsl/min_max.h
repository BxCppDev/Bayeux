// -*- mode: c++; -*- 
/* mygsl::min_max.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-02
 * Last modified: 2009-10-02
 * 
 * License: 
 * 
 * Description: 
 *
 *   A collection of algorithm to compute the min_max of
 *   a list of numbers
 *
 */

#ifndef __mygsl__min_max_h
#define __mygsl__min_max_h 1

using namespace std;

namespace mygsl {

  class min_max 
    {
    private:
      double __min;
      double __max;
    public:
      bool is_valid () const;
      double get_min () const;
      double get_max () const;
      double get_median () const;
      min_max ();
      void reset ();
      void add (double value_);
    };

} // end of namespace mygsl

#endif // __mygsl__min_max_h

// end of mygsl::min_max.h
