// -*- mode: c++; -*- 
// genbb_help.cc
/*
 * Copyright 2007-2011 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Publi * License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Publi * License for more details.
 * 
 * You should have received a copy of the GNU General Publi * License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <genbb_help/genbb_help.h>

namespace genbb {

  bool library_config::g_use_old_serialization_tag = false; 

  void library_config::set_use_old_serialization_tag (bool u_ )
  {
    library_config::g_use_old_serialization_tag = u_;
    return;
  }
  
} // end of namespace genbb

// end of genbb_help.cc
