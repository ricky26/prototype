#include "prototype.h"
#include <ostream>

#pragma once

namespace prototype
{
	template<size_t S, typename T, bool C>
	class matrix;

	template<size_t S, typename T>
	class point_base
	{
	public:
		typedef point_base<S, T> self_t;
		typedef T value_t;
		typedef T array_t[S];

		PROTOTYPE_INLINE point_base() { clear(); }
		PROTOTYPE_INLINE point_base(self_t const& _b) { set(_b.pt); }
		PROTOTYPE_INLINE point_base(array_t const& _arr) { set(_arr); }

		PROTOTYPE_INLINE array_t const& get() const { return pt; }
		PROTOTYPE_INLINE array_t &get() { return pt; }
		
		PROTOTYPE_INLINE value_t const& get(size_t _idx) const { return pt[_idx]; }
		PROTOTYPE_INLINE value_t &get(size_t _idx) { return pt[_idx]; }

		bool zero() const
		{
			for(size_t i = 0; i < S; i++)
				if(pt[i] != 0)
					return false;

			return true;
		}

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
				ret[i] = get(i) + _b[i];

			return ret;
		}

		void addi(self_t const& _b)
		{
			for(size_t i = 0; i < S; i++)
				pt[i] += _b[i];
		}

		self_t sub(self_t const& _b) const
		{
			self_t ret;
			
			for(size_t i = 0; i < S; i++)
				ret[i] = get(i) - _b[i];

			return ret;
		}

		void subi(self_t const& _b)
		{
			for(size_t i = 0; i < S; i++)
				pt[i] -= _b[i];
		}

		self_t mult(float _f) const
		{
			self_t ret;

			for(size_t r = 0; r < S; r++)
			{
				ret[r] = pt[r] * _f;
			}

			return ret;
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

		void multi(float _f)
		{
			for(size_t r = 0; r < S; r++)
				pt[r] *= _f;
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

		self_t div(float _f) const
		{
			self_t ret;

			for(size_t r = 0; r < S; r++)
			{
				ret[r] = pt[r] / _f;
			}

			return ret;
		}

		void divi(float _f)
		{
			for(size_t r = 0; r < S; r++)
				pt[r] /= _f;
		}
		
		PROTOTYPE_INLINE value_t const& operator [](size_t _idx) const { return get(_idx); }
		PROTOTYPE_INLINE value_t &operator [](size_t _idx) { return get(_idx); }
		
		PROTOTYPE_INLINE self_t operator +(self_t const& _b) const { return add(_b); }
		PROTOTYPE_INLINE self_t &operator +=(self_t const& _b) { addi(_b); return *this; }
		
		PROTOTYPE_INLINE self_t operator -(self_t const& _b) const { return sub(_b); }
		PROTOTYPE_INLINE self_t &operator -=(self_t const& _b) { subi(_b); return *this; }

		PROTOTYPE_INLINE self_t operator *(float _f) const { return mult(_f); }
		PROTOTYPE_INLINE self_t &operator *=(float _f) { multi(_f); return *this; }

		PROTOTYPE_INLINE self_t operator /(float _f) const { return div(_f); }
		PROTOTYPE_INLINE self_t &operator /=(float _f) { divi(_f); return *this; }
		
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
	class point: public point_base<S, T>
	{
	};

	template<typename T>
	class point<2, T>: public point_base<2, T>
	{
	public:
		inline point(): point_base() {}
		inline point(point_base<2, T> const& _b): point_base(_b) {}
		inline point(array_t const& _a): point_base(_a) {}
		inline point(T _x, T _y): point_base()
		{
			T arr[2] = {_x, _y};
			set(arr);
		}

		inline T &x() { return get(0); }
		inline T x() const { return get(0); }
		
		inline T &y() { return get(1); }
		inline T y() const { return get(1); }
	};

	template<typename T>
	class point<3, T>: public point_base<3, T>
	{
	public:
		inline point(): point_base() {}
		inline point(array_t const& _a): point_base(_a) {}
		inline point(T _x, T _y, T _z): point_base()
		{
			T arr[3] = {_x, _y, _z};
			set(arr);
		}

		inline T &x() { return get(0); }
		inline T x() const { return get(0); }
		
		inline T &y() { return get(1); }
		inline T y() const { return get(1); }
		
		inline T &z() { return get(2); }
		inline T z() const { return get(2); }
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
