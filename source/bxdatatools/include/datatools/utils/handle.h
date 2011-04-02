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
 *  An serializable handle object referencing an object through a shared pointer
 *
 */

#ifndef __datatools__utils__handle_h
#define __datatools__utils__handle_h 1

#include <iostream>
#include <typeinfo>

#include <boost/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace datatools {
  
  namespace utils {

    /** A \t handle object is given the responsability to 
		 *  handle an instance through its pointer using the shared pointer
		 *  mechanism from Boost. The inner hidden pointer cannot be null
		 *  and must be initialized with the 'new' construction operator.
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

		template <class T>
		class handle
		{
			boost::shared_ptr<T> sp_;

		public:

			handle (T * a_t_ptr = 0) : sp_ (a_t_ptr != 0 ? a_t_ptr : new T)
			{
				return;
			}

			const T & get () const
			{
				return *sp_.get ();
			}

			T & get ()
			{
				return *sp_.get ();
			}

			T & reset (T * a_t_ptr = 0)
			{
				sp_.reset (a_t_ptr != 0 ? a_t_ptr : new T);
				return *sp_.get ();
			}

		private:
			friend class boost::serialization::access; 
			template <class Archive>
			void serialize (Archive & ar, int version)
			{
				ar & boost::serialization::make_nvp ("sp", sp_);
				return;
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
