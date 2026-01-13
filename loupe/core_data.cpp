// Copyright (c) 2022 Emilian Cioca
#include "core_data.h"
#include "loupe.h"
#include "metadata.h"

#include <string>

namespace loupe
{
	bool property::operator==(const property& other) const
	{
		return signature == other.signature;
	}

	bool property::operator!=(const property& other) const
	{
		return signature != other.signature;
	}

	bool structure::is_derived_from(const type& type) const
	{
		for (const base& base : bases)
		{
			if (base.type == &type)
				return true;
		}

		for (const base& base : bases)
		{
			if (std::get<structure>(base.type->data).is_derived_from(type))
				return true;
		}

		return false;
	}

	const member* structure::find_member(std::string_view name) const
	{
		for (const member& member : members)
		{
			if (member.name == name)
			{
				return &member;
			}
		}

		for (const base& base : bases)
		{
			if (const member* member = std::get<loupe::structure>(base.type->data).find_member(name))
			{
				return member;
			}
		}

		return nullptr;
	}

	const member* structure::find_member(std::size_t offset) const
	{
		for (const member& member : members)
		{
			if (member.offset == offset)
			{
				return &member;
			}
		}

		for (const base& base : bases)
		{
			if (const member* member = std::get<loupe::structure>(base.type->data).find_member(offset))
			{
				return member;
			}
		}

		return nullptr;
	}

	bool type::is_a(const type& type) const
	{
		if (&type == this)
			return true;

		if (const structure* struct_data = std::get_if<structure>(&data))
		{
			return struct_data->is_derived_from(type);
		}

		return false;
	}

	const std::uint16_t* enumeration::find_value(std::string_view name) const
	{
		if (const enum_entry* entry = find_entry(name))
		{
			return &entry->value;
		}

		return nullptr;
	}

	const std::string_view* enumeration::find_name(std::uint16_t value) const
	{
		if (const enum_entry* entry = find_entry(value))
		{
			return &entry->name;
		}

		return nullptr;
	}

	const enum_entry* enumeration::find_entry(std::string_view name) const
	{
		for (const enum_entry& entry : entries)
		{
			if (entry.name == name)
			{
				return &entry;
			}
		}

		return nullptr;
	}

	const enum_entry* enumeration::find_entry(std::uint16_t value) const
	{
		for (const enum_entry& entry : entries)
		{
			if (entry.value == value)
			{
				return &entry;
			}
		}

		return nullptr;
	}

	unsigned int reflection_blob::get_version() const
	{
		return version;
	}

	const type* reflection_blob::find(std::string_view name) const
	{
		auto itr = std::lower_bound(types.begin(), types.end(), name, [](const type& type, std::string_view name) {
			return type.name < name;
		});

		if (itr != types.end() && itr->name == name)
		{
			return &(*itr);
		}

		return nullptr;
	}

	const property* reflection_blob::find_property(std::string_view signature) const
	{
		auto itr = std::lower_bound(properties.begin(), properties.end(), signature, [](const property& property, std::string_view signature) {
			return property.signature < signature;
		});

		if (itr != properties.end() && itr->signature == signature)
		{
			return &(*itr);
		}

		return nullptr;
	}

	std::span<const type> reflection_blob::get_types() const
	{
		return types;
	}

	std::span<const property> reflection_blob::get_properties() const
	{
		return properties;
	}

	const type* find_common_ancestor(const type& left, const type& right)
	{
		if (&left == &right)
			return &left;

		auto* left_struct  = std::get_if<structure>(&left.data);
		auto* right_struct = std::get_if<structure>(&right.data);

		if (!left_struct || !right_struct)
			return nullptr;

		auto get_base = [](const type& child) -> const type* {
			auto& structure = std::get<loupe::structure>(child.data);

			if (structure.bases.empty())
				return nullptr;

			LOUPE_ASSERT(structure.bases.size() == 1, "find_common_ancestor() does not support multiple-inheritance.");
			return structure.bases[0].type;
		};

		auto count_depth = [&](const type& node) -> int {
			const type* current_node = &node;
			int count = -1;

			do
			{
				++count;
				current_node = get_base(*current_node);
			} while (current_node);

			return count;
		};

		int left_depth  = count_depth(left);
		int right_depth = count_depth(right);

		const type* current_left_node  = &left;
		const type* current_right_node = &right;

		// Match the depths we are checking at first.
		while (left_depth > right_depth)
		{
			current_left_node = get_base(*current_left_node);
			--left_depth;
		}

		while (right_depth > left_depth)
		{
			current_right_node = get_base(*current_right_node);
			--right_depth;
		}

		// With depths equal, we can start checking where the paths converge.
		int current_depth = left_depth;
		while (current_depth >= 0)
		{
			if (current_left_node == current_right_node)
				return current_left_node;

			current_left_node = get_base(*current_left_node);
			current_right_node = get_base(*current_right_node);
			--current_depth;
		}

		return nullptr;
	}
}

