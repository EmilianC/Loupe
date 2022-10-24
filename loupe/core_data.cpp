// Copyright (c) 2022 Emilian Cioca
#include "core_data.h"
#include "loupe.h"

namespace loupe
{
	bool class_type::is_derived_from(const type& type) const
	{
		for (const struct type* base : bases)
		{
			if (&type == base)
				return true;

			if (std::get<class_type>(base->data).is_derived_from(type))
				return true;
		}

		return false;
	}

	const variable* class_type::find_variable(std::string_view var_name) const
	{
		for (const variable& variable : variables)
		{
			if (variable.name == var_name)
			{
				return &variable;
			}
		}

		return nullptr;
	}

	const variable* class_type::find_variable(std::size_t offset) const
	{
		for (const variable& variable : variables)
		{
			if (variable.offset == offset)
			{
				return &variable;
			}
		}

		return nullptr;
	}

	bool type::is_a(const type& type) const
	{
		if (&type == this)
			return true;

		if (const class_type* class_data = std::get_if<class_type>(&data))
		{
			return class_data->is_derived_from(type);
		}

		return false;
	}

	const std::size_t* enum_type::find_enum_value(std::string_view value_name) const
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

	const std::string_view* enum_type::find_enum_name(std::size_t value) const
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
REFLECT(void)                   REF_END;
REFLECT(short int)              REF_END;
REFLECT(int)                    REF_END;
REFLECT(long int)               REF_END;
REFLECT(long long int)          REF_END;
REFLECT(unsigned short int)     REF_END;
REFLECT(unsigned int)           REF_END;
REFLECT(unsigned long int)      REF_END;
REFLECT(unsigned long long int) REF_END;
REFLECT(char)                   REF_END;
REFLECT(unsigned char)          REF_END;
REFLECT(char8_t)                REF_END;
REFLECT(char16_t)               REF_END;
REFLECT(char32_t)               REF_END;
REFLECT(wchar_t)                REF_END;
REFLECT(bool)                   REF_END;
REFLECT(float)                  REF_END;
REFLECT(double)                 REF_END;
REFLECT(long double)            REF_END;
REFLECT(std::byte)              REF_END;


/// Non-generic standard types ///
REFLECT(std::string)      REF_END;
REFLECT(std::string_view) REF_END;


/// Reflect our own types ///
REFLECT(std::vector<loupe::enum_entry>) REF_END;
REFLECT(loupe::enum_entry) MEMBERS {
	REF_MEMBER(name),
	REF_MEMBER(value),
	REF_MEMBER(metadata)
} REF_END;

REFLECT(std::vector<loupe::variable>) REF_END;
REFLECT(loupe::variable) MEMBERS {
	REF_MEMBER(name),
	REF_MEMBER(offset),
	REF_MEMBER(type),
	REF_MEMBER(is_const),
	REF_MEMBER(metadata)
} REF_END;

REFLECT(std::vector<loupe::static_variable>) REF_END;
REFLECT(loupe::static_variable) MEMBERS {
	REF_MEMBER(name),
	REF_MEMBER(address),
	REF_MEMBER(type),
	REF_MEMBER(is_const),
	REF_MEMBER(metadata)
} REF_END;

REFLECT(loupe::class_type) MEMBERS {
	REF_MEMBER(variables),
	REF_MEMBER(static_variables),
	REF_MEMBER(bases)
} REF_END;

REFLECT(loupe::enum_type) MEMBERS {
	REF_MEMBER(underlying_type),
	REF_MEMBER(entries)
} REF_END;

REFLECT(loupe::pointer_type) MEMBERS {
	REF_MEMBER(target_type),
	REF_MEMBER(is_target_const)
} REF_END;

REFLECT(loupe::array_type) MEMBERS {
	REF_MEMBER(element_type),
	REF_MEMBER(dynamic)
} REF_END;

REFLECT(loupe::map_type) MEMBERS {
	REF_MEMBER(key_type),
	REF_MEMBER(value_type)
} REF_END;

REFLECT(loupe::variant_type) MEMBERS {
	REF_MEMBER(alternatives)
} REF_END;

REFLECT(loupe::fundamental_type) REF_END;

REFLECT(std::vector<loupe::type>) REF_END;
REFLECT(std::vector<const loupe::type*>) REF_END;
REFLECT(loupe::type) MEMBERS {
	REF_MEMBER(name),
	REF_MEMBER(size),
	REF_MEMBER(alignment),
	REF_MEMBER(data)
} REF_END;

REFLECT(loupe::reflection_blob) MEMBERS {
	REF_MEMBER(types)
} REF_END;
