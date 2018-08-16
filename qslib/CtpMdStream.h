#ifndef _QSLIB_CTPMDSTREAM_H_
#define _QSLIB_CTPMDSTREAM_H_

#pragma comment(lib,"thostmduserapi")
#pragma comment(lib,"thosttraderapi")

#include <memory>
#include <condition_variable>
#include <utility>

#include "ThostFtdcMdApi.h"
#include "IDataStream.h"
#include "Configuration.h"
#include "Buffer.h"
#include "logger.h"

namespace qs
{
	using DefaultCtpFormatter = DummyFormatter<CThostFtdcDepthMarketDataField>;

	/* 
		@brief class Formatter is used for transferring the data type of input stream to yourself data type.

		In the protocol of CTP, the type of marker data input if CThostFtdcDepthMarketDataField.
		If you want to deal with the input and make your own output, you need to completement operator();

		An instance of class Formatter is a callable object with members below.
		@type From:	The input type 
		@type To:	The output type
		@mfunc:		To* operator(To*, const From*);
		via this function, type From is transferred to type To.
		Default CTP formatter just returns a copy of the input data.
	*/
	template<class Formatter = DefaultCtpFormatter, class Logger = ILogger>
	class CtpMdStream :public IDataStream, public CThostFtdcMdSpi
	{
	public:
		using InputType = CThostFtdcDepthMarketDataField;
		using OutputType = typename Formatter::To;

		CtpMdStream() :m_pApi(CThostFtdcMdApi::CreateFtdcMdApi())
		{
			m_pApi->RegisterSpi(this);
		}

		CtpMdStream(const std::string& configpath, std::shared_ptr<Logger> logger);

		CtpMdStream(const std::string& frontAddr, const std::string& brokerId, const std::string& userId, const std::string& password)
			: m_pApi(CThostFtdcMdApi::CreateFtdcMdApi())
		{
			m_pApi->RegisterSpi(this);
		}

		virtual ~CtpMdStream()
		{
			close();
		}

		virtual void open();

		virtual void close();

		virtual void set(const std::string&, const ValueStorage&) {}

		virtual void pipe(IDataStream* next)
		{
			m_children.push_back(next);
		}

		virtual void join()
		{
			m_pApi->Join();
		}

		virtual void OnFrontConnected()
		{
			m_isConnected = true;
			m_pLogger->log("[CTP] Successfully connected to front.");
		}

		virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
		{
			if (pRspInfo&&pRspInfo->ErrorID == 0)
			{
				m_isLogin = true;
				m_pLogger->log("[CTP] Successfully login.");
			}
			else
				m_pLogger->log("[CTP] Failed to login.");
		}

		virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

		virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	private:
		char** instrumentArray();
	private:
		CThostFtdcMdApi* m_pApi;
		std::map<std::string, std::shared_ptr<Buffer<Formatter>>> m_buffers;
		Configuration m_configuration;
		std::vector<std::string> m_instruments;
		bool m_isConnected	= false;
		bool m_isLogin = false;
		std::shared_ptr<Logger> m_pLogger;
		std::vector<IDataStream*> m_children;

		static int s_requestId;
	};

	template<class Formatter, class Logger>
	int CtpMdStream<Formatter, Logger>::s_requestId = 0;

	template<class Formatter, class Logger>
	CtpMdStream<Formatter, Logger>::CtpMdStream(const std::string& configpath, std::shared_ptr<Logger> logger) :
		m_pApi(CThostFtdcMdApi::CreateFtdcMdApi()), m_configuration(configpath), m_pLogger(logger)
	{
		m_pApi->RegisterSpi(this);
		std::size_t pos1 = 0, pos2;
		const std::string& insts = m_configuration["Instruments"];
		while ((pos2 = insts.find(',', pos1)) != std::string::npos)
		{
			m_instruments.push_back(insts.substr(pos1, pos2 - pos1));
			pos1 = pos2 + 1;
		}
		if (pos1 != insts.size())
			m_instruments.push_back(insts.substr(pos1));
	}		

	template<class Formatter, class Logger>
	char** CtpMdStream<Formatter, Logger>::instrumentArray()
	{
		char** insts = new char*[m_instruments.size()];
		for (std::size_t i = 0; i < m_instruments.size(); ++i)
		{
			insts[i] = const_cast<char*>(m_instruments[i].c_str());
		}
		return insts;
	}

	template<class Formatter, class Logger>
	void CtpMdStream<Formatter, Logger>::open()
	{
		if (!m_isConnected)
		{
			m_pApi->RegisterFront((char*)m_configuration["Address"].c_str());
			m_pApi->Init();
		}
		while (!m_isConnected) {}
		CThostFtdcReqUserLoginField loginField;
		strcpy(loginField.BrokerID, m_configuration["Broker"].c_str());
		strcpy(loginField.UserID, m_configuration["User"].c_str());
		strcpy(loginField.Password, m_configuration["Password"].c_str());
		m_pApi->ReqUserLogin(&loginField, s_requestId++);
		while (!m_isLogin) {}
		char** insts = instrumentArray();
		m_pApi->SubscribeMarketData(insts, (int)m_instruments.size());
		delete[] insts;
	}

	template<class Formatter, class Logger>
	void CtpMdStream<Formatter, Logger>::close()
	{
		if (m_pApi)
		{
			m_pApi->RegisterSpi(nullptr);
			m_pApi->Release();
			m_pApi = nullptr;
		}
		m_isConnected = false;
	}

	template<class Formatter, class Logger>
	void CtpMdStream<Formatter, Logger>::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		m_buffers.insert(std::make_pair(
			pSpecificInstrument->InstrumentID, 
			new Buffer<Formatter>
		));
		std::string msg = "";
		msg.append(pSpecificInstrument->InstrumentID);
		msg.push_back('.');
		m_pLogger->log("[CTP] Successfully subscribed the market data of "s + pSpecificInstrument->InstrumentID + ".");
	}

	template<class Formatter, class Logger>
	void CtpMdStream<Formatter, Logger>::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
	{
		m_buffers[pDepthMarketData->InstrumentID]->write(pDepthMarketData);
		auto ds = m_buffers[pDepthMarketData->InstrumentID]->read();
		auto ptr = std::shared_ptr<ValueStorage>(new ValueStorage(ds));
		for (const auto& child : m_children)
		{
			child->joint(ptr);
		}
		static int a = 1;
		printf("%d, %f\n", a++, pDepthMarketData->AskPrice1);
	}
}

#endif
