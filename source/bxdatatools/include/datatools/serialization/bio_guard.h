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

#if DATATOOLS_XBIO == 1	                                
#warning This executable must be linked with the datatools auxiliary Boost/Serialization DLL. 
#else
#if DATATOOLS_EBIO == 1					
#warning This executable must be linked with the datatools DLL built with Boost/Serialization embedded code. 
#else
#warning This executable must be built with its own datatools Boost/Serialization code. 
#include <datatools/the_serializable.h>				
#endif // DATATOOLS_EBIO == 1
#endif // DATATOOLS_XBIO == 1	  							

#endif // __datatools__serialization__bio_guard_h

/* end of datatools::serialization::bio_guard.h */
