#ifndef _QSLIB_LOGGER_H_
#define _QSLIB_LOGGER_H_

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>

#include "lockable.h"

namespace qs
{
	class ILogger
	{
	public:
		virtual ~ILogger() {}
		virtual void log(const std::string& msg) = 0;
	protected:
		std::string now()
		{
			static char buf[64];
			std::time_t t;
			time(&t);
			std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
			return buf;
		}
	};

	class ConsoleLogger :public ILogger
	{
	public:
		std::ostream& operator<<(const std::string& msg)
		{
			std::cout << msg;
		}

		virtual void log(const std::string& msg)
		{
			std::cout << "[" << now() << "] " << msg << std::endl;
		}
	};

	template<class ThreadModel = ObjectLevelLockable>
	class FileLogger :public ThreadModel, public ILogger
	{
	public:
		FileLogger(const std::string& logpath) :os(logpath, std::ios::app)
		{
			os << endl;
			log("Log started.");
		}

		std::ostream& operator<<(const std::string& msg)
		{
			os << msg;
		}

		virtual void log(const std::string& msg)
		{
			os << "[" << now() << "] " << msg << endl;
		}

		void flush()
		{
			os.flush();
		}

		void close()
		{
			os.close();
		}	

	private:
		std::ofstream os;
	};


	class LoggerFactory
	{
	public:
		static std::shared_ptr<ILogger> createLogger(const std::string& name)
		{
			if(name.substr(0, 4) == "file")
			{
				return std::shared_ptr<ILogger>(new FileLogger<>(name.substr(6)));
			}
			else if(name == "console")
			{
				return std::shared_ptr<ILogger>(new ConsoleLogger);
			}
			else
				return nullptr;
		}
	};
}

#endif
