// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "assert.h"
#include "core_data.h"

#include <concepts>
#include <iterator>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <type_traits>

namespace loupe::adapters
{
	/// Pointers ///
	template<typename Type> struct pointer_adapter : public std::false_type {};

	template<typename Type>
	struct pointer_adapter<Type*> : public std::true_type
	{
		using TargetType = std::remove_cv_t<Type>;

		[[nodiscard]] static pointer make_data(const reflection_blob& blob)
		{
			const property* target_property = blob.find_property<TargetType>();
			LOUPE_ASSERT(target_property, "The pointer's target type was not registered. It must be reflected separately.");

			return {
				.target_property = target_property,
				.is_target_const = std::is_const_v<Type>,
				.get_target_address = [](void* pointer) { return pointer; }
			};
		}
	};

	template<typename Type>
	struct pointer_adapter<Type* const> : public pointer_adapter<Type*> {};

	template<typename Type>
	struct pointer_adapter<std::shared_ptr<Type>> : public std::true_type
	{
		using TargetType = std::remove_cv_t<Type>;

		[[nodiscard]] static pointer make_data(const reflection_blob& blob)
		{
			const property* target_property = blob.find_property<TargetType>();
			LOUPE_ASSERT(target_property, "The pointer's target type was not registered. It must be reflected separately.");

			return {
				.target_property = target_property,
				.is_target_const = std::is_const_v<Type>,
				.get_target_address = [](void* pointer) -> void* {
					return static_cast<std::shared_ptr<void>*>(pointer)->get();
				},
				.get_shared = [](void* pointer) -> std::shared_ptr<void> {
					return *static_cast<std::shared_ptr<void>*>(pointer);
				}
			};
		}
	};

	template<typename Type>
	struct pointer_adapter<std::weak_ptr<Type>> : public std::true_type
	{
		using TargetType = std::remove_cv_t<Type>;

		[[nodiscard]] static pointer make_data(const reflection_blob& blob)
		{
			const property* target_property = blob.find_property<TargetType>();
			LOUPE_ASSERT(target_property, "The pointer's target type was not registered. It must be reflected separately.");

			return {
				.target_property = target_property,
				.is_target_const = std::is_const_v<Type>,
				.get_target_address = [](void* pointer) -> void* {
					return static_cast<std::weak_ptr<void>*>(pointer)->lock().get();
				},
				.get_shared = [](void* pointer) -> std::shared_ptr<void> {
					return static_cast<std::weak_ptr<void>*>(pointer)->lock();
				}
			};
		}
	};

	template<typename Type>
	struct pointer_adapter<std::unique_ptr<Type>> : public std::true_type
	{
		using TargetType = std::remove_cv_t<Type>;

		[[nodiscard]] static pointer make_data(const reflection_blob& blob)
		{
			const property* target_property = blob.find_property<TargetType>();
			LOUPE_ASSERT(target_property, "The pointer's target type was not registered. It must be reflected separately.");

			return {
				.target_property = target_property,
				.is_target_const = std::is_const_v<Type>,
				.get_target_address = [](void* pointer) -> void* {
					return static_cast<std::unique_ptr<void>*>(pointer)->get();
				}
			};
		}
	};
	/// End Pointers ///


	/// Arrays ///
	template<typename Type> struct array_adapter : public std::false_type {};

	template<typename Type, std::size_t ElementCount>
	struct array_adapter<Type[ElementCount]> : public std::true_type
	{
		using ElementType = Type;
		using ArrayType = Type[ElementCount];

		[[nodiscard]] static array make_data(const reflection_blob& blob)
		{
			const property* element_property = blob.find_property<ElementType>();
			LOUPE_ASSERT(element_property, "The array's element type was not registered. It must be reflected separately.");

			bool (*contains)(const void*, const void*) = nullptr;
			if constexpr (std::equality_comparable<ElementType>)
			{
				contains = [](const void* array, const void* data) -> bool {
					auto* element = static_cast<const ElementType*>(data);
					auto* begin = static_cast<const ElementType*>(array);
					auto* end = begin + ElementCount;

					return std::find(begin, end, *element) != nullptr;
				};
			}

			return {
				.element_property = element_property,
				.dynamic = false,
				.unique_set = false,
				.get_count = [](const void*) { return ElementCount; },
				.contains = contains,
				.get_at = [](const void* array, std::size_t index) -> const void* {
					LOUPE_ASSERT(index < ElementCount, "Index was out of bounds.");

					auto* begin = static_cast<const ElementType*>(array);
					return begin + index;
				},

				.set_at = [](void* array, std::size_t index, const void* data) {
					LOUPE_ASSERT(index < ElementCount, "Index was out of bounds.");

					auto* begin = static_cast<ElementType*>(array);
					auto* element = static_cast<const ElementType*>(data);

					begin[index] = *element;
				}
			};
		}
	};

