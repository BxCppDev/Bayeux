/* brio_record.cc
 */
// Ourselves
#include <brio/detail/brio_record.h>

ClassImp(brio_record)
 
void brio_record::reset() {
  fDataBuffer.Clear();
}
  
brio_record::brio_record() {
  fSerialTag = "";
  fVersionTag = 0;
}
  
brio_record::~brio_record() {
  this->reset();
}

// end of brio_record.cc
