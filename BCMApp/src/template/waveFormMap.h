#ifndef WAVEFORMMAP_H
#define WAVEFORMMAP_H

#include <cstddef>


template <typename T, int DATA_SIZE>
class wave_form_map {
		T* storage;
		int* ne;
	public:
		wave_form_map() = delete;
		wave_form_map(T* _storage, int* _ne): storage(_storage), ne(_ne) {}
		wave_form_map(const wave_form_map& src):storage(src.storage), ne(src.ne) {}

		void operator=(const wave_form_map& src)
		{
			size_t _sz = resize(src.size());
			for(size_t i = 0; i < _sz; ++i)
				storage[i] = src[i];
		}

		size_t size() { return *ne; }
		size_t resize (int _ne) {
			_ne = _ne <= DATA_SIZE ? _ne : DATA_SIZE;
			*ne = _ne;
			return size();
		}
		size_t maxsize()                 { return DATA_SIZE; }

		T& operator[](int i)             { return storage[i]; }
		const T& operator[](int i) const { return storage[i]; }

		void linspace(float t0, float tmult, int _size = DATA_SIZE) {
			_size = resize(_size);
			for(int i = 0; i < _size; ++i)
				storage[i] = t0 + tmult * i;
		}

		template<typename U>
		void linspace(float t0, float tmult, wave_form_map<U, DATA_SIZE> idx) {
			int _size = resize(idx.size());
			for(int i = 0; i < _size; ++i)
				storage[i] = t0 + tmult * idx[i];
		}

		void operator*=(float value) {
			size_t _sz = size();
			for (size_t i = 0; i < _sz; ++i)
				storage[i] *= value;
		}

		T max(size_t begin = 0, size_t end = DATA_SIZE) {
			T value = storage[begin];
			if( end < size() )
				end = size();
			for(size_t i = begin; i < end; ++i)
				if( value < storage[i] )
					value = storage[i];
			return value;
		}

		T min(size_t begin=0, size_t end = DATA_SIZE) {
			T value = storage[begin];
			if( end < size() )
				end = size();
			for(size_t i = begin; i < end; ++i)
				if( value > storage[i] )
					value = storage[i];
			return value;
		}

		float mean(size_t begin = 0, size_t end = DATA_SIZE) {
			float v = 0;
			float w = 0;
			if( end > size() )
				end = size();
			for(size_t i = begin; i < end; ++i) {
				w += 1;
				v += storage[i];
			}
			if(w > 0)
				v /= w;

			return v;
		}

		template<typename U>
		float mean(wave_form_map<U, DATA_SIZE> weight, size_t begin = 0, size_t end = DATA_SIZE) {
			float v = 0;
			float w = 0;
			if( end > size() )
				end = size();
			for(size_t i = begin; i < end; ++i) {
				float wi = weight[i];
				w += wi;
				v += storage[i] * wi;
			}
			if(w > 0)
				v /= w;

			return v;
		}

		template<typename U>
		float mean2(wave_form_map<U, DATA_SIZE> weight, size_t begin = 0, size_t end = DATA_SIZE) {
			float v = 0;
			float w = 0;
			if( end > size() )
				end = size();
			for(size_t i = begin; i < end; ++i) {
				float wi = weight[i] * weight[i];
				w += wi;
				v += storage[i] * wi;
			}
			if(w > 0)
				v /= w;

			return v;
		}

};

#include<array>

template<class T> struct wfm_remove_extent { using type = T;/* enum{ size = 0}; */ };
//template<class T> struct wfm_remove_extent<T[]> { using type = T; };
template<class T, std::size_t N> struct wfm_remove_extent<T[N]> { using type = T; enum{ size = N}; };

#define WFM_t(type,size) wave_form_map<type, size>
#define WFM4_t(type,size) std::array<WFM_t(type,size), 4>

#define WFMtype(stor) WFM_t(typename wfm_remove_extent<decltype(stor)>::type, wfm_remove_extent<decltype(stor)>::size)

#define WFM4type(stor) std::array<WFMtype(stor##0), 4>

#define WFM(stor) WFMtype(stor)(stor, & stor##_ne)

#define WFM4(stor) \
	WFM4type(stor) {\
	WFMtype(stor##0) (stor##0, & stor##0_ne), \
	WFMtype(stor##0) (stor##1, & stor##1_ne), \
	WFMtype(stor##0) (stor##2, & stor##2_ne), \
	WFMtype(stor##0) (stor##3, & stor##3_ne)  \
	}

#endif
