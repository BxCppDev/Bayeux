// -*- mode: c++; -*-
#ifndef ELECTRONICS_COMPONENT_INFO_H
#define ELECTRONICS_COMPONENT_INFO_H

// Third Party
// - Bayeux/datatools
#include <datatools/i_predicate.h>
// - Bayeux/geomtools
#include <geomtools/geom_id.h>

// This project
#include <electronics/logical_component.h>

namespace electronics {

  /// \brief Information about an electronic component
  class component_info {
  public:

    /// Default constructor
    component_info();

    /// Destructor
    virtual ~component_info();

    /// Check if there is an electronic identifier (EID)
    bool has_eid() const;

    /// Set the electronic identifier (EID)
    void set_eid(const geomtools::geom_id & eid_);

    /// Get the electronic identifier (EID)
    const geomtools::geom_id & get_eid() const;

    /// Check if there is a parent EID
    bool has_parent_eid() const;

    /// Set the electronic ID of the parent component
    void set_parent_eid(const geomtools::geom_id & eid_);

    /// Get the electronic ID of the parent component
    const geomtools::geom_id & get_parent_eid() const;

    /// Check if there is a logical component
    bool has_logical() const;

    /// Set the logical component
    void set_logical(const logical_component &);

    /// Return a const reference to the logical component
    const logical_component & get_logical() const;

    /// Return validity status
    bool is_valid() const;

    /// Reset
    void reset();

    friend std::ostream & operator<< (std::ostream &, const component_info &);

  private:

    geomtools::geom_id        _eid_; //!< Electronic identifier (EID) of the component
    geomtools::geom_id        _parent_eid_; //!< Electronic identifier (EID) of the parent component
    const logical_component * _logical_; //!< Electronic component model
    //datatools::properties  _auxiliaries_; //!< Auxiliary properties

  };

  struct has_component_type_predicate : public datatools::i_predicate<component_info>
  {
  public:
    has_component_type_predicate(uint32_t component_type_) : _component_type_(component_type_) {
      return;
    }
    bool operator()(const component_info & cinfo_) const {
      return (cinfo_.get_eid().get_type() == _component_type_);
    }
  private:
    uint32_t _component_type_;
  };

} // end of namespace electronics

#endif // ELECTRONICS_COMPONENT_INFO_H
