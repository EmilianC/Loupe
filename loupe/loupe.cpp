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

		std::vector<void(*)(reflection_blob&)>& get_type_descriptor_tasks()
		{
			static std::vector<void(*)(reflection_blob&)> tasks;
			return tasks;
		}

		std::vector<void(*)(reflection_blob&)>& get_member_descriptor_tasks()
		{
			static std::vector<void(*)(reflection_blob&)> tasks;
			return tasks;
		}
	}

	reflection_blob reflect()
	{
		reflection_blob blob;

		// Add entires for built-in-types
		//...


		auto& enum_tasks   = detail::get_enum_descriptor_tasks();
		auto& type_tasks   = detail::get_type_descriptor_tasks();
		auto& member_tasks = detail::get_member_descriptor_tasks();

		blob.enums.reserve(enum_tasks.size());
		for (auto* func : enum_tasks)
		{
			func(blob);
		}

		blob.types.reserve(type_tasks.size());
		blob.type_handlers.reserve(type_tasks.size());
		for (auto* func : type_tasks)
		{
			func(blob);
		}

		blob.members.reserve(member_tasks.size());
		for (auto* func : member_tasks)
		{
			func(blob);
		}

		return blob;
	}

	void clear_reflect_tasks()
	{
		detail::get_enum_descriptor_tasks().clear();
		detail::get_type_descriptor_tasks().clear();
		detail::get_member_descriptor_tasks().clear();
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
