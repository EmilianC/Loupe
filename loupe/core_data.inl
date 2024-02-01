// Copyright (c) 2022 Emilian Cioca
namespace loupe
{
	namespace detail
	{
		template<typename... Functors>
		struct overload : public Functors...
		{
			using Functors::operator()...;
		};

		template<typename Type> [[nodiscard]]
		consteval std::string_view get_type_name()
		{
			std::string_view result = __FUNCSIG__;
#ifdef __clang__
			const std::string_view function_start_tag = "[Type = ";
			const std::string_view function_end_tag = "]";
#else
			const std::string_view function_start_tag = "get_type_name<";
			const std::string_view function_end_tag = ">(";
#endif
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

	// Returns a string representation of the given type's name.
	// Results will vary by compiler but should be consistent enough.
	template<typename Type> [[nodiscard]]
	consteval std::string_view get_type_name()
	{
		return detail::get_type_name<std::remove_cv_t<Type>>();
	}

	template<typename... Visitors>
	auto property::visit(Visitors&&... visitors) const
	{
		return std::visit(detail::overload{ std::forward<Visitors>(visitors)... }, data);
	}

	template<typename Tag> [[nodiscard]]
	bool metadata_container::has_metadata() const
	{
		for (const type* tag : metadata)
		{
			if (get_type_name<Tag>() == tag->name)
			{
				return true;
			}
		}

		return false;
	}

	template<typename To> [[nodiscard]]
	To* member::offset_from(void* location) const
	{
		static_assert(!std::is_const_v<To>, "Requested target type cannot be const.");
		static_assert(!std::is_volatile_v<To>, "Requested target type cannot be volatile.");

		LOUPE_ASSERT(get_type_name<To>() == data->signature, "Requested target type does not match the property's signature.");

		return reinterpret_cast<To*>(static_cast<std::byte*>(location) + offset);
	}

	template<typename... Visitors>
	auto type::visit(Visitors&&... visitors) const
	{
		return std::visit(detail::overload{ std::forward<Visitors>(visitors)... }, data);
	}

	template<typename... Args>
	void type::user_construct_at(void* location, Args&&... args) const
	{
		LOUPE_ASSERT(user_constructor, "A custom user constructor was not bound for this type.");

		auto* func = static_cast<void(*)(void*, Args...)>(user_constructor);
		func(location, std::forward<Args>(args)...);
	}

	template<typename Type> [[nodiscard]]
	const type* reflection_blob::find() const
	{
		return find(get_type_name<Type>());
	}

	template<typename Property> [[nodiscard]]
	const property* reflection_blob::find_property() const
	{
		return find_property(get_type_name<Property>());
	}
}
