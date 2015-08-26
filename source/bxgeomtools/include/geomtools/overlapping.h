// \file geomtools/overlapping.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-06-15
 * Last modified: 2015-06-15
 *
 * License: GPL 3
 *
 * Description:
 *
 *  Overlapping detection tools
 *
 */

#ifndef GEOMTOOLS_OVERLAPPING_H
#define GEOMTOOLS_OVERLAPPING_H

// Standard library:
#include <string>
#include <vector>
#include <set>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_placement.h>

namespace geomtools {

  class logical_volume;

  /// \brief Volume overlapping detection algorithms
  class overlapping {

  public:

    /// Constructor
    overlapping();

    /// Detructor
    ~overlapping();

    /// Return the prefix of overlapping related properties
    static const std::string & property_prefix();

    /// Make a key
    static std::string make_key(const std::string & label_);

    static void extract(const datatools::properties & source_,
                        datatools::properties & target_);

    static bool has_flag(const datatools::properties & config_,
                         const std::string & flag_);

    static bool has_key(const datatools::properties & config_,
                        const std::string & key_);

    /// \brief Type of overlapping
    enum overlap_type {
      OVERLAP_SIMPLE = 0,  ///< Two positionned volumes are overlapping
      OVERLAP_MOTHER_DAUGHTER = 1 ///< A daughter volume overlaps its mother volume
    };

    /// Information about a detected overlap
    struct overlap_info {
      overlap_type type;
      std::string name_first; ///< Name of the first overlapping volume
      int         item_first; ///< Copy number of the first overlapping volume
      std::string name_second; ///< Name of the second overlapping volume
      int         item_second; ///< Copy number of the second overlapping volume
      // std::string which_item; ///< Description of the overlapping volume
      vector_3d   vertex; ///< Overlapping vertex
    };

    /// Overlapping search report
    struct report {
      std::vector<overlap_info> overlaps; ///< Collection of overlapping informations
      void reset();
    };

    enum flag_type {
      FLAG_NONE                     =  0x0,
      FLAG_WIRES_SAMPLING           =  0x1,
      FLAG_WIRES_LOW_SAMPLING       =  0x2,
      FLAG_WIRES_HIGH_SAMPLING      =  0x4,
      FLAG_WIRES_VERY_HIGH_SAMPLING =  0x8,
      FLAG_WIRES_HUGE_SAMPLING      = 0x10,
      FLAG_OVERLAP_ALL              = 0x20,
      FLAG_CHECK_OLD_FIRST          = 0x40,
      FLAG_CHECK_INHIBIT_SECOND     = 0x80,
      FLAG_DEBUG_DISPLAY            = 0x100
    };

    static flag_type get_flag_from_label(const std::string & label_);

    /// Check if a mother volume and its daughter volume have some intersecting surfaces.
    /**
     *               ........
     *           ...'        `...         ------
     *          :                :       /      \
     *         :    ------        :     | D3     |
     *        :    /      \        :     \      /
     *        :   | D1     |     ------   ------
     *        :    \      /    /   *   \
     *         :    ------    |  **     |
     *          `...          |**  D2   |
     *              `........' \_______/
     *
     *
     */
    bool check_mother_daughter_surface_intersection(const i_shape_3d & mother_shape_,
                                                    const i_shape_3d & daughter_shape_,
                                                    const i_placement & daughter_placement_,
                                                    report & report_,
                                                    uint32_t flags_ = 0) const;

    /// Check if a daughter overlaps its mother volume's outside region
    /**
     *               ........
     *           ...'        `...         ******
     *          :                :       ********
     *         :    ------        :     **D3******
     *        :    /      \        :     ********
     *        :   | D1     |     --****   ******
     *        :    \      /    /   *****
     *         :    ------    |  ********
     *          `...          |****D2****
     *              `........' *********
     *
     *
     */
    bool check_mother_daughter_overlap(const i_shape_3d & mother_shape_,
                                       const i_shape_3d & daughter_shape_,
                                       const i_placement & daughter_placement_,
                                       report & report_,
                                       uint32_t flags_ = 0) const;

    bool check_two_volumes_overlap(const i_shape_3d & shape_old_,
                                   const i_placement & placement_old_,
                                   const i_shape_3d & shape_new_,
                                   const i_placement & placement_new_,
                                   report &, uint32_t flags_ = 0) const;

    datatools::logger::priority get_logging() const;

    void set_logging(datatools::logger::priority);

    static void make_vertice_unique(const wires_type & wires_,
                                    std::set<geomtools::vector_3d> & vertice_);


  private:

    datatools::logger::priority _logging_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_OVERLAPPING_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
