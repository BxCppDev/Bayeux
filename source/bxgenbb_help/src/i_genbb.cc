// -*- mode: c++; -*- 
// i_genbb.cc
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 *
 */

#include <genbb_help/i_genbb.h>

namespace genbb {
 
  i_genbb::i_genbb ()
  {
  }
  
  i_genbb::~i_genbb ()
  {
  }

  void i_genbb::load_next (primary_event & event_, 
			   bool compute_classification_)
  { 
    _load_next (event_, compute_classification_);
    return;
  }

} // end of namespace genbb

// end of i_genbb.cc
