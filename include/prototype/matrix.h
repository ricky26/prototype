#include "point.h"
#include <cstring>

#pragma once

namespace prototype
{
	template<size_t S, typename T, bool C>
	struct matrix_major
	{
		typedef T array_t[S*S];

		PROTOTYPE_INLINE static size_t index(size_t _r, size_t _c)
		{
			return (S*_c) + _r;
		}
	};

	template<size_t S, typename T>
	struct matrix_major<S, T, false>
	{
		PROTOTYPE_INLINE static size_t index(size_t _r, size_t _c)
		{
			return (S*_r) + _c;
		}
	};

	template<size_t S, typename T=float, bool C=false>
	class matrix
	{
	public:
		typedef T value_t;
		typedef T array_type[S*S];
		typedef matrix<S, T, C> self_t;
		typedef point<S-1, T> point_t;
		typedef matrix_major<S, T, C> major_t;

		static const size_t size = S;

		PROTOTYPE_INLINE matrix(array_type const& _arr) { set(_arr); }
		PROTOTYPE_INLINE matrix() { clear(); }

		PROTOTYPE_INLINE array_type const& get() const { return mat; }

		PROTOTYPE_INLINE T get(size_t _idx) const { return mat[_idx]; }
		PROTOTYPE_INLINE T &get(size_t _idx) { return mat[_idx]; }
		
		PROTOTYPE_INLINE T get(size_t _r, size_t _c) const { return mat[index(_r, _c)]; }
		PROTOTYPE_INLINE T &get(size_t _r, size_t _c) { return mat[index(_r, _c)]; }

		PROTOTYPE_INLINE void set(array_type const& _arr)
		{
			std::memcpy(mat, _arr, sizeof(_arr));
		}

		PROTOTYPE_INLINE void set(self_t const& _b)
		{
			set(_b.mat);
		}

		PROTOTYPE_INLINE void clear()
		{
			std::memset(mat, 0, sizeof(mat));
		}

		PROTOTYPE_INLINE size_t index(size_t _r, size_t _c) const
		{
			return major_t::index(_r, _c);
		}

		self_t mult(self_t const& _b) const
		{
			self_t ret;

			for(size_t r = 0; r < S; r++)
				for(size_t c = 0; c < S; c++)
					for(size_t k = 0; k < S; k++)
						ret.get(r, c) += get(r, k)*_b.get(k, c);

			return ret;
		}

		void multi(self_t const& _b)
		{
			self_t res = mult(_b);
			set(res);
		}

		static self_t identity()
		{
			self_t ret;
			for(size_t r = 0; r < S; r++)
				ret.get(r, r) = 1;

			return ret;
		}
		
		PROTOTYPE_INLINE T operator[](size_t _idx) const { return get(_idx); }
		PROTOTYPE_INLINE T operator[](size_t _idx) { return get(_idx); }

		PROTOTYPE_INLINE self_t &operator *=(self_t const& _b) { multi(_b); return *this; }
		PROTOTYPE_INLINE self_t operator *(self_t const& _b) const { return mult(_b); }

	private:
		array_type mat;
	};

	template<size_t S, typename T, bool C>
	PROTOTYPE_INLINE std::ostream &operator <<(std::ostream &_str, matrix<S, T, C> const& _mat)
	{
		_str << '[';
		for(size_t r = 0; r < S; r++)
		{
			for(size_t c = 0; c < S; c++)
			{
				_str << _mat.get(r, c);

				if(c < (S-1))
					_str << ", ";
			}
			
			if(r < (S-1))
				_str << "; ";
		}
		_str << ']';
		return _str;
	}
	
	typedef matrix<4, float> matrix4f;
	typedef matrix<4, double> matrix4d;

	typedef matrix<4, float, true> matrix4fc;
	typedef matrix<4, double, true> matrix4dc;

	typedef matrix<4, float, false> matrix4fr;
	typedef matrix<4, double, false> matrix4dr;
}
