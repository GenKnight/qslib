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
		Buffer���ڻ�������

		Buffer��ģ�������һ��Formatter�����ڽ����ܵ�����ת�����û���Ҫ���Զ�������
		Buffer������ڴ�
		Buffer������������ڶ��п���һ�ݣ���ָ�����shared_ptr���й���
		��Bufferά���Ķ�����ָ�룬�����ָ��ָ��λ�õ����ݽ��п�������ֻ�ǿ���ָ�뱾���ֵ
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
