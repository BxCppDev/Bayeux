/// \file geomtools/regular_3d_mesh_placement.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-06-12
 * Last modified: 2015-06-12
 *
 * License: GPL 3
 *
 * Description:
 *
 *  Regular 3D mesh placement in a logical volume
 *
 */

#ifndef GEOMTOOLS_REGULAR_3D_MESH_PLACEMENT_H
#define GEOMTOOLS_REGULAR_3D_MESH_PLACEMENT_H

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
#include <boost/utility.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/logger.h>

// This project:
#include <geomtools/i_placement.h>
#include <geomtools/placement.h>
#include <geomtools/logical_volume.h>

namespace geomtools {

  //! \brief Regular 3D mesh placement of small daughter volumes with respect to a mother logical volume
  class regular_3d_mesh_placement
    : public i_placement
    , private boost::noncopyable
  {
  public:

    /// Check the validity of the mesh placement
    bool is_valid() const;

    /// Invalidate the mesh placement
    void invalidate();

    /// Set the logging priprity threshold
    void set_logging(datatools::logger::priority);

    /// Return the logging priprity threshold
    datatools::logger::priority get_logging() const;

    //! Check if a cache file path is set
    bool has_cache_file_path() const;

    //! Return the cache file path
    const std::string & get_cache_file_path() const;

    //! Set the cache file path
    void set_cache_file_path(const std::string &);

    void set_log_vol(const logical_volume &);

    bool has_log_vol() const;

    void set_node_log_vol(const logical_volume &);

    bool has_node_log_vol() const;

    void set_steps(double step_x_, double step_y_, double step_z_);

    void set_starts(double x0_, double y0_, double z0_);

    void set_stops(double x1_, double y1_, double z1_);

    void set_step_x(double step_x_);

    void set_step_y(double step_y_);

    void set_step_z(double step_z_);

    void set_start_x(double x0_);

    void set_start_y(double y0_);

    void set_start_z(double z0_);

    void set_stop_x(double x1_);

    void set_stop_y(double y1_);

    void set_stop_z(double z1_);

    void set_overlapping_flags(uint32_t);

    uint32_t get_overlapping_flags() const;

    bool has_only_one_rotation() const override;

    size_t get_dimension() const override;

    bool is_replica() const override;

    size_t get_number_of_items() const override;

    void get_placement(int item_, placement & p_) const override;

    size_t compute_index_map(std::vector<uint32_t> & map_,
                                     int item_) const override;

    const placement & get_basic_placement() const;

    regular_3d_mesh_placement();

    ~regular_3d_mesh_placement() override;

    bool is_initialized() const;

    void initialize();

    void initialize(const datatools::properties & config_);

    virtual void reset();

    void tree_dump(std::ostream & out_  = std::clog,
                           const std::string & title_ = "geomutils::regular_grid_placement",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  protected:

    void _load_cache(const std::string & cache_file_path_);

    void _store_cache(const std::string & cache_file_path_);

    void _set_defaults();

    void _init(const datatools::properties & config_);

    void _fini();

  private:

    bool _initialized_ = false; ///< Initialization flag
    datatools::logger::priority _logging_; ///< Logging priority
    const logical_volume * _log_vol_ = nullptr; ///< Reference to a logical volume
    const logical_volume * _node_log_vol_ = nullptr; ///< Reference to a replicated node logical volume
    uint32_t _overlapping_flags_; ///< Flags for overlapping search
    std::string _cache_file_path_; ///< Cache file path
    double _start_x_; ///< Starting X coordinates
    double _start_y_; ///< Starting Y coordinates
    double _start_z_; ///< Starting Y coordinates
    double _stop_x_;  ///< Stopping X coordinates
    double _stop_y_;  ///< Stopping Y coordinates
    double _stop_z_;  ///< Stopping Y coordinates
    double _step_x_;  ///< Step length along axis 0 (X)
    double _step_y_;  ///< Step length along axis 1 (Y)
    double _step_z_;  ///< Step length along axis 2 (Z)
    size_t    _nsamples_x_;
    size_t    _nsamples_y_;
    size_t    _nsamples_z_;
    placement _basic_placement_;

    struct data;
    boost::scoped_ptr<data> _data_; ///< Internal working data

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_REGULAR_3D_MESH_PLACEMENT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
