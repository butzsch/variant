#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

namespace cpe
{
    namespace detail
    {
        template<typename ...>
        struct OverloadHelper;

        template<typename T>
        struct OverloadHelper<T>
        {
            static T test_construction(T const &);
        };

        template<typename T, typename ... Ts>
        struct OverloadHelper<T, Ts ...>
            : OverloadHelper<Ts ...>
        {
            using OverloadHelper<Ts ...>::test_construction;

            static T test_construction(T const &);
        };

        template<std::size_t ...>
        struct AccumulateMaxSize;

        template<std::size_t A>
        struct AccumulateMaxSize<A>
            : std::integral_constant<std::size_t, A>
        {};

        template<std::size_t A, std::size_t B, std::size_t ... Rest>
        struct AccumulateMaxSize<A, B, Rest ...>
            : std::integral_constant<
                std::size_t,
                AccumulateMaxSize<(A > B ? A : B), Rest ...>::value
            >
        {};
    }

    template<typename T, typename ... Ts>
    struct ConstructionHelper
    {
        using type = decltype(
            detail::OverloadHelper<Ts ...>::test_construction(std::declval<T>())
        );
    };

    template<typename ... Ts>
    using MaxSize = detail::AccumulateMaxSize<sizeof(Ts) ...>;

    template<typename ...>
    struct IndexOf;

    template<typename S, typename ... Ts>
    struct IndexOf<S, S, Ts ...>
        : std::integral_constant<std::size_t, 0>
    {};

    template<typename S, typename T, typename ... Ts>
    struct IndexOf<S, T, Ts ...>
        : std::integral_constant<std::size_t, IndexOf<S, Ts ...>::value + 1>
    {};

	template<bool ...>
	struct BoolList
	{};

	template<bool ... Args>
	struct AllOf
		: std::integral_constant<
			bool,
			std::is_same<
				BoolList<true, Args ...>,
				BoolList<Args ..., true>
			>::value
		>
	{};

    template<bool B, class T = void>
    using enable_if_t = typename std::enable_if<B, T>::type;

    template<typename T>
    using decay_t = typename std::decay<T>::type;
}
