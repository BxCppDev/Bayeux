/* i_propinit.h */
#ifndef DATATOOLS_UTILS_I_PROPINIT_H_
#define DATATOOLS_UTILS_I_PROPINIT_H_

namespace datatools {
namespace utils {

class properties;

class i_propinit {
 public:
  virtual void init_from (const datatools::utils::properties &) = 0;
  /*
     {
     std::cerr << "datatools/utils/properties.h: i_propinit::init_from: "
     << "default method!" << std::endl; 
     }
     */
};

} // end of namespace utils 
} // end of namespace datatools 

#endif // DATATOOLS_UTILS_I_PROPINIT_H_

