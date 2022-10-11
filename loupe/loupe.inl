// Copyright (c) 2022 Emilian Cioca
namespace loupe
{
	namespace detail
	{
		template<typename Type, typename... Tags>
		loupe::member_descriptor register_member(loupe::reflection_blob& blob, const loupe::type_descriptor& owning_type, std::string_view name, std::size_t offset)
		{
			loupe::member_descriptor member;
			member.name = name;
			member.offset = offset;
			member.owning_type = &owning_type;

			member.is_const = std::is_const_v<Type>;
			member.is_pointer = std::is_pointer_v<Type> || std::is_reference_v<Type>;

			if constexpr (std::is_array_v<Type>)
			{
				member.is_array = true;
				member.element_count = std::size(Type{});
			}
			else
			{
				member.is_array = false;
			}

			if constexpr (std::is_enum_v<Type>)
			{
				member.enum_type = blob.find_enum<Type>();
			}
			else
			{
				member.type = blob.find_type<Type>();
			}

			if constexpr (sizeof...(Tags) > 0)
			{
				member.metadata.reserve(sizeof...(Tags));

				([&] {
					const loupe::type_descriptor* tag_type = blob.find_type<Tags>();
					// error if null.
					member.metadata.push_back(tag_type);
				} (), ...);
			}

			return member;
		}

		template<typename Base>
		const loupe::type_descriptor* register_base(loupe::reflection_blob& blob)
		{
			const loupe::type_descriptor* base_type = blob.find_type<Base>();
			// error if null.
			return base_type;
		}

		template<typename Type> [[nodiscard]]
		consteval std::string_view get_type_name()
		{
			std::string_view result = __FUNCSIG__;
			const std::string_view function_start_tag = "get_type_name<";
			const std::string_view function_end_tag = ">(";
			const std::string_view enum_tag = "enum ";
			const std::string_view class_tag = "class ";
			const std::string_view struct_tag = "struct ";

			auto start_index = result.find(function_start_tag) + function_start_tag.size();
			auto end_index = result.rfind(function_end_tag);

			result = result.substr(start_index, end_index - start_index);

			if (result.starts_with(enum_tag))
			{
				result.remove_prefix(enum_tag.size());
			}

			if (result.starts_with(class_tag))
			{
				result.remove_prefix(class_tag.size());
			}

			if (result.starts_with(struct_tag))
			{
				result.remove_prefix(struct_tag.size());
			}

			return result;
		}
	}

	template<typename Tag> [[nodiscard]]
	bool member_descriptor::has_metadata() const
	{
		if (metadata.empty())
			return false;

		constexpr std::string_view tag_name = detail::get_type_name<Tag>();
		for (const type_descriptor* data : metadata)
		{
			if (tag_name == data->name)
			{
				return true;
			}
		}

		return false;
	}

	template<typename Type> [[nodiscard]]
	const type_descriptor* reflection_blob::find_type() const
	{
		static_assert(!std::is_enum_v<Type>);

		return find_type(detail::get_type_name<Type>());
	}

	template<typename Type> [[nodiscard]]
	const enum_descriptor* reflection_blob::find_enum() const
	{
		static_assert(std::is_enum_v<Type>);

		return find_enum(detail::get_type_name<Type>());
	}
}
