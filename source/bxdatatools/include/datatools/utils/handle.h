/* handle.h */
//! \file datatools/utils/handle.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-04-01
 * Last modified : 2011-04-01
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
 *  An serializable handle object referencing an object through 
 *  a shared pointer.
 *
 */

#ifndef __datatools__utils__handle_h
#define __datatools__utils__handle_h 1

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <datatools/utils/i_predicate.h>

namespace datatools {
  
  namespace utils {

    /** A \t handle object is given the responsability to 
		 *  handle an instance through its pointer using the shared pointer
		 *  mechanism from Boost. The inner hidden pointer can be null.
		 *  It must be initialized with the 'new' construction operator.
		 *  The handle class is copyable and can be used within STL containers.
     *
     *  \b Example:
     *
     *  @code
     *  #include <datatools/utils/handle.h>
     *
     *  {
		 *    using namespace datatools::util;
		 *    handle<int> h0;  // a default int is allocated
		 *    h0.get () = 999; // access the int and modify its value
		 *    cout << "h0 = " << h0.get () << endl; // print it
		 *    h0.reset (new int (0)); // discard the former int 
		 *                            // and allocate a new one
     *    handle<int> h1 (new int (1)); // create a new handle for another int
		 *    {
		 *      handle<int> h2 (h1); // another handle that shares the same int
		 *                           // than the previous handle
		 *      h2.get () = 666;     //change the int value through this handle
		 *    }
		 *    cout << "h1 = " << h1.get () << endl; // print it through the
		 *                                          // first handle
     *  };
     *  @endcode
     *
     */     

		// forward declaration :
		template <class T> struct handle_predicate;

		template <class T>
		class handle
		{
			boost::shared_ptr<T> sp_;

		public:

			handle () : sp_ (new T)
			{
				return;
			}

			handle (T * a_t_ptr ) : sp_ (a_t_ptr)
			{
				return;
			}

			handle (const boost::shared_ptr<T> & a_sp) : sp_ (a_sp)
			{
			}

			bool has_data () const
			{
				return sp_.get () != 0;
			}

			const T & get () const
			{
				if (sp_.get () == 0)
					{
						throw std::logic_error ("datatools::utils::handle::get: Handle has no data !");
					}
				return *sp_.get ();
			}

			T & get ()
			{
				if (sp_.get () == 0)
					{
						throw std::logic_error ("datatools::utils::handle::get: Handle has no data !");
					}
				return *sp_.get ();
			}

			void reset ()
			{
				sp_.reset (new T);
				return;
			}

			void reset (T * a_t_ptr)
			{
				sp_.reset (a_t_ptr);
				return;
			}

			void reset (const boost::shared_ptr<T> & a_sp)
			{
				sp_ = a_sp;
				return;
			}

		private:
			friend class boost::serialization::access; 
			template <class Archive>
			void serialize (Archive & ar, int version)
			{
				ar & boost::serialization::make_nvp ("sp", sp_);
				return;
			}

		public:

			typedef handle_predicate<T> predicate_type;
			
		};

		template <class T>
		struct handle_predicate : public i_predicate <handle<T> >
		{
			const i_predicate<T> & predicate_;
			bool                   no_data_means_false_;
			
			handle_predicate (const i_predicate<T> & a_predicate, 
												bool a_no_data_means_false = true) 
				: predicate_ (a_predicate), 
					no_data_means_false_ (a_no_data_means_false)
			{
				return;
			}
			
			bool operator () (const handle<T> & a_handle) const
			{
				if (! a_handle.has_data()) 
					{
						if (no_data_means_false_) return false;
						throw std::logic_error ("datatools::utils::handle_predicate::operator (): Handle has no data !");
					}
				return (predicate_ (a_handle.get ()));
			}
			
		};
		 
  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__handle_h 

/* end of handle.h */
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
