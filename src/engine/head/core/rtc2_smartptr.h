/**
* @file rtc2_smartptr.h ÷«ƒ‹÷∏’Î
*/

#ifndef SMARTPTR_H
#define SMARTPTR_H
///////////////////////////////////////////////////////
//
//  Author Sandu Turcan
//  idlsoft@hotmail.com
//
namespace rt2_core {

template <class T> class SmartPtr;
/////////////////////////////////////////
// IRefCount
// is an interface for reference counting
// Classes can implement it themselves,
// or SmartPtr will provide its internal implementation of IRefCount
template <class T> class IRefCount {
    friend class SmartPtr<T>;
    protected:
    virtual void __IncRefCount() = 0;
    virtual void __DecRefCount() = 0;
    virtual T * GetPtr() const = 0;
};
//============================================================
/////////////////////////////////////////
// IRefCountImpl
// is a standart implementation of IRefCount
// To use it just derive your class from it:
// class CMyObject : public IRefCountImpl<CMyObject> { ... };
// Reminder: implementing IRefCount is optional but it would reduce
// memory fragmentation. 
template <class T> class IRefCountImpl : public IRefCount<T> {
    private:
    int __m_counter;
    protected:
    virtual void __IncRefCount() 
    {
        __m_counter++; 
    }
    virtual void __DecRefCount()
    {
        __m_counter--;
        if(__m_counter<=0)
        {
            __DestroyRef();
        }
    }
    virtual T * GetPtr() const
    {
        return ((T *)this);
    }
    virtual void __DestroyRef() 
    { 
        if(GetPtr()!=NULL)
            delete GetPtr();
    }
    protected:
    IRefCountImpl()
    {
        __m_counter = 0;
    }
};

//============================================================

/////////////////////////////////////////
// SmartPtr
// Usage:
// ----------------------
// 1. In a program block
// ----------------------
// SmartPtr<CMyObject> ptr1(new CMyObject); // creates object 1
// SmartPtr<CMyObject> ptr2(new CMyObject); // creates object 2
// ptr1 = ptr2;             // destroys object 1
// ptr2 = NULL;
// ptr1 = new CMyObject;    // creates object 3, destroys object 2
// ptr1->methodcall(...);
// CMyObject o1;
// // ptr1 = &o1;  // DON'T ! only memory allocated by new operator should be used
// CMyObject *o2 = new CMyObject;
// ptr1 = o2;
// //ptr2 = o2;  // DON'T ! unless CMyObject implements IRefCount
//               // try to use ptr1 = ptr2 instead, it's always safe;
// ----------------------
// 2. in a function call
// ----------------------
// void f(CMyObject *o) {...}
// ...
// SmartPtr<CMyObject> ptr(new CMyObject)
// f(ptr);
// ----------------------
// 3. As a return value
// ----------------------
// SmartPtr<CMyObject> f()
// {
//      SmartPtr<CMyObject> ptr(new CMyObject);
//      return ptr;
// }
template <class T> class SmartPtr {
private:
    IRefCount<T> *__m_refcount;
    
    /////////////////////////////////////////
    // __RefCounter
    // An internal implementation of IRefCount
    // for classes that don't implement it
    // SmartPtr will automatically choose between its internal and 
    // class' implementation of IRefCount
    class __RefCounter : public IRefCountImpl<T> {
        private:
        T *__m_ptr;
        protected:
        virtual T * GetPtr() const
        {
            return __m_ptr;
        }
        virtual void __DestroyRef() {delete this;}
        public:
        __RefCounter(T *ptr)
        {
            __m_ptr = ptr;
        }
        virtual ~__RefCounter()
        {
            IRefCountImpl<T>::__DestroyRef();
        }
    };
    // this method is called if T does not implement IRefCount
    void __Assign(void *ptr)
    {
        if(ptr==NULL)
            __Assign((IRefCount<T> *)NULL);
        else
        {
            __Assign(new __RefCounter(static_cast<T *>(ptr)));
        }
    }
    // this method is picked over __Assign(void *ptr)
    // if T implements IRefCount.
    // This allows some memory usage optimization
    void __Assign(IRefCount<T> *refcount)
    {
        if(refcount!=NULL) refcount->__IncRefCount();
        IRefCount<T> *oldref = __m_refcount;
        __m_refcount = refcount;
        if(oldref!=NULL) oldref->__DecRefCount();
    }
public:
    SmartPtr()
    {
        __m_refcount = NULL;
    }
    SmartPtr(T * ptr)
    {
        __m_refcount = NULL;
        __Assign(ptr);
    }
    SmartPtr(const SmartPtr &sp)
    {
        __m_refcount = NULL;
        __Assign(sp.__m_refcount);
    }
    virtual ~SmartPtr()
    {
        __Assign((IRefCount<T> *)NULL);
    }

    // get the contained pointer, not really needed but...
    T *GetPtr() const
    {
        if(__m_refcount==NULL) return NULL;
        return __m_refcount->GetPtr();
    }

    // assign another smart pointer
    SmartPtr & operator = (const SmartPtr &sp) {__Assign(sp.__m_refcount); return *this;}
    // assign pointer or NULL
    SmartPtr & operator = (T * ptr) {__Assign(ptr); return *this;}
    // to access members of T
    T * operator ->()
    {
#ifdef _ASSERT
        _ASSERT(GetPtr()!=NULL);
#endif
        return GetPtr();
    }
    // conversion to T* (for function calls)
    operator T* () const
    {
        return GetPtr();
    }

    // utilities
    bool operator !()
    {
        return GetPtr()==NULL;
    }
    bool operator ==(const SmartPtr &sp)
    {
        return GetPtr()==sp.GetPtr();
    }
    bool operator !=(const SmartPtr &sp)
    {
        return GetPtr()!=sp.GetPtr();
    }
};

} // namespace rt2_core
#endif
