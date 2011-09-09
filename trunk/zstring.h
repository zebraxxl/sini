#ifndef __ZSTRING_H__
#define __ZSTRING_H__

#include "zalgoritms.h"

template <class T>
class CZString
{
private:
	T *Data;
	unsigned int Length;
	unsigned int Allocated;

	inline CZString( T *Ptr, unsigned int Len, unsigned int Alloc )
	{
		Data = Ptr;
		Length = Len;
		Allocated = Alloc;
	}

	inline void Resize( unsigned int NewLength )
	{
		if (Allocated < NewLength )
		{
			T *NewData = new T[NewLength];
			za_memcpy<T>( NewData, Data, Length+1 );
			delete [] Data;
			Data = NewData;
			Allocated = NewLength;
		}
	}

public:
	static const unsigned int Nop = 0xffffffff;

	inline unsigned int get_length()const{return Length;}
	inline unsigned int get_allocated()const{return Allocated;}
	inline const T*		get_ptr()const{return Data;}

	inline CZString()
	{
		Data = new T[16];
		za_memset<T>( Data, 0, 16 );
		Length = 0;
		Allocated = 16;
	}

	inline CZString( const T *Ptr )
	{
		Length = za_strlen( Ptr );
		Allocated = za_max( Length+1, 16 );
		Data = new T[Allocated];
		za_strcpy<T>( Data, Ptr );
	}

	inline CZString( const CZString &Other )
	{
		Length = Other.get_length();
		Allocated = Other.get_allocated();
		Data = new T[Allocated];
		za_memcpy<T>( Data, Other.get_ptr(), Length+1 );
	}

	inline CZString( const T *Ptr, unsigned int Len )
	{
		Length = Len;
		Allocated = za_max( Len+1, 16 );
		Data = new T[Allocated];
		za_memcpy<T>( Data, Ptr, Len );
		Data[Len] = 0;
	}

	inline ~CZString()
	{
		delete [] Data;
	}

	inline const	T&operator[](unsigned int i)const	{return Data[i];}
	inline			T&operator[](unsigned int i)		{return Data[i];}
	inline const	T& At( unsigned int i )const		{return Data[i];}
	inline			T& At( unsigned int i )				{return Data[i];}

	inline CZString &operator=(const CZString &Other )
	{
		delete [] Data;
		Allocated = Other.get_allocated();
		Data = new T[Allocated];
		Length = Other.get_length();
		za_memcpy<T>( Data, Other.get_ptr(), Length );
		Data[Length] = 0;
		return *this;
	}

	inline CZString operator+( const CZString &Other )const
	{
		unsigned int NewLen = Length + Other.Length;
		unsigned int NewAllocated = za_max( NewLen+1, 16 );
		T *NewData = new T[NewAllocated];
		za_memcpy<T>( NewData, Data, Length );
		za_memcpy<T>( &NewData[Length], Other.Data, Other.Length );
		NewData[NewLen] = 0;
		return CZString( NewData, NewLen, NewAllocated );
	}

	inline CZString operator+( const T *Other )const
	{
		unsigned int OtherLength = za_strlen( Other );
		unsigned int NewLen = Length + OtherLength;
		unsigned int NewAllocated = za_max( NewLen+1, 16 );
		T *NewData = new T[NewAllocated];
		za_memcpy<T>( NewData, Data, Length );
		za_memcpy<T>( &NewData[Length], Other, OtherLength );
		NewData[NewLen] = 0;
		return CZString( NewData, NewLen, NewAllocated );
	}

	inline CZString &operator+=( const CZString &Other )
	{
		unsigned int NewLen = Length + Other.Length;
		Resize( NewLen+1 );
		za_memcpy( &Data[Length], Other.Data, Other.Length+1 );
		Length = NewLen;
		return *this;
	}

	inline CZString &operator+=( const T *Other )
	{
		unsigned int OtherLength = za_strlen<T>( Other );
		unsigned int NewLen = Length + OtherLength;
		Resize( NewLen+1 );
		za_memcpy( &Data[Length], Other, OtherLength+1 );
		Length = NewLen;
		return *this;
	}

	inline bool operator==(const CZString& Other)const
	{
		if (Length != Other.Length)
			return false;
		for (unsigned int i = 0; i < Length; i++)
			if (Data[i] != Other.Data[i])
				return false;
		return true;
	}

	inline bool operator!=(const CZString& Other)const
	{
		if (Length != Other.Length)
			return true;
		for (unsigned int i = 0; i < Length; i++)
			if (Data[i] != Other.Data[i])
				return true;
		return false;
	}

	inline unsigned int IndexOf( T What, unsigned int Start = 0, unsigned int End = 0xffffffff )const
	{
		if (End > Length)
			End = Length;
		for (unsigned int i = Start; i < End; i++)
			if (What == Data[i])
				return i;
		return Nop;
	}

	inline unsigned int LastIndexOf( T What, unsigned int Start = 0, unsigned int End = 0xffffffff )const
	{
		if (End > Length)
			End = Length;
		for (unsigned int i = End; i >= Start; i--)
		{
			if (What == Data[i])
				return i;
			if (i == 0)
				break;
		}
		return Nop;
	}

	inline CZString SubStr( unsigned int Start, unsigned int Count = 0xffffffff )const
	{
		if (Count > Length - Start)
			Count = Length - Start;
		return CZString( &Data[Start], Count );
	}

	inline void Insert( const CZString &What, unsigned int Where )
	{
		CZString End = SubStr( Where );
		Length = Where;
		Data[Where] = 0;
		(*this) += What + End;
	}

	inline bool IsEmpty()const
	{
		return (Data[0] == 0);
	}

};

typedef CZString<char>		CZStringA;
typedef CZString<wchar_t>	CZStringW;

#endif	//__ZSTRING_H__
