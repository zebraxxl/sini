#ifndef __ZALGORITMS__H__
#define __ZALGORITMS__H__

#define za_min(a,b) ((a < b)?(a):(b))
#define za_max(a,b) ((a > b)?(a):(b))

template<class T>
inline void za_memset( T *Data, T Value, unsigned int Count )
{
	for (unsigned int i = 0; i < Count; i++)
		Data[i] = Value;
}

template<class T>
inline void za_memcpy( T *Dst, const T *Src, unsigned int Count )
{
	for (unsigned int i = 0; i < Count; i++)
		Dst[i] = Src[i];
}

template<class T>
inline unsigned int za_strlen( const T *Ptr )
{
	const T *Begin = Ptr;
	while (*Ptr)
		Ptr++;
	return (unsigned int)(Ptr - Begin);
}

template<class T>
inline void za_strcpy( T *Dst, const T *Src )
{
	do
	{
		*Dst++ = *Src;
	}while(*Src++);
}

#endif	//__ZALGORITMS__H__
