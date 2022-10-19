// Copyright (c) 2022 Emilian Cioca
#include "loupe.h"

namespace loupe
{
	std::any type::construct() const
	{
		// assert(default_constructible)

		return construct_implementation();
	}

	void type::construct_at(void* location) const
	{
		// assert(default_constructible)
		// assert(reinterpret_cast<std::uintptr_t>(location) % alignment != 0)

		construct_at_implementation(location);
	}

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
