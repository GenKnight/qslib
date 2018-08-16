#ifndef _QSLIB_FILEOUTPUTMDSTREAM_H_
#define _QSLIB_FILEOUTPUTMDSTREAM_H_

#include <sstream>
#include <thread>
#include <memory>

#include "ThostFtdcMdApi.h"
#include "IDataStream.h"
#include "logger.h"
#include "sleep.h"

namespace qs
{
	template<char separator = ','>
	struct CtpTxtOutputFormatter
	{
		using From = CThostFtdcDepthMarketDataField;
		using To = std::string;

		To* operator()(To* dst, const From* src)
		{
			m_ss << src->TradingDay << s_sep
				 << src->InstrumentID << s_sep
				 << src->ExchangeID << s_sep
				 << src->ExchangeInstID << s_sep
				 << src->LastPrice << s_sep
				 << src->PreSettlementPrice << s_sep
				 << src->PreClosePrice << s_sep
				 << src->PreOpenInterest << s_sep
				 << src->OpenPrice << s_sep
				 << src->HighestPrice << s_sep
				 << src->LowestPrice << s_sep
				 << src->Volume << s_sep
				 << src->Turnover << s_sep
				 << src->OpenInterest << s_sep
				 << src->ClosePrice << s_sep
				 << src->SettlementPrice << s_sep
				 << src->UpperLimitPrice << s_sep
				 << src->LowerLimitPrice << s_sep
				 << src->PreDelta << s_sep
				 << src->CurrDelta << s_sep
				 << src->UpdateTime << s_sep
				 << src->UpdateMillisec << s_sep
				 << src->BidPrice1 << s_sep
				 << src->BidVolume1 << s_sep
				 << src->AskPrice1 << s_sep
				 << src->AskVolume1 << s_sep
				 << src->BidPrice2 << s_sep
				 << src->BidVolume2 << s_sep
				 << src->AskPrice2 << s_sep
				 << src->AskVolume2 << s_sep
				 << src->BidPrice3 << s_sep
				 << src->BidVolume3 << s_sep
				 << src->AskPrice3 << s_sep
				 << src->AskVolume3 << s_sep
				 << src->BidPrice4 << s_sep
				 << src->BidVolume4 << s_sep
				 << src->AskPrice4 << s_sep
				 << src->AskVolume4 << s_sep
				 << src->BidPrice5 << s_sep
				 << src->BidVolume5 << s_sep
				 << src->AskPrice5 << s_sep
				 << src->AskVolume5 << s_sep
				 << src->AveragePrice << s_sep
				 << src->ActionDay << std::endl;
			*dst = m_ss.str();
			m_ss.clear();
			return dst;
		}

	private:
		static const char s_sep = separator;
		std::ostringstream m_ss;
	};

	class CtpBinOutputFormatter
	{
	public:
		
	};

	template<class Formatter = CtpTxtOutputFormatter<>, class Logger = ILogger>
	class FileOutputMdStream :public IDataStream, public ObjectLevelLockable
	{
	public:
		using InputType = typename Formatter::From;
		using OutputType = const char*;

		FileOutputMdStream(const std::string& filepath, std::shared_ptr<ILogger> logger) :
			m_daemon(std::mem_fn(&FileOutputMdStream::writeData), this), m_pLogger(logger)
		{
			m_os.open(filepath, std::ios::app);
			m_pLogger->log("[FOS] File output stream was created. Data would be saved in "s + filepath);
		}

		virtual void open()
		{
			m_isOpen = true;
		}

		virtual void close() 
		{
			m_isOpen = false;
			m_os.close();
		}

		virtual void joint(std::shared_ptr<ValueStorage> data)
		{
			auto ds = data->cast<BufferReadType<InputType>>();
			for (const auto& p : ds)
			{
				m_buffer.write(p.get());
			}
		}

	private:
		void writeData()
		{
			while (!m_isOpen) { sleep(1000); }
			m_pLogger->log("[FOS] Started to receive and save data in .");
			while (m_isOpen)
			{
				sleep(2000);
				auto dataset = m_buffer.read();
				for (const auto& data : dataset)
				{
					m_os << *data;
					printf("%s", data->c_str());
				}
				m_os.flush();
			}
		}

	private:
		std::fstream m_os;
		std::thread m_daemon;
		bool m_isOpen = false;
		Buffer<Formatter> m_buffer;
		std::shared_ptr<ILogger> m_pLogger;
	};
}

#endif
