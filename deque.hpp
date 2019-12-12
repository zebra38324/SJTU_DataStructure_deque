#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cmath>

namespace sjtu {

	template<class T>
	class deque {
	public:
		struct colNode
		{
			T *value;
			colNode *prev;
			colNode *next;
			colNode() :value(NULL), prev(NULL), next(NULL) {}
			colNode(const T *obj) :prev(NULL), next(NULL) 
			{ 
				value=new T(*obj);
			}
			colNode(const T &obj) :prev(NULL), next(NULL) 
			{ 
				value = new T(obj); 
			}
			~colNode() { if(value!=NULL)delete value; }
		};
		struct rowNode
		{
			int len;
			colNode *down;
			colNode *bot;
			rowNode *prev;
			rowNode *next;
			rowNode() :len(0), down(NULL), bot(NULL), prev(NULL), next(NULL) {}
			~rowNode(){}
		};
		class const_iterator;
		class iterator {
		public:
			rowNode * cur;
			rowNode * head;
			rowNode *tail;
			colNode * add;
			colNode *first;
			colNode *last;
		public:
			iterator() :cur(NULL), add(NULL), first(NULL), last(NULL) {}
			iterator(rowNode *c, rowNode *h, rowNode *t, colNode *a, colNode *f, colNode *l) :cur(c), head(h), tail(t), add(a), first(f), last(l) {}
			void init(iterator &rhs)
			{
				head = new rowNode;
				cur = head;
				tail = new rowNode;
				cur->next = tail;
				tail->prev = cur;
				rhs.cur = cur->next;
				rhs.head = head;
				rhs.tail = tail;
			}
			void copy(iterator &finish, const iterator &objStart, const iterator &objFinish)
			{
				iterator tmp = *this;
				iterator objTmp = objStart;
				++objTmp;
				while (objTmp != objFinish)
				{
					tmp.cur->next = new rowNode;
					tmp.cur->next->prev = tmp.cur;
					tmp.cur = tmp.cur->next;
					tmp.cur->len = objTmp.cur->len;
					tmp.cur->down = new colNode(objTmp.add->value);
					tmp.add = tmp.cur->down;
					tmp.first = tmp.add;
					while (objTmp.add->next != objTmp.last)
					{
						++objTmp;
						tmp.add->next = new colNode(objTmp.add->value);
						tmp.add->next->prev = tmp.add;
						tmp.add = tmp.add->next;
					}
					tmp.last = new colNode;
					tmp.last->prev = tmp.add;
					tmp.add->next = tmp.last;
					tmp.cur->bot = tmp.last;
					if (objTmp.cur->next == objTmp.tail)
						break;
					++objTmp;
				}
				tmp.cur->next = finish.cur;
				finish.cur->prev = tmp.cur;
			}
			void clear(iterator &finish)
			{
				iterator tmp = *this;
				++tmp;
				while (tmp != finish)
				{
					colNode *old = tmp.add;
					tmp.first = tmp.first->next;
					tmp.add = tmp.first;
					tmp.cur->down = tmp.first;
					delete old;
					tmp.cur->len--;
					if (tmp.first == tmp.last)
					{
						delete tmp.first;
						rowNode *t = tmp.cur;
						tmp.cur->prev->next = tmp.cur->next;
						tmp.cur->next->prev = tmp.cur->prev;
						tmp.cur = tmp.cur->next;
						tmp.first = tmp.cur->down;
						tmp.last = tmp.cur->bot;
						tmp.add = tmp.first;
						delete t;
					}
				}
			}
			void insert(iterator &pos, const T &v)
			{
				if (pos.head != head || pos.tail != tail)
				{
					invalid_iterator e;
					throw exception(e);
				}
				if (pos.cur == pos.tail)
				{
					if (pos.cur->prev == pos.head)
					{
						pos.head->next = new rowNode;
						pos.head->next->next = pos.tail;
						pos.head->next->prev = pos.head;
						pos.tail->prev = pos.head->next;
						rowNode *tmp = pos.head->next;
						tmp->down = new colNode(v);
						tmp->down->next = new colNode;
						tmp->down->next->prev = tmp->down;
						tmp->bot = tmp->down->next;
						tmp->len = 1;
						--pos;
						return;
					}
					iterator tmp = --pos;
					colNode *old = tmp.add->next;
					tmp.add->next = new colNode(v);
					tmp.add->next->prev = tmp.add;
					tmp.add = tmp.add->next;
					tmp.add->next = old;
					old->prev = tmp.add;
					tmp.last = tmp.add->next;
					tmp.cur->len++;
					++pos;
					return;
				}
				if (pos.add == pos.first)
				{
					pos.cur->down = new colNode(v);
					pos.first = pos.cur->down;
					pos.first->next = pos.add;
					pos.add->prev = pos.first;
					pos.cur->len++;
					pos.add = pos.add->prev;
					return;
				}
				colNode *tmp = pos.add->prev;
				tmp->next = new colNode(v);
				tmp->next->next = pos.add;
				tmp->next->prev = tmp;
				pos.add->prev = tmp->next;
				pos.add = pos.add->prev;
				pos.cur->len++;
			}
			iterator erase(iterator pos)
			{
				if (pos.head != head || pos.tail != tail)
				{
					invalid_iterator e;
					throw exception(e);
				}
				iterator res = pos;
				bool f = false;
				++res;
				colNode *tmp = pos.add;
				if (pos.add == pos.first)
				{
					pos.add = pos.add->next;
					pos.cur->down = pos.add;
					pos.first = pos.add;
					pos.cur->len--;
				}
				else
				{
					pos.add->prev->next = pos.add->next;
					pos.add->next->prev = pos.add->prev;
					pos.cur->len--;
					if (pos.add->next = pos.last)
						f = true;
					else
						pos.add = pos.add->next;
				}
				delete tmp;
				if (pos.cur->len == 0)
				{
					f = true;
					delete pos.cur->down;
					rowNode *tmp1 = pos.cur;
					if (pos.cur->prev == head)
					{
						head->next = pos.cur->next;
						pos.cur->next->prev = head;
					}
					else
					{
						pos.cur->next->prev = pos.cur->prev;
						pos.cur->prev->next = pos.cur->next;
					}
					pos.cur = pos.cur->next;
					pos.add = pos.cur->down;
					pos.first = pos.add;
					pos.last = pos.cur->bot;
					delete tmp1;
				}
				if(f)
					return res;
				return pos;

			}
			void pop_back()
			{
				if (add == first)
				{
					delete add;
					delete last;
					cur->prev->next = cur->next;
					cur->next->prev = cur->prev;
					delete cur;
					return;
				}
				colNode *t = add;
				add = add->prev;
				add->next = last;
				last->prev = add;
				cur->len--;
				delete t;
				return;
			}
			void pop_front()
			{
				if (add == last->prev)
				{
					delete add;
					delete last;
					cur->prev->next = cur->next;
					cur->next->prev = cur->prev;
					delete cur;
					return;
				}
				colNode *t = add;
				add = add->next;
				cur->down = add;
				first = add;
				cur->len--;
				delete t;
				return;
			}
			void push_back(const T &value)
			{
				if (cur->prev == head)
				{
					head->next = new rowNode;
					head->next->next = tail;
					head->next->prev = head;
					tail->prev = head->next;
					rowNode *tmp = head->next;
					tmp->down = new colNode(value);
					tmp->down->next = new colNode;
					tmp->down->next->prev = tmp->down;
					tmp->bot = tmp->down->next;
					tmp->len = 1;
					return;
				}
				iterator tmp = *this;
				--tmp;
				colNode *old = tmp.add->next;
				tmp.add->next = new colNode(value);
				tmp.add->next->prev = tmp.add;
				tmp.add = tmp.add->next;
				tmp.add->next = old;
				old->prev = tmp.add;
				tmp.last = tmp.add->next;
				tmp.cur->len++;
			}
			void push_front(const T &value)
			{
				if (cur->next == tail)
				{
					tail->prev = new rowNode;
					tail->prev->next = tail;
					tail->prev->prev = head;
					head->next = tail->prev;
					rowNode *tmp = head->next;
					tmp->down = new colNode(value);
					tmp->down->next = new colNode;
					tmp->down->next->prev = tmp->down;
					tmp->bot = tmp->down->next;
					tmp->len = 1;
					return;
				}
				iterator tmp = *this;
				++tmp;
				colNode *old = tmp.add;
				tmp.cur->down = new colNode(value);
				tmp.cur->down->next = tmp.add;
				tmp.add->prev = tmp.cur->down;
				tmp.add = tmp.add->prev;
				tmp.first = tmp.add;
				tmp.cur->len++;
			}
			void adjustBack(const int x)
			{
				if (cur->len < x + 1 || x < 6)
					return;
				int mid = cur->len >> 1;
				rowNode *record = cur->prev;
				cur->prev = new rowNode;
				cur->prev->next = cur;
				cur->prev->prev = record;
				record->next = cur->prev;
				colNode *move1 = first, *move2, *en = first;
				for (int i = 1; i < mid; i++)
				{
					move1 = move1->next;
				}
				move2 = move1->next;
				cur->prev->down = en;
				cur->prev->bot = new colNode;
				cur->prev->bot->prev = move1;
				move1->next = cur->prev->bot;
				cur->down = move2;
				move2->prev = NULL;
				first = move2;
				add = last->prev;
				cur->len -= mid;
				cur->prev->len = mid;
			}
			void adjust(const int x)
			{
				if (cur->len < x + 1 || x < 6)
					return;
				int mid = cur->len >> 1;
				bool f = false;
				rowNode *record = cur->prev;
				cur->prev = new rowNode;
				cur->prev->next = cur;
				cur->prev->prev = record;
				record->next = cur->prev;
				colNode *move1 = first, *move2, *en = first;
				for (int i = 1; i < mid; i++)
				{
					if (move1 == add)
						f = true;
					move1 = move1->next;
				}
				if (move1 == add)
					f = true;
				move2 = move1->next;
				cur->prev->down = en;
				cur->prev->bot = new colNode;
				cur->prev->bot->prev = move1;
				move1->next = cur->prev->bot;
				cur->down = move2;
				move2->prev = NULL;
				cur->len -= mid;
				cur->prev->len = mid;
				if (f)
				{
					cur = cur->prev;
					first = cur->down;
					last = cur->bot;
				}
				else
				{
					first = cur->down;
				}
			}
			iterator &operator=(const iterator &rhs)
			{
				cur = rhs.cur;
				head = rhs.head;
				tail = rhs.tail;
				add = rhs.add;
				first = rhs.first;
				last = rhs.last;
				return *this;
			}
			iterator operator+(const int &n) const {
				iterator result = *this;
				return result += n;
			}
			iterator operator-(const int &n) const {
				iterator result = *this;
				return result -= n;
			}
			// return th distance between two iterator,
			// if these two iterators points to different vectors, throw invaild_iterator.
			int operator-(const iterator &rhs) const
			{
				if (head != rhs.head || tail != rhs.tail)
				{
					invalid_iterator e;
					throw exception(e);
				}
				if (add == rhs.add)
					return 0;
				if (first == rhs.first)
				{
					colNode *tmp1 = add, *tmp2 = add;
					int res = 0;
					while (true)
					{
						if (tmp1 == rhs.add)
						{
							return res;
						}
						if (tmp2 == rhs.add)
						{
							return -res;
						}
						if (tmp1 == first || tmp2->next == last)
							break;
						++res;
						tmp1 = tmp1->prev;
						tmp2 = tmp2->next;
					}
					if (tmp1 == first)
					{
						while (tmp2 != rhs.add)
						{
							tmp2 = tmp2->next;
							++res;
						}
						return -res;
					}
					else
					{
						while (tmp1 != rhs.add)
						{
							tmp1 = tmp1->prev;
							++res;
						}
						return res;
					}
				}
				iterator t = *this;
				if (cur == tail)
				{
					--t;
					return t - rhs + 1;
				}
				iterator he(cur, head, tail, first, first, last), ta(cur, head, tail, last->prev, first, last);
				rowNode *tmp1 = cur, *tmp2 = cur;
				int res1 = *this - he, res2 = cur->len - 1 - res1;
				while (true)
				{
					if (tmp1 == rhs.cur)
					{
						ta = rhs;
						/*ta.add = ta.last->prev;
						res1 += ta - rhs;*/
						ta.add = ta.last->prev;
						while (ta.add != rhs.add)
						{
							ta.add = ta.add->prev;
							++res1;
						}
						return res1 + 1 - tmp1->len;
					}
					if (tmp2 == rhs.cur)
					{
						he = rhs;
						/*he.add = he.first;
						res2 += rhs - he;*/
						he.add = he.first;
						while (he.add != rhs.add)
						{
							he.add = he.add->next;
							++res2;
						}
						return -(res2 + 1 - tmp2->len);
					}
					if (tmp1->prev == head || tmp2->next == tail)
					{
						break;
					}
					tmp1 = tmp1->prev;
					tmp2 = tmp2->next;
					res1 += tmp1->len;
					res2 += tmp2->len;
				}
				if (tmp1->prev == head)
				{
					while (tmp2 != rhs.cur)
					{
						tmp2 = tmp2->next;
						res2 += tmp2->len;
					}
					he = rhs;
					he.add = rhs.first;
					while (he.add != rhs.add)
					{
						++res2;
						he.add = he.add->next;
					}
					//res2 += rhs - he;
					return -(res2 + 1 - tmp2->len);
				}
				else
				{
					while (tmp1 != rhs.cur)
					{
						tmp1 = tmp1->prev;
						res1 += tmp1->len;
					}
					ta = rhs;
					ta.add = ta.last->prev;
					while (ta.add != rhs.add)
					{
						++res1;
						ta.add = ta.add->prev;
					}
					//res1 += ta - rhs;
					return res1 + 1 - tmp1->len;
				}
			}
			iterator &operator+=(const int &n) {
				if (head == NULL)
					return *this;
				if (n < 1)
				{
					if (n < 0)
					{
						*this -= (-n);
						return *this;
					}
					return *this;
				}
				int r = n;
				if (cur == head)
				{
					cur = cur->next;
					first = cur->down;
					add = first;
					last = cur->bot;
					r--;
				}
				if (r == 0)
					return *this;
				/*iterator tmp = *this;
				tmp.add = tmp.last->prev;
				int t = r - (tmp - *this);*/
				/*if (t < 1)
				{
				for (int i = 0; i < r; i++)
				{
				add = add->next;
				}
				return *this;
				}*/
				if (add == first)
				{
					while (true)
					{
						if (r < cur->len)
						{
							for (int i = 0; i < r; i++)
								add = add->next;
							first = cur->down;
							last = cur->bot;
							break;
						}
						if (cur->next == tail)
						{
							if (r == cur->len)
							{
								cur = cur->next;
								add = NULL;
								first = NULL;
								last = NULL;
								return *this;
							}
							index_out_of_bound e;
							throw exception(e);
						}
						r -= cur->len;
						cur = cur->next;
						add = cur->down;
					}
					return *this;
				}
				for (; r > 0; --r)
				{
					if (add->next == last)
					{
						break;
					}
					add = add->next;
				}
				if (r == 0)
					return *this;
				if (cur->next == tail && r == 1)
				{
					cur = cur->next;
					add = NULL;
					first = NULL;
					last = NULL;
					return *this;
				}
				if (cur->next == tail)
				{
					index_out_of_bound e;
					throw exception(e);
				}

				while (true)
				{
					cur = cur->next;
					int y = cur->len;
					if (r <= cur->len)
					{
						add = cur->down;
						for (int i = 1; i < r; i++)
						{
							add = add->next;
						}
						first = cur->down;
						last = cur->bot;
						break;
					}
					if (cur->next == tail && r != cur->len + 1)
					{
						index_out_of_bound e;
						throw exception(e);
					}
					if (cur->next == tail)
					{
						cur = cur->next;
						add = NULL;
						first = NULL;
						last = NULL;
						return *this;
					}
					r -= cur->len;
				}
				return *this;
			}
			iterator &operator-=(const int &n) {
				if (head == NULL)
					return *this;
				if (n < 0)
				{
					*this += (-n);
					return *this;
				}
				if (n == 0)
					return *this;
				int r = n;
				if (cur == tail)
				{
					/*
					todo finish-1=start
					*/
					cur = cur->prev;
					first = cur->down;
					last = cur->bot;
					add = last->prev;
					r--;
				}
				if (r == 0)
					return *this;
				iterator tmp = *this;
				tmp.add = tmp.first;
				int t = r - 1 - (*this - tmp);
				if (t < 0)
				{
					for (int i = 0; i < r; i++)
					{
						add = add->prev;
					}
					return *this;
				}
				if (cur->prev == head)
				{
					index_out_of_bound e;
					throw exception(e);
				}
				while (true)
				{
					cur = cur->prev;
					add = cur->bot->prev;
					if (t < cur->len)
					{
						for (int i = 0; i < t; i++)
						{
							add = add->prev;
						}
						first = cur->down;
						last = cur->bot;
						break;
					}
					if (cur->prev == head)
					{
						index_out_of_bound e;
						throw exception(e);
					}
					t -= cur->len;
				}
				return *this;
			}
			iterator operator++(int)
			{
				iterator tmp = *this;
				++*this;
				return tmp;
			}
			iterator& operator++()
			{
				if (head == NULL)
					return *this;
				if (cur == head)
				{
					cur = cur->next;
					first = cur->down;
					add = first;
					last = cur->bot;
					return *this;
				}
				if (cur == tail)
				{
					index_out_of_bound e;
					throw exception(e);
				}
				if (add->next != last)
				{
					add = add->next;
					return *this;
				}
				if (cur->next == tail)
				{
					cur = cur->next;
					add = NULL;
					first = NULL;
					last = NULL;
					return *this;
				}
				cur = cur->next;
				add = cur->down;
				first = cur->down;
				last = cur->bot;
				return *this;
			}
			iterator operator--(int)
			{
				iterator tmp = *this;
				--*this;
				return tmp;
			}
			iterator& operator--()
			{
				if (head == NULL)
					return *this;
				if (add != first)
				{
					add = add->prev;
					return *this;
				}
				if (cur->prev == head)
				{
					index_out_of_bound e;
					throw exception(e);
				}
				cur = cur->prev;
				add = cur->bot->prev;
				first = cur->down;
				last = cur->bot;
				return *this;
			}
			T& operator*() const
			{
				if (cur == tail)
				{
					index_out_of_bound e;
					throw exception(e);
				}
				return *(add->value);
			}
			/**
			* TODO it->field
			*/
			T* operator->() const noexcept
			{
				return &(operator *());
			}
			/**
			* a operator to check whether two iterators are same (pointing to the same memory).
			*/
			bool operator==(const iterator &rhs) const { return (add == rhs.add)&&(head==rhs.head); }
			bool operator==(const const_iterator &rhs) const { return (add == rhs.add) && (head == rhs.head); }
			/**
			* some other operator for iterator.
			*/
			bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
			bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
			void del()
			{
				delete cur;
				cur == NULL;
				head = NULL;
				tail = NULL;
			}
		};
		class const_iterator {
			// it should has similar member method as iterator.
			//  and it should be able to construct from an iterator.
		public:
			rowNode * cur;
			rowNode *head;
			rowNode *tail;
			colNode * add;
			colNode *first;
			colNode *last;
		public:
			const_iterator() {
				cur = NULL;
				head = NULL;
				tail = NULL;
				add = NULL;
				first = NULL;
				last = NULL;
			}
			const_iterator(rowNode *c, rowNode *h, rowNode *t, colNode *a, colNode *f, colNode *l) :cur(c), head(h), tail(t), add(a), first(f), last(l) {}
			const_iterator(const const_iterator &other) {
				cur = other.cur;
				head = other.head;
				tail = other.tail;
				add = other.add;
				first = other.first;
				last = other.last;
			}
			const_iterator(const iterator &other) {
				cur = other.cur;
				head = other.head;
				tail = other.tail;
				add = other.add;
				first = other.first;
				last = other.last;
			}
			int operator-(const const_iterator &rhs) const
			{
				if (head != rhs.head || tail != rhs.tail)
				{
					invalid_iterator e;
					throw exception(e);
				}
				if (add == rhs.add)
					return 0;
				if (first == rhs.first)
				{
					colNode *tmp1 = add, *tmp2 = add;
					int res = 0;
					while (true)
					{
						if (tmp1 == rhs.add)
						{
							return res;
						}
						if (tmp2 == rhs.add)
						{
							return -res;
						}
						if (tmp1 == first || tmp2->next == last)
							break;
						++res;
						tmp1 = tmp1->prev;
						tmp2 = tmp2->next;
					}
					if (tmp1 == first)
					{
						while (tmp2 != rhs.add)
						{
							tmp2 = tmp2->next;
							++res;
						}
						return -res;
					}
					else
					{
						while (tmp1 != rhs.add)
						{
							tmp1 = tmp1->prev;
							++res;
						}
						return res;
					}
				}
				const_iterator t = *this;
				if (cur == tail)
				{
					--t;
					return t - rhs + 1;
				}
				const_iterator he(cur, head, tail, first, first, last), ta(cur, head, tail, last->prev, first, last);
				rowNode *tmp1 = cur, *tmp2 = cur;
				int res1 = *this - he, res2 = cur->len - 1 - res1;
				while (true)
				{
					if (tmp1 == rhs.cur)
					{
						ta = rhs;
						/*ta.add = ta.last->prev;
						res1 += ta - rhs;*/
						ta.add = ta.last->prev;
						while (ta.add != rhs.add)
						{
							ta.add = ta.add->prev;
							++res1;
						}
						return res1 + 1 - tmp1->len;
					}
					if (tmp2 == rhs.cur)
					{
						he = rhs;
						/*he.add = he.first;
						res2 += rhs - he;*/
						he.add = he.first;
						while (he.add != rhs.add)
						{
							he.add = he.add->next;
							++res2;
						}
						return -(res2 + 1 - tmp2->len);
					}
					if (tmp1->prev == head || tmp2->next == tail)
					{
						break;
					}
					tmp1 = tmp1->prev;
					tmp2 = tmp2->next;
					res1 += tmp1->len;
					res2 += tmp2->len;
				}
				if (tmp1->prev == head)
				{
					while (tmp2 != rhs.cur)
					{
						tmp2 = tmp2->next;
						res2 += tmp2->len;
					}
					he = rhs;
					he.add = rhs.first;
					while (he.add != rhs.add)
					{
						++res2;
						he.add = he.add->next;
					}
					//res2 += rhs - he;
					return -(res2 + 1 - tmp2->len);
				}
				else
				{
					while (tmp1 != rhs.cur)
					{
						tmp1 = tmp1->prev;
						res1 += tmp1->len;
					}
					ta = rhs;
					ta.add = ta.last->prev;
					while (ta.add != rhs.add)
					{
						++res1;
						ta.add = ta.add->prev;
					}
					//res1 += ta - rhs;
					return res1 + 1 - tmp1->len;
				}
			}
			const_iterator operator+(const int &n) const {
				const_iterator result = *this;
				return result += n;
			}
			const_iterator operator-(const int &n) const {
				const_iterator result = *this;
				return result -= n;
			}
			const_iterator &operator+=(const int &n) {
				if (n < 1)
				{
					if (n < 0)
					{
						*this -= (-n);
						return *this;
					}
					return *this;
				}
				int r = n;
				if (cur == head)
				{
					cur = cur->next;
					first = cur->down;
					add = first;
					last = cur->bot;
					r--;
				}
				if (r == 0)
					return *this;
				/*iterator tmp = *this;
				tmp.add = tmp.last->prev;
				int t = r - (tmp - *this);*/
				/*if (t < 1)
				{
				for (int i = 0; i < r; i++)
				{
				add = add->next;
				}
				return *this;
				}*/
				if (add == first)
				{
					while (true)
					{
						if (r < cur->len)
						{
							for (int i = 0; i < r; i++)
								add = add->next;
							first = cur->down;
							last = cur->bot;
							break;
						}
						if (cur->next == tail)
						{
							if (r == cur->len)
							{
								cur = cur->next;
								add = NULL;
								first = NULL;
								last = NULL;
								return *this;
							}
							index_out_of_bound e;
							throw exception(e);
						}
						r -= cur->len;
						cur = cur->next;
						add = cur->down;
					}
					return *this;
				}
				for (; r > 0; --r)
				{
					if (add->next == last)
					{
						break;
					}
					add = add->next;
				}
				if (r == 0)
					return *this;
				if (cur->next == tail && r == 1)
				{
					cur = cur->next;
					add = NULL;
					first = NULL;
					last = NULL;
					return *this;
				}
				if (cur->next == tail)
				{
					index_out_of_bound e;
					throw exception(e);
				}

				while (true)
				{
					cur = cur->next;
					int y = cur->len;
					if (r <= cur->len)
					{
						add = cur->down;
						for (int i = 1; i < r; i++)
						{
							add = add->next;
						}
						first = cur->down;
						last = cur->bot;
						break;
					}
					if (cur->next == tail && r != cur->len + 1)
					{
						index_out_of_bound e;
						throw exception(e);
					}
					if (cur->next == tail)
					{
						cur = cur->next;
						add = NULL;
						first = NULL;
						last = NULL;
						return *this;
					}
					r -= cur->len;
				}
				return *this;
			}
			const_iterator &operator-=(const int &n) {
				if (n < 0)
				{
					*this += (-n);
					return *this;
				}
				if (n == 0)
					return *this;
				int r = n;
				if (cur == tail)
				{
					/*
					todo finish-1=start
					*/
					cur = cur->prev;
					first = cur->down;
					last = cur->bot;
					add = last->prev;
					r--;
				}
				if (r == 0)
					return *this;
				const_iterator tmp = *this;
				tmp.add = tmp.first;
				int t = r - 1 - (*this - tmp);
				if (t < 0)
				{
					for (int i = 0; i < r; i++)
					{
						add = add->prev;
					}
					return *this;
				}
				if (cur->prev == head)
				{
					index_out_of_bound e;
					throw exception(e);
				}
				while (true)
				{
					cur = cur->prev;
					add = cur->bot->prev;
					if (t < cur->len)
					{
						for (int i = 0; i < t; i++)
						{
							add = add->prev;
						}
						first = cur->down;
						last = cur->bot;
						break;
					}
					if (cur->prev == head)
					{
						index_out_of_bound e;
						throw exception(e);
					}
					t -= cur->len;
				}
				return *this;
			}
			const_iterator operator++(int)
			{
				const_iterator tmp = *this;
				++*this;
				return tmp;
			}
			const_iterator& operator++()
			{
				if (cur == head)
				{
					cur = cur->next;
					first = cur->down;
					add = first;
					last = cur->bot;
					return *this;
				}
				if (cur == tail)
				{
					index_out_of_bound e;
					throw exception(e);
				}
				if (add->next != last)
				{
					add = add->next;
					return *this;
				}
				if (cur->next == tail)
				{
					cur = cur->next;
					add = NULL;
					first = NULL;
					last = NULL;
					return *this;
				}
				cur = cur->next;
				add = cur->down;
				first = cur->down;
				last = cur->bot;
				return *this;
			}
			const_iterator operator--(int)
			{
				const_iterator tmp = *this;
				--*this;
				return tmp;
			}
			const_iterator& operator--()
			{
				if (add != first)
				{
					add = add->prev;
					return *this;
				}
				if (cur->prev == head)
				{
					index_out_of_bound e;
					throw exception(e);
				}
				cur = cur->prev;
				add = cur->down;
				for (int i = 1; i < cur->len; i++)
				{
					add = add->next;
				}
				first = cur->down;
				last = cur->bot;
				return *this;
			}
			T& operator*() const
			{
				return *(add->value);
			}
			/**
			* TODO it->field
			*/
			T* operator->() const noexcept
			{
				return &(operator *());
			}
			/**
			* a operator to check whether two iterators are same (pointing to the same memory).
			*/
			bool operator==(const iterator &rhs) const { return add == rhs.add; }
			bool operator==(const const_iterator &rhs) const { return add == rhs.add; }
			/**
			* some other operator for iterator.
			*/
			bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
			bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
		};
		iterator start;
		iterator finish;
		bool flag;
		int length;
		deque()
		{
			length = 0;
			start.init(finish);
			flag = true;
		}
		deque(const deque &other)
		{
			if (this == &other)
				return;
			length = other.length;
			if (length == 0)
			{
				start.init(finish);
				flag = true;
				return;
			}
			start.init(finish);
			flag = true;
			start.copy(finish, other.start, other.finish);
		}
		~deque()
		{
			if (length != 0)
				start.clear(finish);
			if (flag)
			{
				start.del();
				finish.del();
				flag = false;
			}
		}
		deque &operator=(const deque &other)
		{
			if (this == &other)
				return *this;
			clear();
			length = other.length;
			if (length == 0)
			{
				start.init(finish);
				flag = true;
				return *this;
			}
			start.init(finish);
			flag = true;
			start.copy(finish, other.start, other.finish);
			return *this;
		}
		/**
		* access specified element with bounds checking
		* throw index_out_of_bound if out of bound.
		*/
		T & at(const size_t &pos)
		{
			if (pos >= length || pos < 0)
			{
				index_out_of_bound e;
				throw exception(e);
			}
			return *(start + (pos + 1));
		}
		const T & at(const size_t &pos) const
		{
			if (pos >= length || pos < 0)
			{
				index_out_of_bound e;
				throw exception(e);
			}
			return *(start + (pos + 1));
		}
		T & operator[](const size_t &pos)
		{
			if (pos >= length || pos < 0)
			{
				index_out_of_bound e;
				throw exception(e);
			}
			return *(start + (pos + 1));
		}
		const T & operator[](const size_t &pos) const
		{
			if (pos >= length || pos < 0)
			{
				index_out_of_bound e;
				throw exception(e);
			}
			return *(start + (pos + 1));
		}
		/**
		* access the first element
		* throw container_is_empty when the container is empty.
		*/
		const T & front() const
		{
			if (length == 0)
			{
				container_is_empty e;
				throw exception(e);
			}
			iterator tmp = start;
			++tmp;
			return *tmp;
		}
		/**
		* access the last element
		* throw container_is_empty when the container is empty.
		*/
		const T & back() const
		{
			if (length == 0)
			{
				container_is_empty e;
				throw exception(e);
			}
			iterator tmp = finish;
			--tmp;
			return *tmp;
		}
		/**
		* returns an iterator to the beginning.
		*/
		iterator begin()
		{
			iterator tmp = start;
			++tmp;
			return tmp;
		}
		const_iterator cbegin() const
		{
			const_iterator tmp = start+1;
			return tmp;
		}
		/**
		* returns an iterator to the end.
		*/
		iterator end()
		{
			return finish;
		}
		const_iterator cend() const
		{
			const_iterator tmp = finish;
			return tmp;
		}
		/**
		* checks whether the container is empty.
		*/
		bool empty() const
		{
			return length == 0;
		}
		/**
		* returns the number of elements
		*/
		size_t size() const
		{
			return length;
		}
		/**
		* clears the contents
		*/
		void clear()
		{
			if (length == 0)
			{
				if (flag)
				{
					start.del();
					finish.del();
					flag = false;
				}
				return;
			}
			length = 0;
			start.clear(finish);
			if (flag)
			{
				start.del();
				finish.del();
				flag = false;
			}
		}
		/**
		* inserts elements at the specified locat on in the container.
		* inserts value before pos
		* returns an iterator pointing to the inserted value
		*     throw if the iterator is invalid or it point to a wrong place.
		*/
		iterator insert(iterator pos, const T &value)
		{
			if (!flag)
			{
				start.init(finish);
				flag = true;
			}
			length++;
			start.insert(pos, value);
			pos.adjust(sqrt(length << 1));
			return pos;
		}
		/**
		* removes specified element at pos.
		* removes the element at pos.
		* returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
		* throw if the container is empty, the iterator is invalid or it points to a wrong place.
		*/
		iterator erase(iterator pos)
		{
			if (length == 0)
			{
				container_is_empty e;
				throw exception(e);
			}
			length--;
			iterator tmp = start.erase(pos);
			return tmp;
			/*if (length == 0)
			return tmp;
			tmp.adjust(sqrt(length));
			return tmp;*/
		}
		/**
		* adds an element to the end
		*/
		void push_back(const T &value)
		{
			if (!flag)
			{
				start.init(finish);
				flag = true;
			}
			length++;
			finish.push_back(value);
			iterator tmp = finish;
			--tmp;
			tmp.adjustBack(sqrt(length << 1));
		}
		/**
		* removes the last element
		*     throw when the container is empty.
		*/
		void pop_back()
		{
			if (length == 0)
			{
				container_is_empty e;
				throw exception(e);
			}
			length--;
			iterator p = finish;
			--p;
			p.pop_back();
			/*if (length == 0)
			return;
			iterator tmp = finish;
			--tmp;
			tmp.adjustBack(sqrt(length));*/
		}
		/**
		* inserts an element to the beginning.
		*/
		void push_front(const T &value)
		{
			if (!flag)
			{
				start.init(finish);
				flag = true;
			}
			length++;
			start.push_front(value);
			iterator tmp = start;
			++tmp;
			tmp.adjust(sqrt(length << 1));
		}
		/**
		* removes the first element.
		*     throw when the container is empty.
		*/
		void pop_front()
		{
			if (length == 0)
			{
				container_is_empty e;
				throw exception(e);
			}
			length--;
			iterator t = start;
			++t;
			t.pop_front();
			/*if (length == 0)
			return;
			iterator tmp = start;
			++tmp;
			tmp.adjustBack(sqrt(length));*/
		}

	};

}

#endif
