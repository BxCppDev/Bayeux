/* prng_state_manager.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-03-02
 * Last modified : 2012-06-15
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   A manager that record the internal state of some PRNGs
 *
 * History:
 *
 */

#ifndef __mygsl__prng_state_manager_h
#define __mygsl__prng_state_manager_h 1

#include <iostream>
#include <string>
#include <map>

#include <mygsl/rng.h>

#include <boost/cstdint.hpp>

namespace mygsl {

  /** A manager that can store the internal states associated to
   * a set of PRNGs addressed with some labels.
   */
  class prng_state_manager
  {
  public:

    /** Internal record that store the internal state of a PRNG
     */
    class record
    {
    public:

      /// Default size of a buffer used to store PRNG's internal state
      static const size_t DEFAULT_BUFFER_SIZE; 

    public:

      /// Default constructor
      record ();

      /// Destructor
      virtual ~record ();

    public:

      /// Buffer to store a PRNG internal state
      mygsl::rng::state_buffer_type state_buffer;

    };

    /// An alias for the embedded dictionnary container class
    typedef std::map<std::string, record> dict_type;

  public:

    static const int    INVALID_COUNTER_VALUE; //!< Constant that represents an invalid value for a counter
    static const std::string DEFAULT_FILENAME; //!< Default backup filename

  public:

    /// Check if a backup filename is associated to the manager
    bool has_filename () const;

    /// Get the backup filename associated to the manager
    const std::string & get_filename () const;

    /// Set the backup filename associated to the manager
    void set_filename (const std::string & = DEFAULT_FILENAME);

    /// Check if the manager has a running counter value
    bool has_counter () const;

    /// Get the manager's running counter value 
    int get_counter () const;

    /// Set the manager's running counter value 
    void set_counter (int);

    /// Reset the manager's running counter value 
    void reset_counter ();

    /// Increment the manager's running counter value by a given value
    void increment_counter (unsigned int a_increment = 1);

    /// Default constructor
    prng_state_manager ();

    /// Constructor with a filename 
    prng_state_manager (const std::string &);

    /// Destructor:
    virtual ~prng_state_manager ();

    /// Invalidate :
    void invalidate ();

    /// Build an array with all PRNGs' labels stored in the manager
    void get_labels (std::vector<std::string> & labels_) const;

    /// Check if the manager stores the internal state of a PRNG with a given label
    bool has_state (const std::string & label_) const;

    /// Add storage for the internal state of given size for a PRNG with a given label
    void add_state (const std::string & label_, size_t sz_);

    /// Get a record that hosts the internal state  of a PRNG with a given label (non mutable)
    const record & get_state (const std::string & label_) const;

    /// Get a record that hosts the internal state  of a PRNG with a given label (mutable)
    record & get_state (const std::string & label_);

    //void update_state (const std::string & label_, int32_t seed_);

    /// Clear the content of the manager
    void clear ();

    /// Reset the manager
    void reset ();

    /// Return the number of internal state records
    size_t size () const;

    /// Check if the manager has no stored internal state records 
    bool empty () const;

    /// Store the content of the manager in a file with given filename
    void store (const std::string & filename_ = "") const;

    /// Load the content of the manager from a file with given filename
    void load (const std::string & filename_ = "");

    /// Basic print
    void dump (std::ostream & out_ = std::clog) const;

  private:

    std::string  _filename_; //!< The name of the file to store the PRNG internal states
    int32_t      _counter_;  //!< A user counter
    dict_type    _dict_;     //!< The dictionary of PRNG internal states

  };

}  // end of namespace mygsl

#endif // __mygsl__prng_state_manager_h

// end of prng_state_manager.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
