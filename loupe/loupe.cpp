// Copyright (c) 2022 Emilian Cioca
#include "loupe.h"

namespace loupe
{
	namespace detail
	{
		std::vector<void(*)(reflection_blob&)>& get_member_descriptor_tasks()
		{
			static std::vector<void(*)(reflection_blob&)> tasks;
			return tasks;
		}

		std::vector<void(*)(reflection_blob&)>& get_type_descriptor_tasks()
		{
			static std::vector<void(*)(reflection_blob&)> tasks;
			return tasks;
		}
	}

	reflection_blob reflect()
	{
		reflection_blob blob;

		for (auto* func : detail::get_type_descriptor_tasks())
		{
			func(blob);
		}

		for (auto* func : detail::get_member_descriptor_tasks())
		{
			func(blob);
		}

		return blob;
	}

	void clear_reflect_tasks()
	{
		detail::get_type_descriptor_tasks().clear();
		detail::get_member_descriptor_tasks().clear();
	}

	namespace detail
	{
		template<float> struct type_handler
		{

		};

		template<double> struct type_handler
		{

		};

		template<char> struct type_handler
		{

		};

		template<int> struct type_handler
		{

		};

		template<unsigned> struct type_handler
		{

		};
	}
}
