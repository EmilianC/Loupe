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
				storage.reserve(256);
				return storage;
			}();

			return tasks;
		}
	}

	reflection_blob reflect()
	{
		reflection_blob blob;

		auto& tasks = detail::get_tasks();
		// The final array of types needs to be sorted to allow for faster lookups.
		// It is easier to just sort the tasks by name from the start.
		std::sort(tasks.begin(), tasks.end(), [](const detail::task& left, const detail::task& right) {
			return left.name < right.name;
		});

#if _DEBUG
		// check for duplicate tasks.
		auto itr = std::adjacent_find(tasks.begin(), tasks.end(), [](const detail::task& left, const detail::task& right) {
			return left.name == right.name;
		});
		LOUPE_ASSERT(itr == tasks.end(), "A type has been reflected multiple times.");
#endif

		blob.types.resize(tasks.size());
		for (unsigned i = 0; const detail::task& task : tasks)
		{
			blob.types[i++].name = task.name;
		}

		auto process_stage = [&](detail::task_stage stage) {
			for (unsigned i = 0; const detail::task& task : tasks)
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
