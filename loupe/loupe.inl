// Copyright (c) 2022 Emilian Cioca
namespace loupe
{
	template<typename T> [[nodiscard]]
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

	template<typename Tag>
	bool member_descriptor::has_metadata() const
	{
		if (metadata.empty())
			return false;

		constexpr std::string_view tag_name = get_type_name<Tag>();
		for (const type_descriptor* data : metadata)
		{
			if (tag_name == data->name)
			{
				return true;
			}
		}
	}


	template<typename Type>
	auto reflection_blob::find() const
		-> std::conditional_t<std::is_enum_v<Type>, const enum_descriptor*, const type_descriptor*>
	{
		if constexpr (std::is_enum_v<Type>)
		{
			return find_enum(get_type_name<Type>());
		}
		else
		{
			return find_type(get_type_name<Type>());
		}
	}
}
