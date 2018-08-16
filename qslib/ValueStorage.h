#ifndef _QSLIB_VALUESTORAGE_
#define _QSLIB_VALUESTORAGE_

#include <memory>

namespace qs
{
	class ValueStorage
	{
	public:
		template<class T>
		ValueStorage(const T& val):m_storage(new InternalStorage<T>(val)) {}

		template<class T>
		ValueStorage(T* val) : m_storage(new InternalStorage<T*>(val)) {}

		template<class T>
		T& cast()
		{
			return static_cast<InternalStorage<T>*>(m_storage.get())->m_val;
		}

		template<class T>
		const T& cast()const
		{
			return const_cast<ValueStorage*>(this)->cast<T>();
		}
	private:
		class StorageBase {};

		template<class T>
		class InternalStorage :public StorageBase
		{
		public:
			InternalStorage(const T& val) :m_val(val) {}

			T m_val;
		};
	private:
		std::shared_ptr<StorageBase> m_storage;
	};
}

#endif
