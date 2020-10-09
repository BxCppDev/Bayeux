/// \file dpp/histogram_service.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-10-23
 * Last modified : 2013-02-16
 *
 * Copyright (C) 2012-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Histogram service.
 *
 */

#ifndef DPP_HISTOGRAM_SERVICE_H
#define DPP_HISTOGRAM_SERVICE_H 1

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/base_service.h>
#include <datatools/service_tools.h>
// - Bayeux/mygsl:
#include <mygsl/histogram_pool.h>

// This project:
#include <dpp/dpp_config.h>

class TH1D;
class TH2D;

namespace datatools {
    class properties;
}

namespace dpp {

  /// \brief A service for managing 1D and 2D histograms
  class histogram_service
    : public datatools::base_service
  {
  public:

    bool has_output_files () const;

    const std::vector<std::string> & get_output_files () const;

    std::vector<std::string> & grab_output_files ();

    void add_output_file (const std::string & fn_);

    const mygsl::histogram_pool & get_pool () const;

    mygsl::histogram_pool & grab_pool ();

    histogram_service ();

    ~histogram_service () override;

    bool is_initialized () const override;

    int initialize (const datatools::properties  & config_,
                            datatools::service_dict_type & service_dict_) override;

    int reset () override;

    void load_from_boost_file (const std::string & filename_);

    void store_as_boost_file (const std::string & filename_) const;

    void store_as_root_file (const std::string & filename_) const;

    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const override;

    static void export_to_root (const mygsl::histogram_1d & h1d_,
                                const std::string & name_,
                                const std::string & title_,
                                TH1D & rh1d_, bool stats_ = false);

    static void export_to_root (const mygsl::histogram_2d & h2d_,
                                const std::string & name_,
                                const std::string & title_,
                                TH2D & rh2d_, bool stats_ = false);

  protected:

    void _at_init ();

    void _at_reset ();

  private:

    bool                      _initialized_; /// Initialization flag
    mygsl::histogram_pool     _pool_;        /// Embeded histogram pool
    std::vector<std::string>  _output_files_; /// List of output files
    bool                      _root_export_stats_; /// Flag for stat box (ROOT export)
    std::string               _root_export_title_prefix_; /// General common user histogram title prefix (ROOT export)
    std::string               _root_export_name_suffix_; /// General common user histogram name suffix (ROOT export)
    // Registration :
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE (histogram_service)

  };

}  // end of namespace dpp

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(dpp::histogram_service)

#endif // DPP_HISTOGRAM_SERVICE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
