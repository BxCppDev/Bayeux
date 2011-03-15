#pragma once

#include <string>

namespace eos {

	class quasi_portable_archive_common
	{
	public:
	  static const int restricted_portable_flag = 0;
	  static const int quasi_portable_flag      = 1;	  
	  static const int strictly_portable_flag   = 2;
	};

} // namespace eos

