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
					LOUPE_ASSERT(tag_type, "Metadata tag was not registered. Metadata tags must also be reflected separately.");

					entry.metadata.push_back(tag_type);
				} (), ...);
			}

			return entry;
		}

		template<typename Type, typename... Tags>
		member register_member(reflection_blob& blob, std::string_view name, std::size_t offset)
		{
			member member {
				.name = name,
				.offset = offset,
				.type = blob.find<Type>(),
				.is_const = std::is_const_v<Type>
			};

			if constexpr (sizeof...(Tags) > 0)
			{
				member.metadata.reserve(sizeof...(Tags));

				([&] {
					const type* tag_type = blob.find<Tags>();
					LOUPE_ASSERT(tag_type, "Metadata tag was not registered. Metadata tags must also be reflected separately.");

					member.metadata.push_back(tag_type);
				} (), ...);
			}

			return member;
		}

		template<typename Type, typename... Tags>
		static_member register_static_member(reflection_blob& blob, std::string_view name, void* ptr)
		{
			static_member member {
				.name = name,
				.address = ptr,
				.type = blob.find<Type>(),
				.is_const = std::is_const_v<Type>
			};

			if constexpr (sizeof...(Tags) > 0)
			{
				member.metadata.reserve(sizeof...(Tags));

				([&] {
					const type* tag_type = blob.find<Tags>();
					LOUPE_ASSERT(tag_type, "Metadata tag was not registered. Metadata tags must also be reflected separately.");

					member.metadata.push_back(tag_type);
				} (), ...);
			}

			return member;
		}

		template<typename Base>
		const type* register_base(reflection_blob& blob)
		{
			const type* base_type = blob.find<Base>();
			LOUPE_ASSERT(base_type, "Base class type not registered. Base classes must also be reflected separately.");

			return base_type;
		}
	}
}
