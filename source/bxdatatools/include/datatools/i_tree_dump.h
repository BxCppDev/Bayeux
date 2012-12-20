/* i_tree_dump.h */
#ifndef DATATOOLS_I_TREE_DUMP_H_
#define DATATOOLS_I_TREE_DUMP_H_
// Standard Library
#include <iostream>
#include <string>

// Third Party
// - A

// This Project

namespace datatools {

/*! \brief An abstract interface with utilities for printable objects.
 *
 */
class i_tree_dumpable {
 public:
  static const std::string TAG;
  static const std::string LAST_TAG;
  static const std::string SKIP_TAG;
  static const std::string LAST_SKIP_TAG;

 public:
  static std::ostream& last_skip_tag(std::ostream& out);

  static std::ostream& skip_tag(std::ostream& out);

  static std::ostream& last_tag(std::ostream& out);

  static std::ostream& tag(std::ostream& out);


  /// Main interface method for smart dump
  virtual void tree_dump (std::ostream& out = std::clog, 
                          const std::string& title  = "",
                          const std::string& indent = "",
                          bool inherit = false) const = 0;

  /// Inner class for ?
  class inherit_tag {
   public:
    inherit_tag(bool inherit); 

    friend std::ostream & operator<<(std::ostream& out, 
                                     const inherit_tag& last_tag);

   private:
     bool inherit_;
 };


  /// Inner class for ?
  class inherit_skip_tag {
   public:
    inherit_skip_tag(bool inherit); 

    friend std::ostream& operator<<(std::ostream& out, 
                                    const inherit_skip_tag& last_tag);
   private:
    bool inherit_;
  };

};

} // end of namespace datatools 

#endif // DATATOOLS_I_TREE_DUMP_H_

