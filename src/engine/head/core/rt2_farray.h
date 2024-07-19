
//////////////////////////////////////////////////////////////////////////
//
//   FileName : rtg_buffer_allocator.h
//     Author : zxy
// CreateTime : 2010.04.01
//       Desc :
//
//////////////////////////////////////////////////////////////////////////

#ifndef _Rt2_FArray_H_
#define _Rt2_FArray_H_

namespace rt2_core {

    template<typename _Type>
    struct farray
    {
        farray()
            : pbuffer(0),
             _size(0)
        {
        }

        farray(const farray& _ary)
            : pbuffer(0),
            _size(0)
        {
            alloc(_ary.size());
            memcpy(pbuffer, _ary.pointer(), buffer_size());
        }

        ~farray()
        {
            clear();
        }

        bool alloc(size_t _siz)
        {
            clear();
            pbuffer = (_Type*)malloc(sizeof(_Type) * _siz);
            _size = _siz;
            return !!pbuffer;
        }

        bool resize(size_t _siz)
        {
            if (!pbuffer)
                return alloc(_siz);
            pbuffer = (_Type*)realloc(pbuffer, sizeof(_Type) * (_size = _siz));
            return true;
        }

        void clear()
        {
            if (pbuffer)
                free(pbuffer);
            pbuffer = 0;
            _size = 0;
        }

        void swap_erase(size_t _index)  
        {
            RTASSERT(_index < _size);
            pbuffer[_index] = pbuffer[_size - 1];
            --_size;
        }

        size_t empty() const
        {
            return _size == 0;
        }

        void merge(const farray<_Type>& _ary)
        {
            const size_t _tail = size();
            resize(_tail + _ary.size());
            memcpy(pbuffer + _tail, _ary.pointer(), _ary.buffer_size());
            return;
        }

        void operator= (const farray& _ary)
        {
            alloc(_ary.size());
            memcpy(pbuffer, _ary.pointer(), buffer_size());
        }

        template<typename _ctype>
        void operator= (const _ctype& _cont)
        {
            alloc(_cont.size());
            copy(_cont.begin(), _cont.end(), pointer());
        }

        void swap(farray& _ary)
        {
            std::swap(pbuffer, _ary.pbuffer);
            std::swap(_size, _ary._size);
        }

        size_t size() const
        {
            return _size;
        }

        size_t buffer_size() const
        {
            return _size * sizeof(_Type);
        }

        _Type& operator[] (size_t _idx)
        {
            RTASSERT(_idx < _size);
            return pbuffer[_idx];
        }

        const _Type* pointer() const
        {
            return pbuffer;
        }

        _Type* pointer()
        {
            return pbuffer;
        }

        _Type& front()
        {
            return pbuffer[0];
        }

        _Type& back()
        {
            RTASSERT(_size > 0);
            return pbuffer[_size - 1];
        }

        template<typename _Pred>
        void sort(size_t _num)
        {
            std::sort(pbuffer, pbuffer + _num, _Pred());
        }

		// Added by Wayne Wong 2010-11-08
		void push_back(const _Type& val)
		{
			resize(_size + 1);
			back() = val;
		}

    private :
        _Type* pbuffer;
        size_t _size;
    };

}

#endif
