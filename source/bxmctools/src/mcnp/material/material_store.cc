// mctools/mcnp/material/material_store.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <mctools/mcnp/material/material_store.h>

// Standard library:
#include <sstream>
#include <iomanip>
#include <list>

// Third party:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <materials/material.h>
#include <materials/element.h>
#include <materials/isotope.h>

// This project:
#include <mctools/mcnp/format.h>

namespace mctools {

  namespace mcnp {

    namespace material {

      material_store::material_store()
      {
        return;
      }

      material_store::~material_store()
      {
        _materials_.clear();
        return;
      }

      material & material_store::add_material(int id_)
      {
        DT_THROW_IF(id_ < 1, std::logic_error,
                    "Invalid material id '" << id_ << "'!");
        material_ptr_type ptr(new material);
        material & mat = *ptr.get();
        mat.set_id(id_);
        // DT_THROW_IF(!mat_.is_valid(), std::logic_error,
        //             "Cannot add invalid material '" << mat_.get_name() << "'!");
        DT_THROW_IF(_materials_.find(mat.get_id()) != _materials_.end(),
                    std::logic_error,
                    "Material store already contains material with ID=[" << mat.get_id() << "]!");
        _materials_[mat.get_id()] = ptr;
        return mat;
      }

      void material_store::remove_material(int id_)
      {
        material_dict_type::iterator found = _materials_.find(id_);
        if (found != _materials_.end()) {
          _materials_.erase(found);
        }
        return;
      }

      bool material_store::has_material(int id_) const
      {
        return _materials_.find(id_) != _materials_.end();
      }

      const material_ptr_type & material_store::get(int id_) const
      {
        material_dict_type::const_iterator found = _materials_.find(id_);
        DT_THROW_IF(found == _materials_.end(), std::logic_error,
                    "No material with identifier = [" << id_ << "]!");
        return found->second;
      }

      void material_store::print_card(std::ostream & out_, uint32_t flags_) const
      {
        if (flags_ & format::CARD_COMMENT) {
          format::add_comment_line(out_, "List of materials:");
        }
        for (material_dict_type::const_iterator imat = _materials_.begin();
             imat != _materials_.end();
             imat++) {
          const material & mat = *imat->second.get();
          mat.print_card(out_, flags_);
        }
        return;
      }

      void material_store::build_from_bxmaterials(const ::materials::manager & mat_mgr_)
      {
        _materials_.clear();
        const std::list<std::string> & mats = mat_mgr_.get_ordered_materials();
        int mat_counter = 0;
        for (std::list<std::string>::const_iterator imat = mats.begin();
             imat != mats.end();
             imat++) {
          const materials::material & bxmat = mat_mgr_.get_material(*imat);
          material & mat = add_material(++mat_counter);
          if (! build_material_from_bxmaterials(bxmat, mat)) {
            remove_material(mat_counter);
          }
        }
        return;
      }

      bool material_store::build_material_from_bxmaterials(const ::materials::material & bxmat_,
                                                           material & mat_)
      {
        bxmat_.tree_dump(std::cerr, "Bayeux material: ", "DEVEL: ");
        std::map<int, double> iso_fractions;
        try {
          DT_THROW_IF(bxmat_.is_composed_by_mean_z_a(), std::logic_error,
                      "Unsupported material composition mode!");
          const ::materials::composition_map_type & comp = bxmat_.get_composition();
          // Scan the Bayeux' material composition:
          for (::materials::composition_map_type::const_iterator icomp = comp.begin();
               icomp != comp.end();
               icomp++) {
            // const std::string & comp_name = icomp->first;
            const materials::compound_entry & centry = icomp->second;
            DT_THROW_IF(centry.has_material(),
                        std::logic_error,
                        "Unsupported composition entry: Cannot use mix of materials!");
            DT_THROW_IF(!centry.has_element(),
                        std::logic_error,
                        "Unsupported composition entry: Missing element!");
            const materials::element & bxelem = centry.get_element();
            std::cerr << "DEVEL: mcnp::material_store::build_material_from_bxmaterials: bxelem = '" << bxelem.get_name() << "'" << std::endl;
            // Fraction from the element:
            double fraction_elem = 0.0;
            if (bxmat_.is_composed_by_number_of_atoms()) {
              fraction_elem = centry.get_number_of_atoms();
            } else if (bxmat_.is_composed_by_fraction_mass()) {
              fraction_elem = centry.get_weight() / bxelem.get_molar_mass();
            }
            // std::cerr << "DEVEL: mcnp::material_store::build_material_from_bxmaterials:      Z = [" << z << "]" << std::endl;
            const materials::isotope_weight_map_type & isocomp = bxelem.get_composition();
            std::cerr << "DEVEL: mcnp::material_store::build_material_from_bxmaterials:      composition = [" << isocomp.size() << "]" << std::endl;
            if (isocomp.size()) {
              for (materials::isotope_weight_map_type::const_iterator iiso = isocomp.begin();
                   iiso != isocomp.end();
                   iiso++) {
                const materials::iso_entry isoe = iiso->second;
                const materials::isotope & bxiso = isoe.get_isotope();
                std::cerr << "DEVEL: mcnp::material_store::build_material_from_bxmaterials: bxiso = '" << bxiso.get_name() << "'" << std::endl;
                double iso_weight = isoe.get_weight();
                int ziso = bxiso.get_z();
                int aiso = bxiso.get_a();
                int za = ziso * 1000 + aiso;
                if (iso_fractions.find(za) == iso_fractions.end()) {
                  iso_fractions[za] = 0.0;
                }
                std::map<int, double>::iterator found = iso_fractions.find(za);
                double & iso_fraction = found->second;
                std::cerr << "DEVEL: mcnp::material_store::build_material_from_bxmaterials:     Z = [" << ziso << "]" << std::endl;
                std::cerr << "DEVEL: mcnp::material_store::build_material_from_bxmaterials:     A = [" << aiso << "]" << std::endl;
                iso_fraction = fraction_elem * iso_weight;
              }
            } else {
              // Force natural element:
              int zelem = bxelem.get_z();
              int za = zelem * 1000;
              if (iso_fractions.find(za) == iso_fractions.end()) {
                iso_fractions[za] = 0.0;
              }
              std::map<int, double>::iterator found = iso_fractions.find(za);
              double & iso_fraction = found->second;
              iso_fraction = fraction_elem / bxelem.get_molar_mass();
            }

          }
          // Build the resulting material for MCNP:
          mat_.set_name(bxmat_.get_name());
          for (std::map<int, double>::const_iterator iso_iter = iso_fractions.begin();
               iso_iter != iso_fractions.end();
               iso_iter++) {
            int za = iso_iter->first;
            int z = za / 1000;
            int a = za % 1000;
            double fraction = iso_iter->second;
            mat_.add_nuclide(z, a, fraction, nuclide_entry::FRACTION_NATOMS);
          }
        }
        catch (std::exception & error) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
          mat_.reset();
          return false;
        }
        return true;
      }

    } // namespace material

  } // namespace mcnp

} // namespace mctools
