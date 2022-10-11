// Copyright (c) 2022 Emilian Cioca
#include "loupe.h"

namespace loupe
{
	namespace detail
	{
		std::vector<void(*)(reflection_blob&)>& get_enum_descriptor_tasks()
		{
			static std::vector<void(*)(reflection_blob&)> tasks;
			return tasks;
		}

		std::vector<void(*)(reflection_blob&, type_descriptor&, stage)>& get_type_descriptor_tasks()
		{
			static std::vector<void(*)(reflection_blob&, type_descriptor&, stage)> tasks;
			return tasks;
		}
	}

	const std::size_t* enum_descriptor::find_value_from_name(std::string_view entry_name) const
	{
		for (const enum_entry& entry : entires)
		{
			if (entry.name == entry_name)
			{
				return &entry.value;
			}
		}

		return nullptr;
	}

	const std::string_view* enum_descriptor::find_name_from_value(std::size_t value) const
	{
		for (const enum_entry& entry : entires)
		{
			if (entry.value == value)
			{
				return &entry.name;
			}
		}

		return nullptr;
	}

	std::any type_descriptor::construct() const
	{
		if (!default_constructible)
			return {};

		return construct_implementation();
	}

	void type_descriptor::construct_at(void* location) const
	{
		if (!default_constructible)
			return;

		if (reinterpret_cast<std::uintptr_t>(location) % alignment != 0)
			return;

		construct_at_implementation(location);
	}

	const member_descriptor* type_descriptor::find_member(std::string_view member_name) const
	{
		for (const member_descriptor& member : members)
		{
			if (member.name == member_name)
			{
				return &member;
			}
		}

		return nullptr;
	}

	const member_descriptor* type_descriptor::find_member(std::size_t offset) const
	{
		for (const member_descriptor& member : members)
		{
			if (member.offset == offset)
			{
				return &member;
			}
		}

		return nullptr;
	}

	const member_descriptor* type_descriptor::find_first_of(const type_descriptor& member_type) const
	{
		for (const member_descriptor& member : members)
		{
			if (member.type == &member_type)
			{
				return &member;
			}
		}

		return nullptr;
	}

	const member_descriptor* type_descriptor::find_first_of(const enum_descriptor& type) const
	{
		for (const member_descriptor& member : members)
		{
			if (member.enum_type == &type)
			{
				return &member;
			}
		}

		return nullptr;
	}


	bool type_descriptor::is_a(const type_descriptor* type) const
	{
		if (type == this)
			return true;

		for (const type_descriptor* base : bases)
		{
			if (base->is_a(type))
				return true;
		}

		return false;
	}

	reflection_blob reflect()
	{
		reflection_blob blob;

		auto& enum_tasks = detail::get_enum_descriptor_tasks();
		auto& type_tasks = detail::get_type_descriptor_tasks();

		blob.enums.reserve(enum_tasks.size());
		blob.types.resize(type_tasks.size());

		unsigned i = 0;
		for (auto* func : type_tasks)
		{
			func(blob, blob.types[i++], detail::stage::types);
		}

		for (auto* func : enum_tasks)
		{
			func(blob);
		}

		i = 0;
		for (auto* func : type_tasks)
		{
			func(blob, blob.types[i++], detail::stage::bases_and_members);
		}

		return blob;
	}

	void clear_reflect_tasks()
	{
		detail::get_enum_descriptor_tasks().clear();
		detail::get_type_descriptor_tasks().clear();
	}


	const type_descriptor* reflection_blob::find_type(std::string_view name) const
	{
		for (const type_descriptor& type_desc : types)
		{
			if (name == type_desc.name)
			{
				return &type_desc;
			}
		}

		return nullptr;
	}

	const enum_descriptor* reflection_blob::find_enum(std::string_view name) const
	{
		for (const enum_descriptor& enum_desc : enums)
		{
			if (name == enum_desc.name)
			{
				return &enum_desc;
			}
		}

		return nullptr;
	}
}

REFLECT(void)             REF_END;
REFLECT(short int)        REF_END;
REFLECT(int)              REF_END;
REFLECT(unsigned int)     REF_END;
REFLECT(long)             REF_END;
REFLECT(char)             REF_END;
REFLECT(unsigned char)    REF_END;
REFLECT(bool)             REF_END;
REFLECT(float)            REF_END;
REFLECT(double)           REF_END;
REFLECT(std::byte)        REF_END;
REFLECT(std::string)      REF_END;
REFLECT(std::string_view) REF_END;

