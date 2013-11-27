// -*- mode: c++; -*-
#ifndef ELECTRONICS_COMPONENT_INFO_H
#define ELECTRONICS_COMPONENT_INFO_H

// Third Party
// - Bayeux/datatools
#include <datatools/i_predicate.h>
// - Bayeux/geomtools
#include <geomtools/geom_id.h>

// This project
#include <electronics/component_model_base.h>

namespace electronics {

  /// \brief Information about a electronics components
  class component_info {
  public:

    /// Default constructor
    component_info();

    /// Destructor
    virtual ~component_info();

    /// Check if there is an electronic identifier (EID)
    bool has_eid () const;

    /// Set the electronic identifier (EID)
    void set_eid(const geomtools::geom_id & eid_);

    /// Get the electronic identifier (EID)
    const geomtools::geom_id & get_eid () const;

    /// Check if there is a parent EID
    bool has_parent_eid () const;

    /// Set the electronic ID of the parent component
    void set_parent_eid(const geomtools::geom_id & eid_);

    /// Get the electronic ID of the parent component
    const geomtools::geom_id & get_parent_eid () const;

    /// Check if there is an electronic component model
    bool has_model () const;

    /// Set the component model
    void set_model(const component_model_base &);

    /// Return a const reference to the component model
    const component_model_base & get_model() const;

    /// Return validity status
    bool is_valid() const;

    /// Reset
    void reset();

    friend std::ostream & operator<< (std::ostream &, const component_info &);

  private:

    geomtools::geom_id     _eid_; //!< Electronic identifier (EID) of the component
    geomtools::geom_id     _parent_eid_; //!< Electronic identifier (EID) of the parent component
    const component_model_base * _model_; //!< Electronic component model
    //datatools::properties  _auxiliaries_; //!< Auxiliary properties

  };

  struct has_component_type_predicate : public datatools::i_predicate<component_info>
  {
    uint32_t _component_type_;
    has_component_type_predicate(uint32_t component_type_) : _component_type_(component_type_)
    {
      return;
    }
    bool operator()(const component_info & cinfo_) const
    {
      return (cinfo_.get_eid().get_type() == _component_type_);
    }
  };

} // end of namespace electronics

#endif // ELECTRONICS_COMPONENT_INFO_H
