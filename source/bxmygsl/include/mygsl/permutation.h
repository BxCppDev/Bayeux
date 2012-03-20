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
 *
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

//using namespace   std;

namespace mygsl {

  class permutation 
  {
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

    void print (std::ostream & out_, bool eol_ = false) const;

    friend std::ostream & operator<< (std::ostream & out_, const permutation & p_);

    friend std::istream & operator>> (std::istream & in_, permutation & p_);

  private:

    bool _check_ () const;

    void _check_throw_ (size_t i_, const std::string & where_) const;

  private:

    gsl_permutation * _perm_;


  };

} // end of namespace mygsl

#endif // __mygsl__permutation_h

// end of mygsl::permutation.h

