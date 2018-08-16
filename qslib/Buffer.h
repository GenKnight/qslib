#ifndef _QSLIB_BUFFER_H_
#define _QSLIB_BUFFER_H_

#include <vector>
#include <utility>

#include "lockable.h"

namespace qs
{
	template<class T>
	using BufferReadType = std::vector<std::shared_ptr<const T>>;

	template<class T>
	class DummyFormatter
	{
	public:
		using From = T;
		using To = T;

		To* operator()(T* dst, const T* src)
		{
			*dst = *src;
			return dst;
		}
	};

	/*
		Buffer用于缓存数据

		Buffer的模版参数是一个Formatter，用于将接受的数据转换成用户想要的自定义数据
		Buffer会分配内存
		Buffer将传入的数据在堆中拷贝一份，堆指针采用shared_ptr进行管理
		若Buffer维护的对象是指针，不会对指针指向位置的内容进行拷贝，而只是拷贝指针本身的值
	*/
	template<class Formatter>
	class Buffer :public ObjectLevelLockable
	{
	public:
		using InputType = typename Formatter::From;
		using OutputType = typename Formatter::To;

		using OutputPointer = std::shared_ptr<const OutputType>;
		using OutputVector = BufferReadType<OutputType>;

		Buffer():m_buffer()
		{
		}

		void write(const InputType* element)
		{
			Lock lock(this);
			m_buffer.push_back(OutputPointer(m_formatter(new OutputType, element)));
		}

		void write(const InputType& element)
		{
			Lock lock(this);
			m_buffer.push_back(OutputPointer(m_formatter(new OutputType, &element)));
		}

		OutputVector read()
		{
			Lock lock(this);
			OutputVector ret(m_buffer);
			m_buffer.clear();
			return ret;
		}

		std::size_t size()
		{
			m_buffer.size();
		}

		const OutputType& operator[](std::ptrdiff_t pos)const
		{
			return *(m_buffer[pos]);
		}

		void clear()
		{
			m_buffer.clear();
		}

	private:
		Formatter m_formatter;
		std::vector<OutputPointer> m_buffer;
		std::size_t m_pos;
	};
}

#endif
