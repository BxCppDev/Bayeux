// materials_utils.cc

// Ourselves:
#include <geomtools/materials_utils.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/materials:
#include <materials/manager.h>

// This project:
#include <geomtools/gdml_writer.h>

namespace geomtools {

  void export_gdml(const materials::manager & mat_mgr_,
                   geomtools::gdml_writer & gw_)
  {
    // Export isotopes:
    for (materials::isotope_dict_type::const_iterator i = mat_mgr_.get_isotopes().begin();
         i != mat_mgr_.get_isotopes().end();
         i++) {
      const materials::isotope & iso = i->second.get_ref();
      gw_.add_isotope(i->first,
                      iso.get_z(),
                      iso.get_a(),
                      iso.get_atomic_mass()); // in [g/mol];

    } // end of loop on isotopes

    // Export elements:
    for (materials::element_dict_type::const_iterator i = mat_mgr_.get_elements().begin();
         i != mat_mgr_.get_elements().end();
         i++) {
      const materials::element & elmt = i->second.get_ref();
      if (elmt.is_built_by_isotopic_composition()) {
        std::map<std::string, double> fractions;
        for (materials::isotope_weight_map_type::const_iterator j
               = elmt.get_composition().begin();
             j != elmt.get_composition().end();
             j++) {
          fractions[j->first] = j->second.get_weight();
        }
        gw_.add_element(i->first, fractions);
      } else {
        gw_.add_element(i->first,
                        elmt.get_z(),
                        elmt.get_symbol(),
                        elmt.get_molar_mass());
      }
    } // end of loop on elements

    // Export materials:
    for (std::list<std::string>::const_iterator i =
           mat_mgr_.get_ordered_materials().begin();
         i != mat_mgr_.get_ordered_materials().end();
         i++) {
      const std::string & material_name = *i;
      materials::material_dict_type::const_iterator found
        = mat_mgr_.get_materials().find(material_name);
      DT_THROW_IF (found == mat_mgr_.get_materials().end(),
                   std::logic_error,
                   "Missing property 'material' for a material alias !");
      const materials::material & a_mat = found->second.get_ref();

      // Composite by mean Z and A:
      if (a_mat.is_composed_by_mean_z_a()) {
        gw_.add_material(material_name,
                         a_mat.get_mean_z(),
                         a_mat.get_density(),
                         a_mat.get_mean_a());
      }

      // Composite by number of atoms:
      if (a_mat.is_composed_by_number_of_atoms()) {
        const materials::composition_map_type & comp = a_mat.get_composition();
        std::map<std::string, size_t> natoms_map;
        for (materials::composition_map_type::const_iterator jcomp = comp.begin();
             jcomp != comp.end();
             jcomp++) {
          const materials::compound_entry & ce = jcomp->second;
          std::string elmt_name = ce.get_element().get_name();
          size_t elmt_nb_of_atoms =(size_t) ce.get_number_of_atoms();
          natoms_map[elmt_name] = elmt_nb_of_atoms;
        }
        std::string formula = material_name;
        if (a_mat.get_properties().has_key("formula")) {
          formula = a_mat.get_properties().fetch_string("formula");
        }
        gw_.add_material(material_name,
                         formula,
                         a_mat.get_density(),
                         natoms_map);
      }

      // Composite by fraction mass:
      if (a_mat.is_composed_by_fraction_mass()) {
        const materials::composition_map_type & comp = a_mat.get_composition();
        std::map<std::string, double> fractions_map;
        for (materials::composition_map_type::const_iterator jcomp = comp.begin();
             jcomp != comp.end();
             jcomp++) {
          const materials::compound_entry & ce = jcomp->second;
          std::string compound_name;
          if (ce.has_element()) compound_name = ce.get_element().get_name();
          else if (ce.has_material()) compound_name = ce.get_material().get_name();
          double compound_fraction = ce.get_weight();
          fractions_map[compound_name] = compound_fraction;
        }
        std::string formula = material_name;
        if (a_mat.get_properties().has_key("formula")) {
          formula = a_mat.get_properties().fetch_string("formula");
        }
        gw_.add_material(material_name,
                         formula,
                         a_mat.get_density(),
                         fractions_map);
      }
    } // end of loop on materials

    return;
  }

  void export_gdml(const materials::manager & mat_mgr_, std::ostream & out_)
  {
    geomtools::gdml_writer GW;
    export_gdml(mat_mgr_, GW);
    out_ << GW.get_stream(geomtools::gdml_writer::materials_section()).str();
    return;
  }

} // end of namespace geomtools
