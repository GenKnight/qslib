#ifndef _QSLIB_FILEINPUTMDSTREAM_H_
#define _QSLIB_FILEINPUTMDSTREAM_H_

#include "ThostFtdcMdApi.h"
#include "IDataStream.h"
#include "Buffer.h"

namespace qs
{
	struct CtpCsvInputFormatter
	{
		using From = std::string;
		using To = CThostFtdcDepthMarketDataField;

		To* operator()(To* dst, const From* src)
		{
			//			
		}
	};

	template<class Formatter, class Logger>
	class FileInputMdStream :public IDataStream
	{
	public:
		using InputType = const char*;
		using OutputType = typename Formatter::To;

		FileInputMdStream(const std::string& filepath, std::shared_ptr<Logger> logger);

		virtual void open() {}
		virtual void close() {}
		virtual void pipe(IDataStream* next) {}

	private:
		Buffer<Formatter> m_buffer;
		std::shared_ptr<Logger> m_pLogger;
	};

	template<class Formatter, class Logger>
	FileInputMdStream<Formatter, Logger>::FileInputMdStream(const std::string& filepath, std::shared_ptr<Logger> logger)
	{
		
	}

}

#endif
