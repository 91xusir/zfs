#ifndef _PTR_H
#define _PTR_H

namespace ui 
{
	//Ä£°æÖÇÄÜÖ¸Õë
	template <typename T>
	class RtwRefPtr
	{
	public:
		RtwRefPtr(T* p = 0):ptr(p), pUse(new size_t(1)){}
		RtwRefPtr(const RtwRefPtr& src): ptr(src.ptr),pUse(src.pUse){ ++*pUse; }

		RtwRefPtr& operator=(const RtwRefPtr& rhs)
		{
			++*rhs.pUse;
			decrUse();
			ptr = rhs.ptr;
			pUse = rhs.pUse;
			return *this;
		}

		T* operator= (T* p)
		{
			T* oldp = ptr;
			ptr = p;
			return oldp;
		}
		
		T* operator->()
		{
			if (ptr)
			{
				return ptr;
			}
			throw std::runtime_error("access through NULL pointer");
		}

		operator T*() const
		{
			return ptr;
		}

		const T* operator->() const
		{
			if (ptr)
			{
				return ptr;
			}
		}

		T& operator*()
		{
			if (ptr)
			{
				return *ptr;
			}
		}
		
		const T& operator*() const
		{
			if (ptr)
			{
				return *ptr;
			}
		}
		~RtwRefPtr()
		{
			decrUse();
		}
		bool IsEmpty()
		{
			if (ptr)
			{
				return false;
			}
			return true;
		}

	private:
		void decrUse()
		{
			if(--pUse == 0)
			{
				DEL_ONE(ptr);
				DEL_ONE(pUse);
			}
		}
		T* ptr;
		size_t *pUse;
	};
}
#endif