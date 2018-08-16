#ifndef _QSLIB_LOCK_H_
#define _QSLIB_LOCK_H_

#include <mutex>

#include "constraints.h"

namespace qs
{
	using DefaultMutex = std::mutex;
	
	namespace internal
	{
		template<class Derive, class MutexPolicy = DefaultMutex>
		class Nonlockable
		{
		public:
			class Lock :Noncopyable, Nonmovable
			{
			public:
				Lock() {}
				Lock(const Derive*) {}
				Lock(const Derive&) {}
				~Lock() {}
			};
		};


		template<class Derive, class MutexPolicy = DefaultMutex>
		class ObjectLevelLockable
		{
		public:
			class Lock :Noncopyable, Nonmovable
			{
			public:
				explicit Lock(const Derive* derive) :m_base(derive)
				{
					m_base->_m_objectLevelMutex.lock();
				}

				explicit Lock(const Derive& derive) :m_base(reinterpret_cast<const ObjectLevelLockable*>(&derive))
				{
					m_base->_m_objectLevelMutex.lock();
				}

				~Lock()
				{
					m_base->_m_objectLevelMutex.unlock();
				}
			private:
				const ObjectLevelLockable* m_base;
			};
		private:
			friend class Lock;
			mutable MutexPolicy _m_objectLevelMutex;
		};


		template<class Derive, class MutexPolicy = DefaultMutex>
		class ClassLevelLockable
		{
		public:
			class Lock :Noncopyable, Nonmovable
			{
			public:
				Lock()
				{
					_s_classLevelMutex.lock();
				}

				explicit Lock(const ClassLevelLockable*)
				{
					_s_classLevelMutex.lock();
				}

				explicit Lock(const ClassLevelLockable&)
				{
					_s_classLevelMutex.lock();
				}

				~Lock()
				{
					_s_classLevelMutex.unlock();
				}
			};
		private:
			friend class Lock;
			static MutexPolicy _s_classLevelMutex;
		};

		template<class Derive, class MutexPolicy>
		MutexPolicy ClassLevelLockable<Derive, MutexPolicy>::_s_classLevelMutex;

	}

	class Nonlockable :public internal::Nonlockable<Nonlockable>
	{
	};

	class ObjectLevelLockable:public internal::ObjectLevelLockable<ObjectLevelLockable>
	{
	};

	class ClassLevelLockable :public internal::ClassLevelLockable<ClassLevelLockable>
	{
	};
}

#endif
