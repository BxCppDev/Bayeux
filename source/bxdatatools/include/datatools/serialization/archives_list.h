/* archives_list.h */
/* 
 * Description :
 *
 *  The list of supported archives.
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
 */ 

#ifndef __datatools__serialization__archives_list_h
#define __datatools__serialization__archives_list_h 1

#include <boost/archive/polymorphic_iarchive.hpp>
#include <boost/archive/polymorphic_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

// 2012-06-07 FM : switch to Portable Archive v5.0
//#include <boost/archive/portable_binary_iarchive.hpp>
//#include <boost/archive/portable_binary_oarchive.hpp>
#include <eos/portable_iarchive.hpp>
#include <eos/portable_oarchive.hpp>

#endif // __datatools__serialization__archives_list_h

/* end of archives_list.h */
