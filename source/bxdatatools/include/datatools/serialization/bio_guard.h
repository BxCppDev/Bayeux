/* datatools::serialization::bio_guard.h */
/* 
 * Description :
 *
 *  Some useful guard macro related to Boost/Serialisation executable 
 *  building and linkage.
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
 *
 *
 * Useful link : http://gcc.gnu.org/onlinedocs/cpp/
 *
 */ 

#ifndef __datatools__serialization__bio_guard_h
#define __datatools__serialization__bio_guard_h 1

#include <datatools/datatools_config.h>
#include <datatools/serialization/link_guard.h>	

#if DATATOOLS_WITH_BIO != 1					
#warning This executable is built with its own datatools Boost/Serialization code. 
#include <datatools/the_serializable.h>				
#else
#warning This executable must ensure the datatools Boost/Serialization library is loaded. 
namespace datatools {
  namespace serialization {
    struct bio_guard
    {
      bio_guard ()
      {
	dynamic_link_guard & dlg = datatools::serialization::dynamic_link_guard::instance ();
	return;
      }
      static bio_guard _g_trigger_link_guard_;
    };
    bio_guard bio_guard::_g_trigger_link_guard_;
  } // end namespace serialization
} // end namespace datatools
#endif // DATATOOLS_WITH_BIO != 1	

#endif // __datatools__serialization__bio_guard_h

/* end of datatools::serialization::bio_guard.h */