/// Fundamental types ///
REFLECT_SIMPLE(void);
REFLECT_SIMPLE(short int);
REFLECT_SIMPLE(int);
REFLECT_SIMPLE(long int);
REFLECT_SIMPLE(long long int);
REFLECT_SIMPLE(unsigned short int);
REFLECT_SIMPLE(unsigned int);
REFLECT_SIMPLE(unsigned long int);
REFLECT_SIMPLE(unsigned long long int);
REFLECT_SIMPLE(char);
REFLECT_SIMPLE(unsigned char);
REFLECT_SIMPLE(char8_t);
REFLECT_SIMPLE(char16_t);
REFLECT_SIMPLE(char32_t);
REFLECT_SIMPLE(wchar_t);
REFLECT_SIMPLE(bool);
REFLECT_SIMPLE(float);
REFLECT_SIMPLE(double);
REFLECT_SIMPLE(long double);

/// Standard Types ///
REFLECT_SIMPLE(std::byte);
REFLECT_SIMPLE(std::monostate);
REFLECT_SIMPLE(std::string);
REFLECT_SIMPLE(std::string_view);

/// Reflect our own types ///
REFLECT_SIMPLE(loupe::metadata::hidden);
REFLECT_SIMPLE(loupe::metadata::readonly);
REFLECT_SIMPLE(loupe::metadata::not_serialized);
REFLECT_SIMPLE(loupe::metadata::description);
REFLECT_SIMPLE(loupe::metadata::display_name);
REFLECT_SIMPLE(loupe::metadata::category);
REFLECT_SIMPLE(loupe::metadata::previously_serialized_as);

REFLECT_SIMPLE(loupe::metadata::range<short int>);
REFLECT_SIMPLE(loupe::metadata::range<int>);
REFLECT_SIMPLE(loupe::metadata::range<long int>);
REFLECT_SIMPLE(loupe::metadata::range<unsigned short int>);
REFLECT_SIMPLE(loupe::metadata::range<unsigned int>);
REFLECT_SIMPLE(loupe::metadata::range<unsigned long int>);
REFLECT_SIMPLE(loupe::metadata::range<float>);
REFLECT_SIMPLE(loupe::metadata::range<double>);

REFLECT(loupe::enum_entry) MEMBERS {
	REF_MEMBER(name)
	REF_MEMBER(value)
	REF_MEMBER(metadata)
} REF_END;

REFLECT(loupe::member) MEMBERS {
	REF_MEMBER(name)
	REF_MEMBER(offset)
	REF_MEMBER(data)
	REF_MEMBER(metadata)
} REF_END;

REFLECT(loupe::base) MEMBERS {
	REF_MEMBER(type)
	REF_MEMBER(offset)
} REF_END;

REFLECT(loupe::structure) MEMBERS {
	REF_MEMBER(members)
	REF_MEMBER(bases)
} REF_END;

REFLECT(loupe::enumeration) MEMBERS {
	REF_MEMBER(entries)
} REF_END;

REFLECT(loupe::pointer) MEMBERS {
	REF_MEMBER(target_property)
	REF_MEMBER(is_target_const)
} REF_END;

REFLECT(loupe::array) MEMBERS {
	REF_MEMBER(element_property)
	REF_MEMBER(dynamic)
} REF_END;

REFLECT(loupe::map) MEMBERS {
	REF_MEMBER(key_property)
	REF_MEMBER(value_property)
} REF_END;

REFLECT(loupe::variant) MEMBERS {
	REF_MEMBER(alternatives)
} REF_END;

REFLECT(loupe::property) MEMBERS {
	REF_MEMBER(signature)
	REF_MEMBER(data)
} REF_END;

REFLECT(loupe::metadata_container::entry) MEMBERS {
	REF_MEMBER(type)
} REF_END;

REFLECT(loupe::metadata_container) MEMBERS {
	REF_MEMBER(entries)
} REF_END;

REFLECT_SIMPLE(loupe::fundamental);

REFLECT(loupe::type) MEMBERS {
	REF_MEMBER(name)
	REF_MEMBER(size)
	REF_MEMBER(alignment)
	REF_MEMBER(data)
} REF_END;

REFLECT(loupe::reflection_blob) MEMBERS {
	REF_MEMBER(version)
	REF_MEMBER(types)
	REF_MEMBER(properties)
} REF_END;
