#ifndef __utils_hpp__
#define __utils_hpp__ 1

#include <string>
#include <sstream>
#include <boost/cstdint.hpp>

/**************************************************************/
template <typename Float>
std::string to_bin_format (Float value_)
{
  std::ostringstream out;
  if (sizeof (Float) == 4)
    {
      void * address = static_cast<void *>(&value_);
      uint32_t * bits = static_cast<uint32_t *>(address);
      for (int i = sizeof (Float) * 8 - 1; i >= 0; i--)
        {
          if ((*bits >> i) & 0x1) out << '1';
          else out << '0';
        }
    }
  if (sizeof (Float) == 8)
    {
      void * address = static_cast<void *>(&value_);
      uint64_t * bits = static_cast<uint64_t *>(address);
      for (int i = sizeof (Float) * 8 - 1; i >= 0; i--)
        {
          if ((*bits >> i) & 0x1) out << '1';
          else out << '0';
        }
    }
  return out.str ();
}

#endif // __utils_hpp__
