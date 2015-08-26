/// \file geomtools/overlapping.cc

// Ourselves:
#include <geomtools/overlapping.h>

// Standard library:
#include <sstream>

// This project:
#include <geomtools/logical_volume.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/i_shape_3d.h>

namespace geomtools {

  void overlapping::report::reset()
  {
    overlaps.clear();
    return;
  }

  // static
  const std::string & overlapping::property_prefix()
  {
    static const std::string _opp("overlapping.");
    return _opp;
  }

  // static
  std::string overlapping::make_key(const std::string & label_)
  {
    std::ostringstream key_oss;
    key_oss << overlapping::property_prefix() << label_;
    return key_oss.str ();
  }

  overlapping::overlapping()
  {
    _logging_ = datatools::logger::PRIO_FATAL;
    return;
  }

  overlapping::~overlapping()
  {
    return;
  }

  // static
  overlapping::flag_type overlapping::get_flag_from_label(const std::string & label_)
  {
    if (label_ == "low_sampling") return FLAG_WIRES_LOW_SAMPLING;
    if (label_ == "high_sampling") return FLAG_WIRES_HIGH_SAMPLING;
    if (label_ == "very_high_sampling") return FLAG_WIRES_VERY_HIGH_SAMPLING;
    if (label_ == "huge_sampling") return FLAG_WIRES_HUGE_SAMPLING;
    if (label_ == "overlap_all") return FLAG_OVERLAP_ALL;
    if (label_ == "check_old_first") return FLAG_CHECK_OLD_FIRST;
    if (label_ == "check_inhibit_second") return FLAG_CHECK_INHIBIT_SECOND;
    return FLAG_NONE;
  }

  // static
  void overlapping::extract(const datatools::properties & source_,
                            datatools::properties & target_)
  {
    source_.export_starting_with(target_, overlapping::property_prefix());
    return;
  }

  // static
  bool overlapping::has_flag(const datatools::properties & config_,
                             const std::string & flag_)
  {
    return (config_.has_flag(overlapping::make_key (flag_)));
  }

  // static
  bool overlapping::has_key(const datatools::properties & config_,
                            const std::string & key_)
  {
    return (config_.has_key(overlapping::make_key(key_)));
  }

  datatools::logger::priority overlapping::get_logging() const
  {
    return _logging_;
  }

  void overlapping::set_logging(datatools::logger::priority lp_)
  {
    _logging_ = lp_;
    return;
  }

  // static
  void overlapping::make_vertice_unique(const wires_type & wires_,
                                        std::set<geomtools::vector_3d> & vertice_)
  {
    for (wires_type::const_iterator iwire = wires_.begin();
         iwire != wires_.end();
         iwire++ ) {
      const polyline_type & poly = *iwire;
      for (polyline_type::const_iterator ivertex = poly.begin();
           ivertex != poly.end();
           ivertex++ ) {
        const vector_3d & surface_vtx = *ivertex;
        vertice_.insert(surface_vtx);
      }
    }
    return;
  }

  bool overlapping::check_two_volumes_overlap(const i_shape_3d & shape_old_,
                                              const i_placement & placement_old_,
                                              const i_shape_3d & shape_new_,
                                              const i_placement & placement_new_,
                                              report & report_,
                                              uint32_t flags_) const
  {
    bool overlap_all = flags_ & FLAG_OVERLAP_ALL;
    bool check_old_first = flags_ & FLAG_CHECK_OLD_FIRST;
    bool check_inhibit_second = flags_ & FLAG_CHECK_INHIBIT_SECOND;
    uint32_t wires_options = i_wires_3d_rendering::WR_NONE;

    if (flags_ & FLAG_WIRES_HUGE_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_HUGE_DENSITY
        | i_wires_3d_rendering::WR_BASE_HUGE_ANGLE_SAMPLING;

    } else if (flags_ & FLAG_WIRES_VERY_HIGH_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_VERY_HIGH_DENSITY
        | i_wires_3d_rendering::WR_BASE_VERY_HIGH_ANGLE_SAMPLING;

    } else if (flags_ & FLAG_WIRES_HIGH_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
        | i_wires_3d_rendering::WR_BASE_HIGH_ANGLE_SAMPLING;

    } else if (flags_ & FLAG_WIRES_LOW_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_LOW_DENSITY
        | i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING;
    }

    if (wires_options == i_wires_3d_rendering::WR_NONE) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID;
    }

    const i_shape_3d  * check_shapes[2];
    check_shapes[0] = 0;
    check_shapes[1] = 0;
    const i_placement * check_placements[2];
    check_placements[0] = 0;
    check_placements[1] = 0;
    const i_placement * check_placements_2[2];
    check_placements_2[0] = 0;
    check_placements_2[1] = 0;

