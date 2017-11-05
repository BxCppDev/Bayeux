//brio_record.cc

// Ourselves:
#include <brio/detail/brio_record.h>

ClassImp(brio_record)

void brio_record::reset()
{
  fDataBuffer.Clear();
  return;
}

brio_record::brio_record()
{
  fSerialTag = "";
  fVersionTag = 0;
  return;
}

brio_record::~brio_record()
{
  this->reset();
  return;
}
