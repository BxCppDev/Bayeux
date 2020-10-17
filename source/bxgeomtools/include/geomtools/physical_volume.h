/// \file geomtools/physical_volume.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-08
 * Last modified: 2010-02-08
 *
 * License:
 *
 * Description:
 *   Physical geometry volume
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_PHYSICAL_VOLUME_H
#define GEOMTOOLS_PHYSICAL_VOLUME_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/logger.h>

namespace geomtools {

  // Forward class declarations:
  class i_placement;
  class logical_volume;

  /// \brief A physical geometry volume (ala GDML)
  class physical_volume : public datatools::i_tree_dumpable
  {

  public:

    bool is_locked() const;

    void lock();

    void unlock();

    const std::string & get_name() const;

    void set_name(const std::string &);

    const datatools::properties & get_parameters() const;

    datatools::properties & grab_parameters();

    bool has_placement() const;

    void set_placement(const i_placement &);

    void set_placement(const i_placement *);

    const i_placement & get_placement() const;

    bool has_logical() const;

    void set_logical(const logical_volume &);

    void set_logical(const logical_volume *);

    const logical_volume & get_logical() const;

    bool has_mother() const;

    void set_mother(const logical_volume &, const std::string & name_ = "");

    const logical_volume & get_mother() const;

    bool has_real_mother() const;

    void set_real_mother(const logical_volume &);

    const logical_volume & get_real_mother() const;

    physical_volume(datatools::logger::priority = datatools::logger::PRIO_FATAL);

    physical_volume(const std::string & name_,
                    datatools::logger::priority = datatools::logger::PRIO_FATAL);

    physical_volume(const std::string & name_,
                    const logical_volume & logical_,
                    const logical_volume & mother_,
                    const i_placement    & placement_,
                    datatools::logger::priority = datatools::logger::PRIO_FATAL);

    physical_volume(const std::string & name_,
                    const logical_volume * logical_,
                    const logical_volume & mother_,
                    const i_placement    & placement_,
                    datatools::logger::priority = datatools::logger::PRIO_FATAL);

    physical_volume(const std::string & name_,
                    const logical_volume & logical_,
                    const logical_volume & mother_,
                    const i_placement    * placement_,
                    datatools::logger::priority = datatools::logger::PRIO_FATAL);

    physical_volume(const std::string & name_,
                    const logical_volume * logical_,
                    const logical_volume & mother_,
                    const i_placement    * placement_,
                    datatools::logger::priority = datatools::logger::PRIO_FATAL);

    ~physical_volume() override;

    void set_logging_priority(datatools::logger::priority);

    datatools::logger::priority get_logging_priority() const;

    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;
  private:

    void _init_defaults_();

    void _clear_logical_();

    void _clear_placement_();

  private:

    std::string            _name_;
    datatools::logger::priority _logging_priority_;
    bool                   _locked_;
    datatools::properties  _parameters_;
    bool                   _own_placement_;
    const i_placement *    _placement_;
    bool                   _own_logical_;
    const logical_volume * _logical_;
    const logical_volume * _mother_;
    const logical_volume * _real_mother_;

  public:

    typedef std::map<std::string, const physical_volume *> dict_type;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_PHYSICAL_VOLUME_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
