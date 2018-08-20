# qslib
量化交易系统，基于流计算框架实现
### 数据的流动和转换就好比水流过一根根的水管，一个数据流管道代表一个对数据处理的方式
### qslib暂时实现的功能
* 从CTP流入数据，即CTP数据输入流，提供默认的Formatter，亦可自定义
* 将CTP流入的数据dump到本地文件，即CTP数据的文件输出流
* 将上述流对接
### qslib将实现的功能，包括但不限于下述几点，欢迎大家提出更多的意见
* 提供交易接口，完善量化交易所需的各个功能
* 提供更多有用的内置流，方便用户编写策略
* 实现数据窗口，并在窗口的基础上提供指标的计算
* 设计一个时间模块，使得能更方便地在时间维度上对数据进行处理，强化框架的表达能力
* 性能优化，当前的数据流转过程伴有大量的数据产生及销毁，尽可能地复用同一块数据可以提高性能
* 提供多种策略编写框架，并不只局限于流式计算
* 加入机器学习模块（结合本作者的另一个符号计算库编写一个深度学习框架）
### 数据流有多个接口函数
* open、close用于开关流，当流打开后，将会接受前面的管道流入的数据且传递给下一个流，关闭后将切断与前后的关联
* pipe用于指定将当前流中的数据流入下一个流
* joint用于接受上一个流传入的数据
* set用于设置一些参数
### 数据流接受多个模版参数
* Formatter用于转换数据形式，或用于计算
* Logger用于确定使用何种日志输出方式
### egg.
```
#include "CtpMdStream.h"
#include "FileOutputMdStream.h"
#include "ValueStorage.h"
#include "lockable.h"
#include "Buffer.h"
#include "Configuration.h"
#include "logger.h"

#include <iostream>
#include <memory>

using namespace std;

int main()
{
	auto logger = qs::LoggerFactory::createLogger("file:/FileLogger.log");  // 创建一个日志记录工具
	qs::IDataStream* ctp = new qs::CtpMdStream<>("login_info.txt", logger); // 创建一个数据流，数据从CTP流入
	ctp->open();  // 打开CTP数据流

	qs::IDataStream* out = new qs::FileOutputMdStream<>("data.csv", logger);  // 创建一个文件输出流，将输入的数据保存到data.csv中
	ctp->pipe(out); // 将CTP的数据流入到文件输出流中
	out->open();  // 打开文件输出流

	ctp->join();  // 阻塞主线程
}
```
