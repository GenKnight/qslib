#ifndef _QS_IDATAWINDOW_H_
#define _QS_IDATAWINDOW_H_

#include <chrono>

#include "IDataStream.h"

namespace qs
{
	class IDataWindow :public IDataStream
	{
	public:
		void setWidth();
	};
}

#endif
