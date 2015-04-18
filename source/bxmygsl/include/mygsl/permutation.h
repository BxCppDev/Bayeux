/// \file mygsl/permutation.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-02
 * Last modified: 2013-04-23
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

#ifndef MYGSL_PERMUTATION_H
#define MYGSL_PERMUTATION_H 1

// Standard library:
#include <iostream>
#include <string>

namespace mygsl {
  class permutation {
  public:
    permutation();
    permutation(size_t sz_);
    permutation(const permutation& p_);

    virtual ~permutation();

    permutation& operator=(const permutation& p_);

    permutation& operator++();

    permutation& operator--();

    const size_t* data() const;

    size_t get(size_t i_) const;

    size_t inversions() const;

    size_t linear_cycles() const;

    size_t canonical_cycles() const;

    void swap(size_t i_, size_t j_);

    bool is_initialized() const;

    bool is_valid() const;

    size_t size() const;

    void reset();

    void init(size_t sz_);

    void reverse ();

    bool inverse(permutation& p_);

    bool canonical_to_linear(permutation& p_);

    bool linear_to_canonical(permutation& p_);

    bool next();

    bool previous();

    void print(std::ostream & out_, bool eol_ = false) const;

    friend std::ostream& operator<<(std::ostream& out_, const permutation& p_);
    friend std::istream& operator>>(std::istream& in_, permutation& p_);

  private:
    bool _check_() const;
    //void _check_throw_(size_t i_, const std::string& where_) const;

  private:
    struct permutation_impl;
    permutation_impl *pImpl;
  };

} // end of namespace mygsl

#endif // MYGSL_PERMUTATION_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