	template<typename Type>
	struct array_adapter<std::vector<Type>> : public std::true_type
	{
		using ElementType = Type;
		using ArrayType = std::vector<ElementType>;

		static_assert(!std::is_same_v<ElementType, bool>, "Loupe doesn't support reflecting \"std::vector<bool>\".");

		[[nodiscard]] static array make_data(const reflection_blob& blob)
		{
			const property* element_property = blob.find_property<ElementType>();
			LOUPE_ASSERT(element_property, "The array's element type was not registered. It must be reflected separately.");

			bool (*contains)(const void*, const void*) = nullptr;
			if constexpr (std::equality_comparable<ElementType>)
			{
				contains = [](const void* array, const void* data) -> bool {
					auto* vec = static_cast<const ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);

					return std::find(std::begin(*vec), std::end(*vec), *element) != std::end(*vec);
				};
			}

			return {
				.element_property = element_property,
				.dynamic = true,
				.unique_set = false,
				.get_count = [](const void* array) {
					return static_cast<const ArrayType*>(array)->size();
				},

				.contains = contains,

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

	template<typename Type, std::size_t ElementCount>
	struct array_adapter<std::array<Type, ElementCount>> : public std::true_type
	{
		using ArrayType = std::array<Type, ElementCount>;
		using ElementType = Type;

		[[nodiscard]] static array make_data(const reflection_blob& blob)
		{
			const property* element_property = blob.find_property<ElementType>();
			LOUPE_ASSERT(element_property, "The array's element type was not registered. It must be reflected separately.");

			bool (*contains)(const void*, const void*) = nullptr;
			if constexpr (std::equality_comparable<ElementType>)
			{
				contains = [](const void* array, const void* data) {
					auto* vec = static_cast<const ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					return std::find(std::begin(*vec), std::end(*vec), *element) != std::end(*vec);
				};
			}

			return {
				.element_property = element_property,
				.dynamic = false,
				.unique_set = false,
				.get_count = [](const void*) { return ElementCount; },

				.contains = contains,

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

		[[nodiscard]] static array make_data(const reflection_blob& blob)
		{
			const loupe::type* element_property = blob.find_property<ElementType>();
			LOUPE_ASSERT(element_property, "The array's element type was not registered. It must be reflected separately.");

			return {
				.element_property = element_property,
				.dynamic = true,
				.unique_set = false,
				.get_count = [](const void* array) {
					return static_cast<const ArrayType*>(array)->size();
				},

				.contains = [](const void* array, const void* data) {
					auto* list    = static_cast<const ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					return std::find(std::begin(list), std::end(list), *element) != std::end(list);
				},

				.get_at = [](const void* array, std::size_t index) -> const void* {
					auto* list = static_cast<const ArrayType*>(array);
					auto itr   = std::begin(list);
					itr = std::next(itr, index);

					return *itr;
				},

				.set_at = [](void* array, std::size_t index, const void* data) {
					auto* list    = static_cast<ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					auto itr      = std::begin(list);
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

	template<typename Type>
	struct array_adapter<std::set<Type>> : public std::true_type
	{
		using ElementType = Type;
		using ArrayType = std::vector<ElementType>;

		[[nodiscard]] static array make_data(const reflection_blob& blob)
		{
			const property* element_property = blob.find_property<ElementType>();
			LOUPE_ASSERT(element_property, "The array's element type was not registered. It must be reflected separately.");

			return {
				.element_property = element_property,
				.dynamic = true,
				.unique_set = true,
				.get_count = [](const void* array) {
					return static_cast<const ArrayType*>(array)->size();
				},

				.contains = [](const void* array, const void* data) -> bool {
					auto* set = static_cast<const ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);

					return set->contains(*element);
				},

				.emplace_back = [](void* array, const void* data) {
					auto* set = static_cast<ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					set->emplace(*element);
				}
			};
		}
	};

	template<typename Type>
	struct array_adapter<std::unordered_set<Type>> : public std::true_type
	{
		using ElementType = Type;
		using ArrayType = std::unordered_set<Type>;

		[[nodiscard]] static array make_data(const reflection_blob& blob)
		{
			const property* element_property = blob.find_property<ElementType>();
			LOUPE_ASSERT(element_property, "The array's element type was not registered. It must be reflected separately.");

			return {
				.element_property = element_property,
				.dynamic = true,
				.unique_set = true,
				.get_count = [](const void* array) {
					return static_cast<const ArrayType*>(array)->size();
				},

				.contains = [](const void* array, const void* data) -> bool {
					auto* set = static_cast<const ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);

					return set->contains(*element);
				},

				.emplace_back = [](void* array, const void* data) {
					auto* set = static_cast<ArrayType*>(array);
					auto* element = static_cast<const ElementType*>(data);
					set->emplace(*element);
				}
			};
		}
	};
	/// End Arrays ///


	/// Maps ///
	template<typename Type> struct map_adapter : public std::false_type {};

	template<typename Key, typename Value>
	struct map_adapter<std::map<Key, Value>> : public std::true_type
	{
		using KeyType = Key;
		using ValueType = Value;
		using MapType = std::map<Key, Value>;

		[[nodiscard]] static map make_data(const reflection_blob& blob)
		{
			const property* key_property   = blob.find_property<KeyType>();
			const property* value_property = blob.find_property<ValueType>();

			LOUPE_ASSERT(key_property,   "The map's key type was not registered. It must be reflected separately.");
			LOUPE_ASSERT(value_property, "The map's value type was not registered. It must be reflected separately.");

			return {
				.key_property = key_property,
				.value_property = value_property,

				.get_count = [](const void* map) -> std::size_t {
					auto* data = static_cast<MapType*>(map);
					return data->size();
				},

				.contains = [](const void* map, const void* key) -> bool {
					auto* map_ptr = static_cast<const MapType*>(map);
					auto* key_ptr = static_cast<const KeyType*>(key);
					return map_ptr->contains(*key_ptr);
				},

				.find = [](void* map, const void* key) -> void* {
					auto* map_ptr = static_cast<MapType*>(map);
					auto* key_ptr = static_cast<const KeyType*>(key);
					auto itr = map_ptr->find(*key_ptr);

					return itr == std::end(*map_ptr) ? nullptr : &(*itr);
				},

				.insert = [](void* map, const void* key, const void* value) {
					auto* map_ptr   = static_cast<MapType*>(map);
					auto* key_ptr   = static_cast<const KeyType*>(key);
					auto* value_ptr = static_cast<const ValueType*>(value);

					map_ptr->insert({ *key_ptr, *value_ptr });
				}
			};
		}
	};

	template<typename Key, typename Value>
	struct map_adapter<std::unordered_map<Key, Value>> : public std::true_type
	{
		using KeyType = Key;
		using ValueType = Value;
		using MapType = std::unordered_map<Key, Value>;

		[[nodiscard]] static map make_data(const reflection_blob& blob)
		{
			const property* key_property   = blob.find_property<KeyType>();
			const property* value_property = blob.find_property<ValueType>();

			LOUPE_ASSERT(key_property,   "The map's key type was not registered. It must be reflected separately.");
			LOUPE_ASSERT(value_property, "The map's value type was not registered. It must be reflected separately.");

			return {
				.key_property = key_property,
				.value_property = value_property,

				.get_count = [](const void* map) -> std::size_t {
					auto* data = static_cast<MapType*>(map);
					return data->size();
				},

				.contains = [](const void* map, const void* key) -> bool {
					auto* map_ptr = static_cast<const MapType*>(map);
					auto* key_ptr = static_cast<const KeyType*>(key);
					return map_ptr->contains(*key_ptr);
				},

				.find = [](void* map, const void* key) -> void* {
					auto* map_ptr = static_cast<MapType*>(map);
					auto* key_ptr = static_cast<const KeyType*>(key);
					auto itr = map_ptr->find(*key_ptr);

					return itr == std::end(*map_ptr) ? nullptr : &(*itr);
				},

				.insert = [](void* map, const void* key, const void* value) {
					auto* map_ptr   = static_cast<MapType*>(map);
					auto* key_ptr   = static_cast<const KeyType*>(key);
					auto* value_ptr = static_cast<const ValueType*>(value);

					map_ptr->insert({ *key_ptr, *value_ptr });
				}
			};
		}
	};
	/// End Maps ///


	/// Variants ///
	template<typename Type> struct variant_adapter : public std::false_type {};

	template<typename... Types>
	struct variant_adapter<std::variant<Types...>> : public std::true_type
	{
		using Tuple = std::tuple<Types...>;
		using VariantType = std::variant<Types...>;

		[[nodiscard]] static variant make_data(const reflection_blob& blob)
		{
			variant result;
			result.alternatives.reserve(sizeof...(Types));

			( [&] {
				const property* variant_property = blob.find_property<Types>();
				LOUPE_ASSERT(variant_property, "A variant's alternative type was not registered. It must be reflected separately.");

				result.alternatives.push_back(variant_property);
			}(), ...);

			result.get_index = [](const void* variant) -> std::size_t {
				auto* variant_ptr = static_cast<const VariantType*>(variant);
				return variant_ptr->index();
			};

			result.get_value = [](void* variant) -> void* {
				auto* variant_ptr = static_cast<VariantType*>(variant);

				void* result = nullptr;
				std::visit([&](auto&& arg) {
					result = &arg;
				}, *variant_ptr);

				return result;
			};

			return result;
		}
	};
	/// End Variants ///
}
