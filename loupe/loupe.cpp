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

		property* find_or_add_property(std::vector<property>& properties, std::string_view signature)
		{
			auto itr = std::lower_bound(properties.begin(), properties.end(), signature, [](const property& prop, std::string_view signature) {
				return prop.signature < signature;
			});

			if (itr == properties.end())
			{
				properties.push_back({ signature });
				return &properties.back();
			}

			if (itr->signature != signature)
			{
				itr = properties.insert(itr, { signature });
			}

			return &(*itr);
		}
	}

	reflection_blob reflect(unsigned int current_program_version)
	{
		LOUPE_ASSERT(current_program_version > 0, "The program version id must be greater than zero.");

		reflection_blob blob;
		blob.version = current_program_version;

		auto& tasks = detail::get_tasks();
		// The final array of types needs to be sorted to allow for faster lookups.
		// It is easier to just sort the tasks by name from the start.
		std::sort(tasks.begin(), tasks.end(), [](const detail::task& left, const detail::task& right) {
			return left.name < right.name;
		});

#ifdef LOUPE_ASSERTS_ENABLED
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

		for (unsigned i = 0; const detail::task& task : tasks)
		{
			task.initialize_type(blob, blob.types[i++]);
		}

		auto process_data_stage = [&](detail::task_data_stage stage) {
			for (unsigned i = 0; const detail::task& task : tasks)
			{
				if (task.initialize_data)
				{
					task.initialize_data(blob, blob.properties, blob.types[i], stage);
				}
				++i;
			}
		};

		process_data_stage(detail::task_data_stage::scan_properties);
		process_data_stage(detail::task_data_stage::enums);
		process_data_stage(detail::task_data_stage::bases);
		process_data_stage(detail::task_data_stage::members);

		return blob;
	}

	void clear_reflect_tasks()
	{
		detail::get_tasks().clear();
	}
}
