// -*- mode: c++; -*- 
/* mygsl::permutation.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-02
 * Last modified: 2009-10-02
 * 
 * License: 
 * 
 * 
 * Description: 
 *   Interface for GSL permutation
 *
 * History: 
 * 
 */

#ifndef __mygsl__permutation_h
#define __mygsl__permutation_h 1

#include <iostream>
#include <string>

#include <gsl/gsl_permutation.h>

using namespace std;

namespace mygsl {

  class permutation 
    {
    private:

      gsl_permutation * __perm;

    private:

      bool __check () const;

      void __check_throw (size_t i_, const string & where_) const;

    public:

      const size_t * data () const;

      size_t get (size_t i_) const;

      size_t inversions () const;

      size_t linear_cycles () const;

      size_t canonical_cycles () const;

      void swap (size_t i_, size_t j_);

      bool is_initialized () const;

      bool is_valid () const;

      size_t size () const;

      permutation ();

      permutation (size_t sz_);

      permutation (const permutation & p_);

      virtual ~permutation ();

      permutation & operator= (const permutation & p_);

      void reset ();

      void init (size_t sz_);

      void reverse ();

      bool inverse (permutation & p_);

      bool canonical_to_linear (permutation & p_);

      bool linear_to_canonical (permutation & p_);

      permutation & operator++ ();

      permutation & operator-- ();

      bool next ();

      bool previous ();

      void print (ostream & out_, bool eol_ = false) const;

      friend ostream & operator<< (ostream & out_, const permutation & p_);

      friend istream & operator>> (istream & in_, permutation & p_);

    };

} // end of namespace mygsl

#endif // __mygsl__permutation_h

// end of mygsl::permutation.h

