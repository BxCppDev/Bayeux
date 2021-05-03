/// \file geomtools/plain_model.cc

// Ourselves:
#include <geomtools/plain_model.h>


namespace geomtools {

  std::string plain_model::get_model_id() const
  {
    return "geomtools::plain_model";
  }

  const geomtools::i_shape_3d & plain_model::get_solid () const
  {
    return *_solid_;
  }

  void plain_model::set_solid(const geomtools::i_shape_3d & solid_)
  {
    DT_THROW_IF(is_constructed(), std::logic_error, "Model is already constructed!");
    _solid_ = &solid_;
  }

  const std::string & plain_model::get_material_name () const
  {
    return _material_name_;
  }

  void plain_model::set_material_name(const std::string & material_name_)
  {
    DT_THROW_IF(is_constructed(), std::logic_error, "Model is already constructed!");
    _material_name_ = material_name_;
    return;
  }

  plain_model::plain_model(const geomtools::i_shape_3d & solid_, const std::string & material_name_)
    : i_model ()
  {
    set_solid(solid_);
    set_material_name(material_name_);
    return;
  }
   
  void plain_model::tree_dump(std::ostream & out_,
                              const std::string & title_,
                              const std::string & indent_,
                              bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);
    // Shape type ID:
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Solid shape : " << (_solid_ != nullptr ? _solid_->get_shape_name() : "no") << std::endl;
    }

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Material name : '" << get_material_name() << "'" << std::endl;
    }
    return;
  }

  void plain_model::_pre_construct (datatools::properties & /* setup_ */,
                                    models_col_type * /* models_ */)
   {
     DT_THROW_IF(_material_name_.empty(), std::logic_error, "Model has no material name!");
     DT_THROW_IF(_solid_ == nullptr, std::logic_error, "Model has no solid shape!");
     DT_THROW_IF(not _solid_->is_locked(), std::logic_error, "Solid shape is not locked!");
     
     return;
   }

  void plain_model::_at_construct (const datatools::properties & /* config_ */,
                                   models_col_type * /* models_ */)
  {

    // Set the envelope solid shape:
    grab_logical().set_name(i_model::make_logical_volume_name(get_name()));
    grab_logical().set_shape(*_solid_);
    grab_logical().set_material_ref(_material_name_);
    grab_logical().set_geometry_model(*this);
    return;
  }

  // void plain_model::_at_destroy (models_col_type * /* models_ */)
  // {
  //   return;
  // }
  
} // end of namespace geomtools

