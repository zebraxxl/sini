#ifndef __ZLIST_H__
#define __ZLIST_H__

#ifdef _MSC_VER
#  define DECL_FRIEND_CROSSCOMPILERVERSION(x) friend x
#else
#  define DECL_FRIEND_CROSSCOMPILERVERSION(x) 
#endif

template <class T>
class CZList
{
private:
	typedef struct Node{
	friend struct ListIteraror;
	DECL_FRIEND_CROSSCOMPILERVERSION(CZList);
	private:
		T val;
		Node *prev;
		Node *next;

		inline void DelThis()
		{
			if (next)
				next->DelThis();
			delete this;
		}

		inline Node( const T &v, Node *p, Node *n ):val(v),prev(p),next(n){}
	};

	Node *first;
	Node *last;
	unsigned int count;

public:
	typedef struct ListIteraror
	{
	DECL_FRIEND_CROSSCOMPILERVERSION(CZList);
	private:
		Node *curr;

		inline ListIteraror( Node *c ):curr(c){}

	public:
		inline ListIteraror():curr(0){}
		inline ListIteraror &operator=(const ListIteraror &o)
		{
			curr = o.curr;
			return *this;
		}

		inline ListIteraror &operator++()
		{
			if (curr)
				curr = curr->next;
			return *this;
		}

		inline ListIteraror &operator++(int)
		{
			if (curr)
				curr = curr->next;
			return *this;
		}

		inline ListIteraror &operator--()
		{
			if ((curr)&&(curr->prev))
				curr = curr->prev;
			return *this;
		}

		inline ListIteraror &operator--(int)
		{
			if ((curr)&&(curr->prev))
				curr = curr->prev;
			return *this;
		}

		inline bool IsNULL()const
		{
			return (curr == 0);
		}

		inline bool operator==(const ListIteraror &o)const
		{
			return (curr == o.curr);
		}

		inline bool operator!=(const ListIteraror &o)const
		{
			return (curr != o.curr);
		}

		inline T &operator->()
		{
			return curr->val;
		}

		inline T &operator*()
		{
			return curr->val;
		}
	};

	inline CZList():first(0),last(0),count(0){};
	inline ~CZList(){Clear();}

	inline ListIteraror get_begin()const{return ListIteraror( first );}
	inline ListIteraror get_end()const{return ListIteraror( last );}
	inline unsigned int get_size()const{return count;}

	inline void AddToBegin( const T &val )
	{
		Node *tmp = first;
		first = new Node( val, 0, first );
		if (tmp)
			tmp->prev = first;
		else if (!last)
			last = first;
		count++;
	}

	inline void AddToEnd( const T &val )
	{
		Node *tmp = last;
		last = new Node( val, last, 0 );
		if (tmp)
			tmp->next = last;
		else if (!first)
			first = last;
		count++;
	}

	inline void AddAfter ( ListIteraror &i, const T &val )
	{
		if (i.curr)
		{
			Node *tmp = i.curr->next;
			i.curr->next = new Node( val, i.curr, i.curr->next );
			count++;
			if (tmp)
				tmp->prev = i.curr->next;
			else
				last = i.curr->next;
		}
	}

	inline void AddBefore( ListIteraror &i, const T &val )
	{
		if (i.curr)
		{
			Node *tmp = i.curr->prev;
			i.curr->prev = new Node( val, i.curr->prev, i.curr );
			count++;
			if (tmp)
				tmp->next = i.curr->prev;
			else
				first = i.curr->prev;
			
		}
	}

	inline void Delete( const ListIteraror &i )
	{
		if (i.curr)
		{
			if (i.curr->next)
				i.curr->next->prev = i.curr->prev;
			else
				last = i.curr->prev;

			if (i.curr->prev)
				i.curr->prev->next = i.curr->next;
			else
				first = i.curr->next;
			count--;
		}
	}

	inline void Clear()
	{
		if (first)
		{
			Node *next;
			Node *curr = first;
			while (curr)
			{
				next = curr->next;
				delete curr;
				curr = next;
			}
		}
		first = 0;
		last = 0;
		count = 0;
	}
};

#endif	//__ZLIST_H__