    if (check_old_first) {
      // We check if the surface of the old shape overlap the new shape volume:
      check_shapes[0] = &shape_new_;
      check_placements[0] = &placement_new_;
      check_placements_2[0] = &placement_old_;
      check_shapes[1] = &shape_old_;
      check_placements[1] = &placement_old_;
      check_placements_2[1] = &placement_new_;
    } else {
      // We check if the surface of the new shape overlaps the old shape volume:
      check_shapes[0] = &shape_old_;
      check_placements[0] = &placement_old_;
      check_placements_2[0] = &placement_new_;
      check_shapes[1] = &shape_new_;
      check_placements[1] = &placement_new_;
      check_placements_2[1] = &placement_old_;
    }

    for (int k = 0; k < 2; k++) {
      if (check_inhibit_second && k == 1) {
        break;
      }
      const i_shape_3d  * check_shape = check_shapes[k];
      const i_placement * check_placement = check_placements[k];
      const i_placement * check_placement_2 = check_placements_2[k];
      wires_type wires;
      check_shapes[(k+1) % 2]->generate_wires_self(wires, wires_options);
      std::set<vector_3d> vertice_on_surface;
      make_vertice_unique(wires, vertice_on_surface);
      for (int i = 0; i < (int) check_placement->get_number_of_items(); i++) {
        placement plcmt_i;
        check_placement->compute_placement(i, plcmt_i);
        for (int j = 0; j < (int) check_placement_2->get_number_of_items(); j++) {
          placement plcmt_j;
          check_placement_2->compute_placement(j, plcmt_j);
          for (std::set<vector_3d>::const_iterator ivertex = vertice_on_surface.begin();
               ivertex != vertice_on_surface.end();
               ivertex++) {
            const vector_3d & surface_vtx = *ivertex;
            vector_3d surface_vertex_in_mother;
            plcmt_j.child_to_mother(surface_vtx, surface_vertex_in_mother);
            vector_3d surface_vertex_in_checked;
            plcmt_i.mother_to_child(surface_vertex_in_mother, surface_vertex_in_checked);
            if (check_shape->check_inside(surface_vertex_in_checked, 0.0)) {
              {
                overlap_info dummy;
                report_.overlaps.push_back(dummy);
              }
              overlap_info & oi = report_.overlaps.back();
              oi.type = OVERLAP_SIMPLE;
              oi.name_first = check_shape->get_shape_name();
              oi.item_first = i;
              oi.name_second = check_shapes[(k+1) % 2]->get_shape_name();
              oi.item_second = j;
              oi.vertex = surface_vertex_in_mother;
              if (!overlap_all) break;
            }
          } // ivertex
          if (!overlap_all && report_.overlaps.size() > 0) {
            break;
          }
        } // placement j
        if (!overlap_all && report_.overlaps.size() > 0) {
          break;
        }
      } // placement i
      if (report_.overlaps.size() > 0) {
        return true;
      }
    } // k
    return report_.overlaps.size() > 0;
  }

  bool overlapping::check_mother_daughter_overlap(const i_shape_3d & mother_shape_,
                                                  const i_shape_3d & daughter_shape_,
                                                  const i_placement & daughter_placement_,
                                                  report & report_,
                                                  uint32_t flags_) const
  {
    DT_LOG_DEBUG(_logging_, "Entering...");
    DT_LOG_DEBUG(_logging_, "Mother shape = '" << mother_shape_.get_shape_name() << "'");
    wires_type wires;
    bool overlap_all = flags_ & FLAG_OVERLAP_ALL;
    DT_LOG_DEBUG(_logging_, "overlap_all = " << overlap_all);
    uint32_t wires_options = i_wires_3d_rendering::WR_NONE;

    if (flags_ & FLAG_WIRES_HUGE_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_HUGE_DENSITY
        | i_wires_3d_rendering::WR_BASE_HUGE_ANGLE_SAMPLING;

    } else if (flags_ & FLAG_WIRES_VERY_HIGH_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_VERY_HIGH_DENSITY
        | i_wires_3d_rendering::WR_BASE_VERY_HIGH_ANGLE_SAMPLING;

    } else if (flags_ & FLAG_WIRES_HIGH_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
        | i_wires_3d_rendering::WR_BASE_HIGH_ANGLE_SAMPLING;

    } else if (flags_ & FLAG_WIRES_LOW_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_LOW_DENSITY
        | i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING;
    }

    if (wires_options == i_wires_3d_rendering::WR_NONE) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID;
    }

    //
    daughter_shape_.generate_wires_self(wires, wires_options);
    std::set<geomtools::vector_3d> vertice_on_daughter_surface;
    make_vertice_unique(wires, vertice_on_daughter_surface);

    for (int i = 0; i < (int) daughter_placement_.get_number_of_items(); i++) {
      DT_LOG_DEBUG(_logging_, "Daughter placement #" << i << " for daughter shape '"
                   << daughter_shape_.get_shape_name() << "'");
      placement plcmt_i;
      daughter_placement_.compute_placement(i, plcmt_i);
      DT_LOG_DEBUG(_logging_, "Daughter placement #" << i << " = " << plcmt_i);
      int wire_count = 0;
      int vertex_count = 0;
      for (std::set<geomtools::vector_3d>::const_iterator ivertex = vertice_on_daughter_surface.begin();
           ivertex != vertice_on_daughter_surface.end();
           ivertex++) {
        const vector_3d & daughter_surface_vtx = *ivertex;
        vector_3d daughter_surface_vtx_in_mother;
        plcmt_i.child_to_mother(daughter_surface_vtx, daughter_surface_vtx_in_mother);
        DT_LOG_DEBUG(_logging_, "    Daughter surface vertex in mother = "
                     << daughter_surface_vtx_in_mother);
        if (mother_shape_.check_outside(daughter_surface_vtx_in_mother, 0.0)) {
          DT_LOG_DEBUG(_logging_, "     --> Daughter surface vertex IS outside the mother volume");
          {
            overlap_info dummy;
            report_.overlaps.push_back(dummy);
          }
          overlap_info & oi = report_.overlaps.back();
          oi.type        = OVERLAP_MOTHER_DAUGHTER;
          oi.name_first  = mother_shape_.get_shape_name();;
          oi.item_first  = -1;
          oi.name_second = daughter_shape_.get_shape_name();
          oi.item_second = i;
          oi.vertex      = daughter_surface_vtx_in_mother;
          DT_LOG_DEBUG(_logging_, "     --> Add a new overlap record...");
          if (!overlap_all) break;
        } else {
          DT_LOG_DEBUG(_logging_, "     >>> Daughter surface vertex is NOT outside the mother volume");
        }
        vertex_count++;
        if (!overlap_all && report_.overlaps.size() > 0) {
          break;
        }
      } // ivertex
      if (!overlap_all && report_.overlaps.size() > 0) {
        break;
      }
    } // daughter
    DT_LOG_DEBUG(_logging_, "Exiting.");
    return report_.overlaps.size() > 0;
  }

  bool overlapping::check_mother_daughter_surface_intersection(const i_shape_3d & mother_shape_,
                                                               const i_shape_3d & daughter_shape_,
                                                               const i_placement & daughter_placement_,
                                                               report & report_,
                                                               uint32_t flags_) const
  {
    wires_type wires;
    uint32_t wires_options = i_wires_3d_rendering::WR_NONE;
    bool overlap_all = flags_ & FLAG_OVERLAP_ALL;

    if (flags_ & FLAG_WIRES_HUGE_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_HUGE_DENSITY
        | i_wires_3d_rendering::WR_BASE_HUGE_ANGLE_SAMPLING;

    } else if (flags_ & FLAG_WIRES_VERY_HIGH_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_VERY_HIGH_DENSITY
        | i_wires_3d_rendering::WR_BASE_VERY_HIGH_ANGLE_SAMPLING;

    } else if (flags_ & FLAG_WIRES_HIGH_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
        | i_wires_3d_rendering::WR_BASE_HIGH_ANGLE_SAMPLING;

    } else if (flags_ & FLAG_WIRES_LOW_SAMPLING) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID_LOW_DENSITY
        | i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING;
    }

    if (wires_options == i_wires_3d_rendering::WR_NONE) {
      wires_options = i_wires_3d_rendering::WR_BASE_GRID;
    }

    mother_shape_.generate_wires_self(wires, wires_options);
    std::set<geomtools::vector_3d> vertice_on_mother_surface;
    make_vertice_unique(wires, vertice_on_mother_surface);

    for (int i = 0; i < (int) daughter_placement_.get_number_of_items(); i++) {
      placement plcmt_i;
      daughter_placement_.compute_placement(i, plcmt_i);
      for (std::set<geomtools::vector_3d>::const_iterator ivertex = vertice_on_mother_surface.begin();
           ivertex != vertice_on_mother_surface.end();
           ivertex++) {
        const vector_3d & log_vtx = *ivertex;
        vector_3d log_vtx_in_daughter;
        plcmt_i.mother_to_child(log_vtx, log_vtx_in_daughter);
        if (daughter_shape_.check_inside(log_vtx_in_daughter, 0.0)) {
          {
            overlap_info dummy;
            report_.overlaps.push_back(dummy);
          }
          overlap_info & oi = report_.overlaps.back();
          oi.type        = OVERLAP_MOTHER_DAUGHTER;
          oi.name_first  = mother_shape_.get_shape_name();;
          oi.item_first  = -1;
          oi.name_second = daughter_shape_.get_shape_name();
          oi.item_second = i;
          oi.vertex      = log_vtx;
          if (!overlap_all) break;
        }
      }
      if (!overlap_all && report_.overlaps.size() > 0) {
        break;
      }
    } // ivertex
    return report_.overlaps.size() > 0;
  }

  /*
    bool overlapping::check_logical_daughters(const logical_volume & log_vol_,
    const placement &
    report & or_,
    int level_)
    {
    or_.reset();
    DT_THROW_IF(!log_vol_.has_shape(), std::logic_error,
    "Logical volume '" << log_vol_.get_name() << "' has no shape!");
    const i_shape_3d & log_shape = log_vol_.get_shape();
    // const face_info_collection_type & log_faces = log_shape.get_computed_faces();
    wires_type wires;
    uint32_t wires_options = 0;
    datatools::properties log_overlapping;
    overlapping::extract(log_vol_.get_parameters(), log_overlapping);
    if (overlapping::has_key(log_overlapping, "wires_sampling")) {
    std::string wires_sampling_label = log_overlapping.fetch_string(overlapping::make_key("wires_sampling"));
    if (wires_sampling_label == "low") {
    wires_options = i_wires_3d_rendering::WR_BASE_GRID_LOW_DENSITY
    | i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING;
    } else if (wires_sampling_label == "high") {
    wires_options = i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
    | i_wires_3d_rendering::WR_BASE_HIGH_ANGLE_SAMPLING;
    } else if (wires_sampling_label == "very_high") {
    wires_options = i_wires_3d_rendering::WR_BASE_GRID_VERY_HIGH_DENSITY
    | i_wires_3d_rendering::WR_BASE_VERY_HIGH_ANGLE_SAMPLING;
    } else if (wires_sampling_label == "huge") {
    wires_options = i_wires_3d_rendering::WR_BASE_GRID_HUGE_DENSITY
    | i_wires_3d_rendering::WR_BASE_HUGE_ANGLE_SAMPLING;
    } else {
    wires_options = i_wires_3d_rendering::WR_BASE_GRID;
    }
    }

    if (wires_options == 0) {
    wires_options =  i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
    | i_wires_3d_rendering::WR_BASE_HIGH_ANGLE_SAMPLING;
    log_shape.generate_wires_self(wires, wires_options);
    }
    const logical_volume::physicals_col_type & daughters = log_vol_.get_physicals();
    for (logical_volume::physicals_col_type::const_iterator idaugther =
    daughters.begin();
    idaugther != daughters.end();
    idaugther++) {
    bool check_overlap = true;
    const physical_volume * daughter_phys = idaugther->second;
    const i_placement & daughter_placement = daughter_phys->get_placement();
    DT_THROW_IF(!daughter_phys->get_logical().has_shape(), std::logic_error,
    "Daughter volume '" << idaugther->first << "' of logical '"
    << log_vol_.get_name() << "' has no shape!");
    const i_shape_3d & daughter_shape = daughter_phys->get_logical().get_shape();
    for (int i = 0; i < (int) daughter_placement.get_number_of_items(); i++) {
    placement pi;
    daughter_placement.get_placement(i, pi);
    for (wires_type::const_iterator iwire = wires.begin();
    iwire != wires.end();
    iwire++ ) {
    const polyline_type & poly = *iwire;
    for (polyline_type::const_iterator ivertex = poly.begin();
    ivertex != poly.end();
    ivertex++ ) {
    const vector_3d & log_vtx = *ivertex;
    vector_3d log_vtx_in_daughter;
    pi.mother_to_child(log_vtx, log_vtx_in_daughter);
    if (daughter_shape.check_inside(log_vtx_in_daughter, 0.0)) {
    {
    overlap_info dummy;
    or_.overlaps.push_back(dummy);
    }
    overlap_info & oi = or_.overlaps.back();
    oi.type = OVERLAP_MOTHER_DAUGHTER;
    oi.name = idaugther->first;
    oi.item = i;
    oi.what = "daughter volume";
    oi.vertex = log_vtx;
    break;
    }
    }
    if (or_.overlaps.size() > 0) {
    break;
    }
    }
    if (or_.overlaps.size() > 0) {
    break;
    }
    }
    if (or_.overlaps.size() > 0) {
    break;
    }
    }

    return or_.overlaps.size() > 0;
    }
  */

} // end of namespace geomtools
