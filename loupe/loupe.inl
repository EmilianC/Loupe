// Copyright (c) 2022 Emilian Cioca
namespace loupe
{
	namespace detail
	{
		enum class task_stage
		{
			// Construct top-level type data.
			type_adapters,
			//
			enums_bases_members
		};

		struct task
		{
			std::string_view name;
			void (*func)(reflection_blob&, type&, task_stage);
		};

		std::vector<task>& get_tasks();

		template<typename... Tags>
		enum_entry register_enum_entry(reflection_blob& blob, std::string_view name, std::size_t value)
		{
			enum_entry entry {
				.name = name,
				.value = value
			};
		
			if constexpr (sizeof...(Tags) > 0)
			{
				entry.metadata.reserve(sizeof...(Tags));
		
				([&] {
					const type* tag_type = blob.find<Tags>();
					// assert(tag_type)
					entry.metadata.push_back(tag_type);
				} (), ...);
			}
		
			return entry;
		}

		template<typename Type, typename... Tags>
		variable register_variable(reflection_blob& blob, std::string_view name, std::size_t offset)
		{
			variable variable {
				.name = name,
				.offset = offset,
				.type = blob.find<Type>(),
				.is_const = std::is_const_v<Type>
			};

			if constexpr (sizeof...(Tags) > 0)
			{
				variable.metadata.reserve(sizeof...(Tags));

				([&] {
					const type* tag_type = blob.find<Tags>();
					// assert(tag_type)
					variable.metadata.push_back(tag_type);
				} (), ...);
			}

			return variable;
		}

		template<typename Type, typename... Tags>
		static_variable register_static_variable(reflection_blob& blob, std::string_view name, void* ptr)
		{
			static_variable variable{
				.name = name,
				.address = ptr,
				.type = blob.find<Type>(),
				.is_const = std::is_const_v<Type>
			};

			if constexpr (sizeof...(Tags) > 0)
			{
				variable.metadata.reserve(sizeof...(Tags));

				([&] {
					const type* tag_type = blob.find<Tags>();
					// assert(tag_type)
					variable.metadata.push_back(tag_type);
				} (), ...);
			}

			return variable;
		}

		template<typename Base>
		const type* register_base(reflection_blob& blob)
		{
			const type* base_type = blob.find<Base>();
			// assert(base_type)
			return base_type;
		}
	}
}
