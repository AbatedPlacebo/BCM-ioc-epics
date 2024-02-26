#ifndef CACHEDEV_H
#define CACHEDEV_H

#if 0 // c++17

#include <type_traits>
//#include <functional>

#define DEFINE_MEMBERFUNC_CHECKER(memberf)                                             \
template<typename T, typename... Args>                                                 \
struct has_##memberf {                                                                 \
	private:                                                                           \
		template<typename TT, typename... AArgs,                                       \
			typename = decltype(std::declval<TT>().memberf(std::declval<AArgs>()...))> \
		static std::true_type f(int);                                                  \
		template<typename...>                                                          \
		static std::false_type f(...);                                                 \
	public:                                                                            \
		using type = decltype(f<T, Args...>(0));                                       \
};                                                                                     \
template<typename T, typename... Args>                                                 \
constexpr static bool has_##memberf##_v =                                              \
	std::is_same_v< typename has_##memberf<T, Args...>::type, std::true_type>;         \
/* end DEFINE_MEMBERFUNC_CHECKER */

#define HAS_MEMBERFUNC(C, memberf) has_##memberf##_v<C>


template<template<typename> typename BASE>
struct CACHE_NONE
{
	CACHE_NONE() = delete;
	CACHE_NONE(const CACHE_NONE&) = delete;
	CACHE_NONE(CACHE_NONE&&) = delete;
	CACHE_NONE& operator=(const CACHE_NONE&) = delete;
	CACHE_NONE& operator=(CACHE_NONE&&) = delete;

	//typedef BASE::INFO INFO;
	template<typename DEV>
		class P:public BASE<DEV>
	{
		DEFINE_MEMBERFUNC_CHECKER(reset);
		public:
			void reset(){
				if constexpr (HAS_MEMBERFUNC(BASE<DEV>,reset))
				{
					//std::cout << typeid(*this).name()  <<  " " << __FUNCTION__ << std::endl;
					BASE<DEV>::reset();
				}
				else {
					//std::cout << "not proto.reset();" << std::endl;
				}
			}
	};
};

#define CACHE_NONE(proto) CACHE_NONE<proto>::P
#endif

template<template<typename> typename BASE>
struct SCACHE_RO
{
	SCACHE_RO() = delete;
	SCACHE_RO(const SCACHE_RO&) = delete;
	SCACHE_RO(SCACHE_RO&&) = delete;
	SCACHE_RO& operator=(const SCACHE_RO&) = delete;
	SCACHE_RO& operator=(SCACHE_RO&&) = delete;

	template<typename DEV>
		class PROTO_RO : public BASE<DEV>
	{
		typedef DEV INFO;
		typedef BASE<DEV> TBASE;
		typename INFO::REG_CACHE_MASK_t reg_cache_mask;
		typename INFO::REGHW_t reg_cache[INFO::REG_SIZE];
		public:
			PROTO_RO(): TBASE() {
				reset();
			}
			void reset() {
				reg_cache_mask = 0;
			}
			int connect(const char *peer, int port) {
				reset();
				return TBASE::connect(peer, port);
			}
			int disconnect() {
				reset();
				return TBASE::disconnect();
			}
			int wr_reg(unsigned int regn, unsigned int param) {
				if((1 << regn) & INFO::REG_CACHE_MASK) {
					reg_cache[regn] = param;
					reg_cache_mask |= (1 << regn);
				}
				return TBASE::wr_reg(regn, param);
			}
			int rd_reg(unsigned int regn, unsigned int *param) {
				int ret = 0;
				unsigned int tp = 0;
				if((1 << regn) & reg_cache_mask) {
					*param = reg_cache[regn];
					return ret;
				}
				if((ret = TBASE::rd_reg(regn, &tp)) < 0)
					return ret;
				*param = tp;
				if((1 << regn) & INFO::REG_CACHE_MASK) {
					reg_cache[regn] = tp;
					reg_cache_mask |= (1 << regn);
				}
				return ret;
			}
	};

	template<typename DEV>
		using type = PROTO_RO<DEV>;
};

#define CACHE_RO(proto) SCACHE_RO<proto>::type

template<template<typename> typename BASE>
struct SCACHE_RW
{
	SCACHE_RW() = delete;
	SCACHE_RW(const SCACHE_RW&) = delete;
	SCACHE_RW(SCACHE_RW&&) = delete;
	SCACHE_RW& operator=(const SCACHE_RW&) = delete;
	SCACHE_RW& operator=(SCACHE_RW&&) = delete;

	template<typename DEV>
		class PROTO_RW : public BASE<DEV>
	{
		typedef DEV INFO;
		typedef BASE<DEV> TBASE;
		typename INFO::REG_CACHE_MASK_t reg_cache_mask;
		typename INFO::REGHW_t reg_cache[INFO::REG_SIZE];
		public:
			PROTO_RW():TBASE() {
				reset();
			}
			void reset() {
				reg_cache_mask = 0;
			}
			int connect(const char *peer, int port) {
				reset();
				return TBASE::connect(peer, port);
			}
			int disconnect() {
				reset();
				return TBASE::disconnect();
			}
			int wr_reg(unsigned int regn, unsigned int param) {
				if((1 << regn) & INFO::REG_CACHE_MASK) {
					if(((1 << regn) & reg_cache_mask) && reg_cache[regn] == param)
						return 0;
					reg_cache[regn] = param;
					reg_cache_mask |= (1 << regn);
				}
				return TBASE::wr_reg(regn, param);
			}
			int rd_reg(unsigned int regn, unsigned int *param) {
				int ret = 0;
				unsigned int tp = 0;
				if((1 << regn) & reg_cache_mask) {
					*param = reg_cache[regn];
					return ret;
				}
				ret = TBASE::rd_reg(regn, &tp);
				if(ret < 0)
					return ret;
				*param = tp;
				if((1 << regn) & INFO::REG_CACHE_MASK) {
					reg_cache[regn] = tp;
					reg_cache_mask |= (1 << regn);
				}
				return ret;
			}
	};

	template<typename DEV>
		using type = PROTO_RW<DEV>;
};

#define CACHE_RW(proto) SCACHE_RW<proto>::type

#endif // CACHEDEV_H
