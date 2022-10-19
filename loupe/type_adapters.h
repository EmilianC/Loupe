// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "core_data.h"

#include <concepts>
#include <iterator>
#include <list>
#include <type_traits>

namespace loupe
{
	/// Arrays ///
	template<typename Type>
	struct array_adapter : public std::false_type { static array_type make_data(loupe::reflection_blob&) { return {}; } };

	template<typename Type, std::size_t ElementCount>
	struct array_adapter<Type[ElementCount]> : public std::true_type
	{
		using ArrayType = Type[ElementCount];
		using ElementType = Type;

		static array_type make_data(loupe::reflection_blob& blob)
		{
			const loupe::type* element_type = blob.find<ElementType>();
			// assert(element_type)

			return {
				.element_type = element_type,
				.dynamic = false,
				.get_count_implementation = &get_count,
				.empty_implementation = &empty,
				.contains_implementation = &contains,
				.get_at_implementation = &get_at,
				.set_at_implementation = &set_at,
			};
		}

		static std::size_t get_count(const void*)
		{
			return ElementCount;
		}

		static bool empty(const void*)
		{
			return false;
		}

		static bool contains(const void* array, const void* data)
		{
			auto* element = static_cast<const ElementType*>(data);
			auto* begin   = static_cast<const ElementType*>(array);
			auto* end     = begin + ElementCount;

			return std::find(begin, end, *element) != nullptr;
		}

		static void* get_at(void* array, std::size_t index)
		{
			// assert (index < ElementCount)

			auto* begin = static_cast<ElementType*>(array);
			return begin[index];
		}

		static void set_at(void* array, std::size_t index, const void* data)
		{
			// assert (index < ElementCount)

			auto* begin   = static_cast<ElementType*>(array);
			auto* element = static_cast<const ElementType*>(data);

			begin[index] = *element;
		}
	};

	template<typename ElementType>
	struct array_adapter<std::vector<ElementType>> : public std::true_type
	{
		using ArrayType = std::vector<ElementType>;

		static array_type make_data(const reflection_blob& blob)
		{
			const loupe::type* element_type = blob.find<ElementType>();
			// assert(element_type)
			return {
				.element_type = element_type,
				.dynamic = true,
				.get_count_implementation = &get_count,
				.empty_implementation = &empty,
				.contains_implementation = &contains,
				.get_at_implementation = &get_at,
				.set_at_implementation = &set_at,
				.emplace_back_implementation = &emplace_back
			};
		}

		static std::size_t get_count(const void* array)
		{
			return static_cast<const ArrayType*>(array)->size();
		}

		static bool empty(const void* array)
		{
			return static_cast<const ArrayType*>(array)->empty();
		}

		static bool contains(const void* array, const void* data)
		{
			if constexpr (std::equality_comparable<ElementType>)
			{
				auto* vec     = static_cast<const ArrayType*>(array);
				auto* element = static_cast<const ElementType*>(data);
				return std::find(vec->begin(), vec->end(), *element) != vec->end();
			}
			else
			{
				// assert(false)
				return true;
			}
		}

		static void* get_at(void* array, std::size_t index)
		{
			auto* vec = static_cast<ArrayType*>(array);

			return &(*vec)[index];
		}

		static void set_at(void* array, std::size_t index, const void* data)
		{
			auto* vec     = static_cast<ArrayType*>(array);
			auto* element = static_cast<const ElementType*>(data);
			
			(*vec)[index] = *element;
		}

		static void* emplace_back(void* array, const void* data)
		{
			auto* vec     = static_cast<ArrayType*>(array);
			auto* element = static_cast<const ElementType*>(data);
			return &vec->emplace_back(*element);
		}
	};

	template<typename ElementType, std::size_t ElementCount>
	struct array_adapter<std::array<ElementType, ElementCount>> : public std::true_type
	{
		using ArrayType = std::array<ElementType, ElementCount>;

		static array_type make_data(const reflection_blob& blob)
		{
			const loupe::type* element_type = blob.find<ElementType>();
			// assert(element_type)
			return {
				.element_type = element_type,
				.dynamic = false
				.get_count_implementation = &get_count,
				.empty_implementation = &empty,
				.contains_implementation = &contains,
				.get_at_implementation = &get_at,
				.set_at_implementation = &set_at
			};
		}

		static std::size_t get_count(const void*)
		{
			return ElementCount;
		}

		static bool empty(const void*)
		{
			return false;
		}

		static bool contains(const void* array, const void* data)
		{
			auto* vec     = static_cast<const ArrayType*>(array);
			auto* element = static_cast<const ElementType*>(data);
			return std::find(vec->begin(), vec->end(), *element) != vec->end();
		}

		static void* get_at(void* array, std::size_t index)
		{
			auto* vec = static_cast<ArrayType*>(array);
			return &(*vec)[index];
		}

		static void set_at(void* array, std::size_t index, const void* data)
		{
			auto* vec     = static_cast<ArrayType*>(array);
			auto* element = static_cast<const ElementType*>(data);

			(*vec)[index] = *element;
		}
	};

	template<typename ElementType>
	struct array_adapter<std::list<ElementType>> : public std::true_type
	{
		using ArrayType = std::list<ElementType>;

		static array_type make_data(const reflection_blob& blob)
		{
			const loupe::type* element_type = blob.find<ElementType>();
			// assert(element_type)
			return {
				.element_type = element_type,
				.dynamic = true
				.get_count_implementation = &get_count,
				.empty_implementation = &empty,
				.contains_implementation = &contains,
				.get_at_implementation = &get_at,
				.set_at_implementation = &set_at,
				.emplace_back_implementation = &emplace_back
			};
		}

		static std::size_t get_count(const void* array)
		{
			return static_cast<const ArrayType*>(array)->size();
		}

		static bool empty(const void* array)
		{
			return static_cast<const ArrayType*>(array)->empty();
		}

		static bool contains(const void* array, const void* data)
		{
			auto* list    = static_cast<const ArrayType*>(array);
			auto* element = static_cast<const ElementType*>(data);
			return std::find(list->begin(), list->end(), *element) != list->end();
		}

		static void* get_at(void* array, std::size_t index)
		{
			auto* list = static_cast<ArrayType*>(array);
			auto itr   = list->begin();
			itr = std::next(itr, index);

			return *itr;
		}

		static void set_at(void* array, std::size_t index, const void* data)
		{
			auto* list    = static_cast<ArrayType*>(array);
			auto* element = static_cast<const ElementType*>(data);
			auto itr      = list->begin();
			itr = std::next(itr, index);

			*itr = *element;
		}

		static void* emplace_back(void* array, const void* data)
		{
			auto* list    = static_cast<ArrayType*>(array);
			auto* element = static_cast<const ElementType*>(data);
			return &list->emplace_back(*element);
		}
	};
	/// End Arrays ///


	/// Enums ///
	namespace detail
	{
		template<bool IsEnum, typename Type>
		struct enum_adapter_impl : public std::false_type { static enum_type make_data(loupe::reflection_blob&) { return {}; } };

		template<typename EnumType>
		struct enum_adapter_impl<true, EnumType> : public std::true_type
		{
			using UnderlyingType = std::underlying_type_t<EnumType>;

			static enum_type make_data(const reflection_blob& blob)
			{
				const loupe::type* underlying_type = blob.find<UnderlyingType>();
				// assert(underlying_type)
				return { .underlying_type = underlying_type };
			}
		};
	}

	template<typename Type>
	struct enum_adapter : public detail::enum_adapter_impl<std::is_enum_v<Type>, Type> {};
	/// End Enums ///
}
