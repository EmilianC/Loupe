// Copyright (c) 2022 Emilian Cioca
#include "core_data.h"
#include "loupe.h"

namespace loupe
{
	bool structure::is_derived_from(const type& type) const
	{
		for (const struct type* base : bases)
		{
			if (&type == base)
				return true;

			if (std::get<structure>(base->data).is_derived_from(type))
				return true;
		}

		return false;
	}

	const member* structure::find_member(std::string_view var_name) const
	{
		for (const member& member : members)
		{
			if (member.name == var_name)
			{
				return &member;
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

		return nullptr;
	}

	bool type::is_a(const type& type) const
	{
		if (&type == this)
			return true;

		if (const structure* class_data = std::get_if<structure>(&data))
		{
			return class_data->is_derived_from(type);
		}

		return false;
	}

	const std::size_t* enumeration::find_value(std::string_view value_name) const
	{
		for (const enum_entry& entry : entries)
		{
			if (entry.name == value_name)
			{
				return &entry.value;
			}
		}

		return nullptr;
	}

	const std::string_view* enumeration::find_name(std::size_t value) const
	{
		for (const enum_entry& entry : entries)
		{
			if (entry.value == value)
			{
				return &entry.name;
			}
		}

		return nullptr;
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
}

/// Fundamental types ///
REFLECT(void)                    REF_END;
REFLECT(short int)               REF_END;
REFLECT(int)                     REF_END;
REFLECT(long int)                REF_END;
REFLECT(long long int)           REF_END;
REFLECT(unsigned short int)      REF_END;
REFLECT(unsigned int)            REF_END;
REFLECT(unsigned long int)       REF_END;
REFLECT(unsigned long long int)  REF_END;
REFLECT(char)                    REF_END;
REFLECT(unsigned char)           REF_END;
REFLECT(char8_t)                 REF_END;
REFLECT(char16_t)                REF_END;
REFLECT(char32_t)                REF_END;
REFLECT(wchar_t)                 REF_END;
REFLECT(bool)                    REF_END;
REFLECT(float)                   REF_END;
REFLECT(double)                  REF_END;
REFLECT(long double)             REF_END;
REFLECT(std::byte)               REF_END;

REFLECT(void*)                   REF_END;
REFLECT(short int*)              REF_END;
REFLECT(int*)                    REF_END;
REFLECT(long int*)               REF_END;
REFLECT(long long int*)          REF_END;
REFLECT(unsigned short int*)     REF_END;
REFLECT(unsigned int*)           REF_END;
REFLECT(unsigned long int*)      REF_END;
REFLECT(unsigned long long int*) REF_END;
REFLECT(char*)                   REF_END;
REFLECT(unsigned char*)          REF_END;
REFLECT(char8_t*)                REF_END;
REFLECT(char16_t*)               REF_END;
REFLECT(char32_t*)               REF_END;
REFLECT(wchar_t*)                REF_END;
REFLECT(bool*)                   REF_END;
REFLECT(float*)                  REF_END;
REFLECT(double*)                 REF_END;
REFLECT(long double*)            REF_END;
REFLECT(std::byte*)              REF_END;


/// Non-generic standard types ///
REFLECT(std::string)      REF_END;
REFLECT(std::string_view) REF_END;


/// Fundamental type arrays ///
REFLECT(std::vector<short int>)              REF_END;
REFLECT(std::vector<int>)                    REF_END;
REFLECT(std::vector<long int>)               REF_END;
REFLECT(std::vector<long long int>)          REF_END;
REFLECT(std::vector<unsigned short int>)     REF_END;
REFLECT(std::vector<unsigned int>)           REF_END;
REFLECT(std::vector<unsigned long int>)      REF_END;
REFLECT(std::vector<unsigned long long int>) REF_END;
REFLECT(std::vector<char>)                   REF_END;
REFLECT(std::vector<unsigned char>)          REF_END;
REFLECT(std::vector<char8_t>)                REF_END;
REFLECT(std::vector<char16_t>)               REF_END;
REFLECT(std::vector<char32_t>)               REF_END;
REFLECT(std::vector<wchar_t>)                REF_END;
REFLECT(std::vector<float>)                  REF_END;
REFLECT(std::vector<double>)                 REF_END;
REFLECT(std::vector<long double>)            REF_END;
REFLECT(std::vector<std::byte>)              REF_END;
REFLECT(std::vector<std::string>)            REF_END;


/// Reflect our own types ///
REFLECT(loupe::enum_entry) MEMBERS {
	REF_MEMBER(name),
	REF_MEMBER(value),
	REF_MEMBER(metadata)
} REF_END;

REFLECT(loupe::member) MEMBERS {
	REF_MEMBER(name),
	REF_MEMBER(offset),
	REF_MEMBER(type),
	REF_MEMBER(is_const),
	REF_MEMBER(metadata)
} REF_END;

REFLECT(loupe::static_member) MEMBERS {
	REF_MEMBER(name),
	REF_MEMBER(address),
	REF_MEMBER(type),
	REF_MEMBER(is_const),
	REF_MEMBER(metadata)
} REF_END;

REFLECT(loupe::structure) MEMBERS {
	REF_MEMBER(members),
	REF_MEMBER(static_members),
	REF_MEMBER(bases)
} REF_END;

REFLECT(loupe::enumeration) MEMBERS {
	REF_MEMBER(underlying_type),
	REF_MEMBER(entries)
} REF_END;

REFLECT(loupe::pointer) MEMBERS {
	REF_MEMBER(target_type)
} REF_END;

REFLECT(loupe::array) MEMBERS {
	REF_MEMBER(element_type),
	REF_MEMBER(dynamic)
} REF_END;

REFLECT(loupe::map) MEMBERS {
	REF_MEMBER(key_type),
	REF_MEMBER(value_type)
} REF_END;

REFLECT(loupe::variant) MEMBERS {
	REF_MEMBER(alternatives)
} REF_END;

REFLECT(loupe::fundamental) REF_END;

REFLECT(loupe::type*) REF_END;
REFLECT(loupe::type) MEMBERS {
	REF_MEMBER(name),
	REF_MEMBER(size),
	REF_MEMBER(alignment),
	REF_MEMBER(data)
} REF_END;

REFLECT(loupe::reflection_blob) MEMBERS {
	REF_MEMBER(types)
} REF_END;

REFLECT(std::vector<loupe::enum_entry>)      REF_END;
REFLECT(std::vector<loupe::member>)          REF_END;
REFLECT(std::vector<loupe::static_member>)   REF_END;
REFLECT(std::vector<loupe::type>)            REF_END;
REFLECT(std::vector<const loupe::type*>)     REF_END;
