// Copyright (c) 2022 Emilian Cioca
#include "loupe.h"

#include <algorithm>

namespace loupe
{
	namespace detail
	{
		std::vector<task>& get_tasks()
		{
			static std::vector<task> tasks = []() {
				std::vector<task> storage;
				storage.reserve(128);
				return storage;
			}();

			return tasks;
		}
	}

	reflection_blob reflect()
	{
		reflection_blob blob;

		auto& tasks = detail::get_tasks();
		// Sorting by name allows for faster searching.
		std::sort(tasks.begin(), tasks.end(), [](const detail::task& left, const detail::task& right) {
			return left.name < right.name;
		});

#if _DEBUG
		// check for duplicate tasks.
		auto itr = std::adjacent_find(tasks.begin(), tasks.end(), [](const detail::task& left, const detail::task& right) {
			return left.name == right.name;
		});
		//assert(itr == tasks.end())
#endif

		blob.types.resize(tasks.size());
		unsigned j = 0;
		for (const detail::task& task : tasks)
		{
			blob.types[j].name = task.name;
			++j;
		}

		auto process_stage = [&](detail::task_stage stage) {
			unsigned i = 0;
			for (const detail::task& task : tasks)
			{
				task.func(blob, blob.types[i++], stage);
			}
		};

		process_stage(detail::task_stage::type_adapters);
		process_stage(detail::task_stage::enums_bases_members);

		return blob;
	}

	void clear_reflect_tasks()
	{
		detail::get_tasks().clear();
	}
}
