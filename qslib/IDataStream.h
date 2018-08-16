#ifndef _QSLIB_DATASTREAM_H_
#define _QSLIB_DATASTREAM_H_

#include <string>
#include <memory>

#include "ValueStorage.h"

namespace qs
{
	class IDataStream
	{
	public:
		virtual ~IDataStream() {}
		virtual void open() = 0;
		virtual void close() = 0;
		virtual void set(const std::string&, const ValueStorage&) {}
		virtual void pipe(IDataStream*) {}
		virtual void joint(std::shared_ptr<ValueStorage>) {}
		virtual void join() {}
	};
}

#endif
