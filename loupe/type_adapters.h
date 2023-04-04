// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "assert.h"
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

		[[nodiscard]] static array_type make_data(loupe::reflection_blob& blob)
		{
			const loupe::type* element_type = blob.find<ElementType>();
			LOUPE_ASSERT(element_type, "The array's element type was not registered. It must be reflected separately.");

			return {
				.element_type = element_type,
				.dynamic = false,
				.get_count = [](const void*) { return ElementCount; },
				.empty     = [](const void*) { return false; },

				.contains  = [](const void* array, const void* data) {
					auto* element = static_cast<const ElementType*>(data);
					auto* begin   = static_cast<const ElementType*>(array);
					auto* end     = begin + ElementCount;

					return std::find(begin, end, *element) != nullptr;
				},

				.get_at = [](const void* array, std::size_t index) -> const void* {
					LOUPE_ASSERT(index < ElementCount, "Index was out of bounds.");

					auto* begin = static_cast<const ElementType*>(array);
					return begin + index;
				},

				.set_at = [](void* array, std::size_t index, const void* data) {
					LOUPE_ASSERT(index < ElementCount, "Index was out of bounds.");

					auto* begin   = static_cast<ElementType*>(array);
					auto* element = static_cast<const ElementType*>(data);

					begin[index] = *element;
				}
			};
		}
	};

	template<typename ElementType>
	struct array_adapter<std::vector<ElementType>> : public std::true_type
	{
		using ArrayType = std::vector<ElementType>;
		static_assert(!std::is_same_v<ElementType, bool>, "Loupe doesn't support reflecting \"std::vector<bool>\".");

		[[nodiscard]] static array_type make_data(const reflection_blob& blob)
		{
			const loupe::type* element_type = blob.find<ElementType>();
			LOUPE_ASSERT(element_type, "The array's element type was not registered. It must be reflected separately.");

			return {
				.element_type = element_type,
				.dynamic = true,
				.get_count = [](const void* array) {
					return static_cast<const ArrayType*>(array)->size();
				},

				.empty = [](const void* array) {
					return static_cast<const ArrayType*>(array)->empty();
				},

				.contains = [](const void* array, const void* data) {
					if constexpr (std::equality_comparable<ElementType>)
					{
						auto* vec     = static_cast<const ArrayType*>(array);
						auto* element = static_cast<const ElementType*>(data);
						return std::find(vec->begin(), vec->end(), *element) != vec->end();
					}
					else
					{
						LOUPE_ASSERT(false, "contains() could not be executed for the array. The elements are not comparable.");
						return false;
					}
				},

				.get_at = [](const void* array, std::size_t index) -> const void* {
					auto* vec = static_cast<const ArrayType*>(array);

					return &(*vec)[index];
				},

				.set_at = [](void* array, std::size_t index, const void* data) {
					auto* vec     = static_cast<ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);

					(*vec)[index] = *element;
				},

				.emplace_back = [](void* array, const void* data) {
					auto* vec     = static_cast<ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					vec->emplace_back(*element);
				}
			};
		}
	};

	template<typename ElementType, std::size_t ElementCount>
	struct array_adapter<std::array<ElementType, ElementCount>> : public std::true_type
	{
		using ArrayType = std::array<ElementType, ElementCount>;

		[[nodiscard]] static array_type make_data(const reflection_blob& blob)
		{
			const loupe::type* element_type = blob.find<ElementType>();
			LOUPE_ASSERT(element_type, "The array's element type was not registered. It must be reflected separately.");

			return {
				.element_type = element_type,
				.dynamic = false
				.get_count = [](const void*) { return ElementCount; },
				.empty     = [](const void*) { return false; },

				.contains = [](const void* array, const void* data) {
					auto* vec     = static_cast<const ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					return std::find(vec->begin(), vec->end(), *element) != vec->end();
				},

				.get_at = [](const void* array, std::size_t index) -> const void* {
					auto* vec = static_cast<const ArrayType*>(array);
					return &(*vec)[index];
				},

				.set_at = [](void* array, std::size_t index, const void* data) {
					auto* vec     = static_cast<ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);

					(*vec)[index] = *element;
				}
			};
		}
	};

	template<typename ElementType>
	struct array_adapter<std::list<ElementType>> : public std::true_type
	{
		using ArrayType = std::list<ElementType>;

		[[nodiscard]] static array_type make_data(const reflection_blob& blob)
		{
			const loupe::type* element_type = blob.find<ElementType>();
			LOUPE_ASSERT(element_type, "The array's element type was not registered. It must be reflected separately.");

			return {
				.element_type = element_type,
				.dynamic = true
				.get_count = [](const void* array) {
					return static_cast<const ArrayType*>(array)->size();
				},

				.empty = [](const void* array) {
					return static_cast<const ArrayType*>(array)->empty();
				},

				.contains = [](const void* array, const void* data) {
					auto* list    = static_cast<const ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					return std::find(list->begin(), list->end(), *element) != list->end();
				},

				.get_at = [](const void* array, std::size_t index) -> const void* {
					auto* list = static_cast<const ArrayType*>(array);
					auto itr   = list->begin();
					itr = std::next(itr, index);

					return *itr;
				},

				.set_at = [](void* array, std::size_t index, const void* data) {
					auto* list    = static_cast<ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					auto itr      = list->begin();
					itr = std::next(itr, index);

					*itr = *element;
				},

				.emplace_back = [](void* array, const void* data) {
					auto* list    = static_cast<ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					list->emplace_back(*element);
				}
			};
		}
	};

	template<>
	struct array_adapter<std::string> : public std::true_type
	{
		using ArrayType = std::string;
		using ElementType = char;

		[[nodiscard]] static array_type make_data(const reflection_blob& blob)
		{
			const loupe::type* element_type = blob.find<ElementType>();
			LOUPE_ASSERT(element_type, "The array's element type was not registered. It must be reflected separately.");

			return {
				.element_type = element_type,
				.dynamic = true,
				.get_count = [](const void* array) {
					return static_cast<const ArrayType*>(array)->size();
				},

				.empty = [](const void* array) {
					return static_cast<const ArrayType*>(array)->empty();
				},

				.contains = [](const void* array, const void* data) {
					auto* string  = static_cast<const ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					return std::find(string->begin(), string->end(), *element) != string->end();
				},

				.get_at = [](const void* array, std::size_t index) -> const void* {
					auto* string = static_cast<const ArrayType*>(array);
					return &(*string)[index];
				},

				.set_at = [](void* array, std::size_t index, const void* data) {
					auto* string  = static_cast<ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);

					(*string)[index] = *element;
				},

				.emplace_back = [](void* array, const void* data) {
					auto* string  = static_cast<ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					string->push_back(*element);
				}
			};
		}
	};

	template<>
	struct array_adapter<std::string_view> : public std::true_type
	{
		using ArrayType = std::string_view;
		using ElementType = char;

		[[nodiscard]] static array_type make_data(const reflection_blob& blob)
		{
			const loupe::type* element_type = blob.find<ElementType>();
			LOUPE_ASSERT(element_type, "The array's element type was not registered. It must be reflected separately.");

			return {
				.element_type = element_type,
				.dynamic = false,
				.get_count = [](const void* array) {
					return static_cast<const ArrayType*>(array)->size();
				},

				.empty = [](const void* array) {
					return static_cast<const ArrayType*>(array)->empty();
				},

				.contains = [](const void* array, const void* data) {
					auto* string  = static_cast<const ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					return std::find(string->begin(), string->end(), *element) != string->end();
				},

				.get_at = [](const void* array, std::size_t index) -> const void* {
					auto* string = static_cast<const ArrayType*>(array);
					LOUPE_ASSERT(index < string->size(), "The index provided to get_at() was out of bounds.");

					return string->data() + index;
				}
			};
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
				LOUPE_ASSERT(underlying_type, "The enum's underlying type was not registered. It must be reflected separately.");

				return { .underlying_type = underlying_type };
			}
		};
	}

	template<typename Type>
	struct enum_adapter : public detail::enum_adapter_impl<std::is_enum_v<Type>, Type> {};
	/// End Enums ///
}
