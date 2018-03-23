#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

#include "type_traits.hxx"

namespace cpe
{
	template<typename ... Ts>
	class Variant
	{
	public:
		template<typename T, typename = enable_if_t<!std::is_same<decay_t<T>, Variant>::value>>
		explicit Variant(T && value)
		{
			using InitialType = typename ConstructionHelper<T, Ts ...>::type;

			new (data) InitialType {std::forward<T>(value)};
			index = IndexOf<InitialType, Ts ...>::value;
		}

		Variant(Variant const & other)
			: index {other.index}
		{
			construct<Variant const &, Ts ...>(other);
		}

		Variant & operator = (Variant const & other)
		{
			destruct<Ts ...>();

			index = other.index;
			assign<Variant const &, Ts ...>(other);

			return *this;
		}

		Variant(Variant && other)
			: index {other.index}
		{
			construct<Variant, Ts ...>(std::move(other));
		}

		Variant & operator = (Variant && other)
		{
			destruct<Ts ...>();

			index = other.index;
			assign<Variant, Ts ...>(std::move(other));

			return *this;
		}

		~Variant()
		{
			destruct<Ts ...>();
		}

		template<typename Visitor>
		void visit(Visitor && visitor))
		{
			visit_impl<Visitor, Ts ...>(std::forward<Visitor>(visitor));
		}

		template<typename Visitor>
		void visit(Visitor && visitor) const
		{
			visit_impl<Visitor, Ts ...>(std::forward<Visitor>(visitor));
		}

	private:
		static std::size_t constexpr SIZE = Max<std::size_t, sizeof(Ts) ...>::value;
		static std::size_t constexpr ALIGNMENT = Max<std::size_t, alignof(Ts) ...>::value;

		template<typename T>
		bool has_value() const
		{
			return index == IndexOf<T, Ts ...>::value;
		}

		template<typename T, typename ... Us>
		static T && cast_data(Variant<Us ...> && variant)
		{
			auto const p = reinterpret_cast<T *>(variant.data);
			return std::move(*p);
		}

		template<typename T, typename ... Us>
		static T & cast_data(Variant<Us ...> & variant)
		{
			auto const p = reinterpret_cast<T *>(variant.data);
			return *p;
		}

		template<typename T, typename ... Us>
		static T const & cast_data(Variant<Us ...> const & variant)
		{
			auto const p = reinterpret_cast<T const *>(variant.data);
			return *p;
		}

		template<typename Other, typename T>
		void construct(Other && other)
		{
			new (data) T {
				cast_data<T>(
					std::forward<Other>(other)
				)
			};
		}

		template<typename Other, typename T1, typename T2, typename ... Tn>
		void construct(Other && other)
		{
			if (has_value<T1>())
			{
				construct<Other, T1>(std::forward<Other>(other));
			}
			else
			{
				construct<Other, T2, Tn ...>(std::forward<Other>(other));
			}
		}

		template<typename Other, typename T>
		void assign(Other && other)
		{
			cast_data<T>(*this) = cast_data<T>(std::forward<Other>(other));
		}

		template<typename Other, typename T1, typename T2, typename ... Tn>
		void assign(Other && other)
		{
			if (has_value<T1>())
			{
				assign<Other, T1>(std::forward<Other>(other));
			}
			else
			{
				assign<Other, T2, Tn ...>(std::forward<Other>(other));
			}
		}

		template<typename T>
		void destruct()
		{
			cast_data<T>(*this).~T();
		}

		template<typename T1, typename T2, typename ... Tn>
		void destruct()
		{
			if (has_value<T1>())
			{
				destruct<T1>();
			}
			else
			{
				destruct<T2, Tn ...>();
			}
		}

		template<typename Visitor, typename T>
		void visit_impl(Visitor && visitor)
		{
			std::forward<Visitor>(visitor)(cast_data<T>(*this));
		}

		template<typename Visitor, typename T1, typename T2, typename ... Tn>
		void visit_impl(Visitor && visitor)
		{
			if (has_value<T1>())
			{
				visit_impl<Visitor, T1>(std::forward<Visitor>(visitor));
			}
			else
			{
				visit_impl<Visitor, T2, Tn ...>(std::forward<Visitor>(visitor));
			}
		}

		template<typename Visitor, typename T>
		void visit_impl(Visitor && visitor) const
		{
			std::forward<Visitor>(visitor)(cast_data<T>(*this));
		}

		template<typename Visitor, typename T1, typename T2, typename ... Tn>
		void visit_impl(Visitor && visitor) const
		{
			if (has_value<T1>())
			{
				visit_impl<Visitor, T1>(std::forward<Visitor>(visitor));
			}
			else
			{
				visit_impl<Visitor, T2, Tn ...>(std::forward<Visitor>(visitor));
			}
		}

		alignas(ALIGNMENT) char data[SIZE];
		std::size_t index;
	};
}
