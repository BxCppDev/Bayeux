/// \file datatools/make_configuration.h
/* License: GPL3
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
 *   Free functions for creating/writing properties/multi_properties from files
 *
 */

#ifndef DATATOOLS_MAKE_PROPERTIES_H
#define DATATOOLS_MAKE_PROPERTIES_H

#include <cstdint>
#include <string>

namespace datatools
{
  // Forward declare so we can include this header in properties/multi_properties
  // headers. Clients then only need to include those headers to make use of these
  // functions. A temporary measure until longer term updates can be done.
  class properties;
  class multi_properties;


  //! Fill a properties object using an input file
  /*!
   *  \param[in] filename_ file to read/parse data from
   *  \param[in,out] props_ properties instance to fill with data
   *  \param[in] options_ flags to pass to the file reader
   *
   *  \sa properties_config
   */
  void read_config(const std::string & filename_, properties & props_, uint32_t options_ = 0);

  //! Write a properties object to an output file as text
  /*!
   *  \param[in] filename_ file to write data to
   *  \param[in] props_ properties instance to write
   *  \param[in] options_ flags to pass to the file writer
   *
   *  \sa properties_config
   */
  void write_config(const std::string & filename_, const properties & props_, uint32_t options_ = 0);

  //! Fill a multi_properties object using an input file
  /*!
   *  \param[in] filename_ file to read/parse data from
   *  \param[in,out] props_ multi_properties instance to fill with data
   *  \param[in] options_ flags to pass to the file reader
   *
   *  \sa multi_properties_config
   */
  void read_config(const std::string & filename_, multi_properties & props_, uint32_t options_ = 0);

  //! Write a multi_properties object to an output file as text
  /*!
   *  \param[in] filename_ file to write data to
   *  \param[in] props_ multi_properties instance to write
   *  \param[in] options_ flags to pass to the file writer
   *
   *  \sa properties_config
   */
  void write_config(const std::string & filename_, const multi_properties & props_, uint32_t options_ = 0);

} // namespace datatools

#endif // DATATOOLS_MAKE_PROPERTIES_H
