#ifndef _QSLIB_TIMESERIESVISITOR_H_
#define _QSLIB_TIMESERIESVISITOR_H_

#include <cstddef>

namespace qs
{
	class TimeSeriesVisitor
	{
	public:

	private:
		std::ptrdiff_t index(std::ptrdiff_t i, std::ptrdiff_t current)
		{
			return current - i;
		}
	};
}

#endif
