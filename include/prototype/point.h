#include "prototype.h"
#include <ostream>

#pragma once

namespace prototype
{
	template<size_t S, typename T, bool C>
	class matrix;

	template<size_t S, typename T>
	class point
	{
	public:
		typedef point<S, T> self_t;
		typedef T value_t;
		typedef T array_t[S];

		PROTOTYPE_INLINE point() { clear(); }
		PROTOTYPE_INLINE point(array_t const& _arr) { set(_arr); }

		PROTOTYPE_INLINE array_t const& get() const { return pt; }
		PROTOTYPE_INLINE array_t &get() { return pt; }
		
		PROTOTYPE_INLINE value_t const& get(size_t _idx) const { return pt[_idx]; }
		PROTOTYPE_INLINE value_t &get(size_t _idx) { return pt[_idx]; }

		void set(array_t const& _arr)
		{
			memcpy(pt, _arr, sizeof(_arr));
		}

		void clear()
		{
			memset(pt, 0, sizeof(pt));
		}

		self_t add(self_t const& _b) const
		{
			self_t ret;
			
			for(size_t i = 0; i < S; i++)
				ret[i] += get(i) + _b[i];

			return ret;
		}

		void addi(self_t const& _b)
		{
			for(size_t i = 0; i < S; i++)
				pt[i] += _b[i];
		}

		template<bool C>
		PROTOTYPE_INLINE self_t mult(matrix<S, T, C> const& _mat) const
		{
			self_t ret;

			for(size_t r = 0; r < S; r++)
			{
				for(size_t c = 0; c < S; c++)
					ret[r] += _mat.get(r, c)*pt[r];
			}

			return ret;
		}

		template<bool C>
		PROTOTYPE_INLINE self_t mult(matrix<S+1, T, C> const& _mat) const
		{
			self_t ret;

			for(size_t r = 0; r < S; r++)
			{
				for(size_t c = 0; c < S; c++)
					ret[r] += _mat.get(r, c)*pt[r];
			}

			for(size_t r = 0; r < S; r++)
				ret[r] += _mat.get(r, S);

			return ret;
		}

		template<bool C>
		PROTOTYPE_INLINE void multi(matrix<S, T, C> const& _mat)
		{
			self_t res = mult(_mat);
			set(res);
		}

		template<bool C>
		PROTOTYPE_INLINE void multi(matrix<S+1, T, C> const& _mat)
		{
			self_t res = mult(_mat);
			set(res);
		}
		
		PROTOTYPE_INLINE value_t const& operator [](size_t _idx) const { return get(_idx); }
		PROTOTYPE_INLINE value_t &operator [](size_t _idx) { return get(_idx); }
		
		template<bool C>
		PROTOTYPE_INLINE self_t operator *(matrix<S+1, T, C> const& _mat) const { return mult(_mat); }
		template<bool C>
		PROTOTYPE_INLINE self_t operator *(matrix<S, T, C> const& _mat) const { return mult(_mat); }
		template<bool C>
		PROTOTYPE_INLINE self_t &operator *=(matrix<S+1, T, C> const& _mat) { multi(_mat); return *this; }
		template<bool C>
		PROTOTYPE_INLINE self_t &operator *=(matrix<S, T, C> const& _mat) { multi(_mat); return *this; }

	private:
		array_t pt;
	};

	template<size_t S, typename T>
	PROTOTYPE_INLINE std::ostream &operator <<(std::ostream &_str, point<S, T> const& _pt)
	{
		_str << '(';
		for(size_t i = 0; i < S; i++)
		{
			_str << _pt[i];
			if(i < (S-1))
				_str << ", ";
		}
		_str << ')';
		return _str;
	}
	
	typedef point<2, float> point2f;
	typedef point<2, double> point2d;
	
	typedef point<3, float> point3f;
	typedef point<3, double> point3d;
}
