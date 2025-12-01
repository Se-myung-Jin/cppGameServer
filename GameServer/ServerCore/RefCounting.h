#pragma once
class RefCounting
{
public:
	RefCounting() : _refCount(1) { }
	virtual ~RefCounting() { }

	int32 GetRefCount() { return _refCount; }

	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		if (refCount == 0)
		{
			delete this;
		}
		return refCount;
	}

protected:
	atomic<int32> _refCount;
};

template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() { }
	TSharedPtr(T* ptr) { Set(ptr); }

	// copy
	TSharedPtr(const TSharedPtr& clone) { Set(clone._ptr); }
	// move
	TSharedPtr(TSharedPtr&& move) { _ptr = move._ptr; move._ptr = nullptr; }
	// inherit
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& copy) { Set(static_cast<T*>(copy._ptr)); }

	~TSharedPtr() { Release(); }

public:
	TSharedPtr& operator=(const TSharedPtr& source)
	{
		if (_ptr != source._ptr)
		{
			Release();
			Set(source._ptr);
		}
		return *this;
	}

	TSharedPtr& operator=(TSharedPtr&& source)
	{
		Release();
		_ptr = source._ptr;
		source._ptr = nullptr;
		return *this;
	}

	bool		operator==(const TSharedPtr& comp) const { return _ptr == comp._ptr; }
	bool		operator==(T* ptr) const { return _ptr == ptr; }
	bool		operator!=(const TSharedPtr& comp) const { return _ptr != comp._ptr; }
	bool		operator!=(T* ptr) const { return _ptr != ptr; }
	bool		operator<(const TSharedPtr& source) const { return _ptr < source._ptr; }
	T*			operator*() { return _ptr; }
	const T*	operator*() const { return _ptr; }
				operator T* () const { return _ptr; }
	T*			operator->() { return _ptr; }
	const T*	operator->() const { return _ptr; }

	bool IsNull() { return _ptr == nullptr; }

private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
			ptr->AddRef();
	}

	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}

private:
	T* _ptr = nullptr;
};