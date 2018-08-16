
#include "CtpMdStream.h"
#include "FileOutputMdStream.h"
#include "ValueStorage.h"
#include "lockable.h"
#include "Buffer.h"
#include "Configuration.h"
#include "logger.h"

#include <iostream>
#include <cstdio>

#include <thread>
#include <condition_variable>
#include <future>
#include <chrono>
#include <memory>


using namespace std;


int main()
{
	auto logger = qs::LoggerFactory::createLogger("file:/FileLogger.log");
	qs::IDataStream* ctp = new qs::CtpMdStream<>("login_info.txt", logger);
	ctp->open();

	qs::IDataStream* out = new qs::FileOutputMdStream<>("data.csv", logger);
	ctp->pipe(out);
	out->open();

	ctp->join();

	system("pause");
}
