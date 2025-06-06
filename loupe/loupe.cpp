// Copyright (c) 2022 Emilian Cioca
#include "loupe.h"

#include <algorithm>

namespace loupe
{
	namespace detail
	{
		std::vector<type_task>& get_tasks()
		{
			static std::vector<type_task> tasks = []() {
				std::vector<type_task> storage;
				storage.reserve(256);
				return storage;
			}();

			return tasks;
		}

		property* add_property(std::vector<property>& properties, std::string_view signature)
		{
			auto itr = std::lower_bound(properties.begin(), properties.end(), signature, [](const property& prop, const std::string_view& signature) {
				return prop.signature < signature;
			});

			LOUPE_ASSERT(itr == properties.end() || itr->signature != signature, "The property has already added. There is a flow error.");

			itr = properties.insert(itr, { signature });
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
		std::sort(tasks.begin(), tasks.end(), [](const detail::type_task& left, const detail::type_task& right) {
			return left.name < right.name;
		});

#ifdef LOUPE_ASSERTS_ENABLED
		{
			// Check for duplicate tasks.
			auto itr = std::adjacent_find(tasks.begin(), tasks.end(), [](const detail::type_task& left, const detail::type_task& right) {
				return left.name == right.name;
			});
			LOUPE_ASSERT(itr == tasks.end(), "A unique type has been reflected more than once.");
		}
#endif

		blob.types.resize(tasks.size());
		for (unsigned i = 0; const detail::type_task& task : tasks)
		{
			blob.types[i++].name = task.name;
		}

		for (unsigned i = 0; const detail::type_task& task : tasks)
		{
			task.initialize_type(blob.types[i++]);
		}

		std::vector<detail::property_task> property_tasks;
		property_tasks.reserve(64);

		auto process_data_stage = [&](detail::task_data_stage stage) {
			for (unsigned i = 0; const detail::type_task& task : tasks)
			{
				if (task.initialize_data)
				{
					task.initialize_data(blob, blob.properties, property_tasks, blob.types[i], stage);
				}
				++i;
			}
		};

		process_data_stage(detail::task_data_stage::scan_properties);
		blob.properties.shrink_to_fit();

		// Sorting to ensure the indices of the tasks will match up to the properties in the blob.
		std::sort(property_tasks.begin(), property_tasks.end(), [](const detail::property_task& left, const detail::property_task& right) {
			return left.signature < right.signature;
		});

		for (unsigned i = 0; const detail::property_task& task : property_tasks)
		{
			task.initialize_property(blob, blob.properties[i++]);
		}

		process_data_stage(detail::task_data_stage::enums);
		process_data_stage(detail::task_data_stage::bases);
		process_data_stage(detail::task_data_stage::members);

#ifdef LOUPE_ASSERTS_ENABLED
		for (const type& type : blob.types)
		{
			if (const auto* structure = std::get_if<loupe::structure>(&type.data))
			{
				// Check that bases are reflected in the order of inheritance.
				if (structure->bases.size() > 1)
				{
					size_t offset = 0;
					for (const base& base : structure->bases)
					{
						LOUPE_ASSERT(base.offset >= offset, "Bases must be reflected in the same order that they are inherited.");
						offset = base.offset;
					}
				}

				if (structure->members.size() > 1)
				{
					// Check that members are reflected in ascending memory order.
					size_t offset = 0;
					for (const member& member : structure->members)
					{
						LOUPE_ASSERT(member.offset >= offset, "Members must be reflected in the same order they appear in the structure or class.");
						offset = member.offset;
					}

					// Check for duplicate member reflection.
					auto itr = std::adjacent_find(structure->members.begin(), structure->members.end(), [](const member& left, const member& right) {
						return left.name == right.name;
					});
					LOUPE_ASSERT(itr == structure->members.end(), "A structure or class member has been reflected more than once.");
				}
			}
			else if (const auto* enumeration = std::get_if<loupe::enumeration>(&type.data))
			{
				if (enumeration->entries.size() > 1)
				{
					// Check for duplicate value entry reflection.
					std::vector<std::string_view> entry_names;
					entry_names.reserve(enumeration->entries.size());

					for (const enum_entry& entry : enumeration->entries)
					{
						entry_names.push_back(entry.name);
					}
					std::sort(entry_names.begin(), entry_names.end());

					auto itr = std::adjacent_find(entry_names.begin(), entry_names.end());
					LOUPE_ASSERT(itr == entry_names.end(), "An enumeration value has been reflected more than once.");
				}
			}
		}
#endif

		return blob;
	}

	void clear_reflect_tasks()
	{
		detail::get_tasks().clear();
	}
}
