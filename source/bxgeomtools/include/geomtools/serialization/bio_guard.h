/* geomtools::serialization::bio_guard.h */
/* 
 * Description :
 *
 *  Some useful guard macro related to Boost/Serialisation executable 
 *  building and linkage.
 *
 * Copyright (C) 2011-2012 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef __geomtools__serialization__bio_guard_h
#define __geomtools__serialization__bio_guard_h 1

#include <geomtools/geomtools_config.h>

#if GEOMTOOLS_WITH_BIO != 1					
#warning This executable must be built with its own geomtools Boost/Serialization code. 
#include <geomtools/serialization/the_serializable.h>	
#endif // GEOMTOOLS_WITH_BIO != 1		  				

#endif // __geomtools__serialization__bio_guard_h

/* end of geomtools::serialization::bio_guard.h */