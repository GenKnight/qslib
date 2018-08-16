#ifndef _QSLIB_CONSTRAINTS_H_
#define _QSLIB_CONSTRAINTS_H_

namespace qs
{

	class Noncopyable
	{
	public:
		Noncopyable() {}
		~Noncopyable() {}
	private:
		Noncopyable(const Noncopyable&) = delete;
		Noncopyable& operator=(const Noncopyable&) = delete;
	};

	class Nonmovable
	{
	public:
		Nonmovable() {}
		~Nonmovable() {}
	private:
		Nonmovable(Nonmovable&&) = delete;
	};
}

#endif
