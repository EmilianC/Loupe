// Copyright (c) 2022 Emilian Cioca
namespace loupe
{
	namespace detail
	{
		template<typename MemberType>
		struct member_info
		{
			using Type = MemberType;
			std::size_t offset;
		};

		template<typename... Functors>
		struct overload : public Functors...
		{
			using Functors::operator()...;
		};

		template<typename Type> [[nodiscard]]
		consteval std::string_view get_type_name()
		{
			std::string_view result = std::source_location::current().function_name();

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

		template<typename T> [[nodiscard]]
		bool matches_signature(const property* property)
		{
			using TypeSignature = std::remove_cv_t<T>;

			// As a convenience, allow the use of std::uint16_t directly when dealing with enumerations.
			if constexpr (std::is_same_v<std::uint16_t, TypeSignature>)
			{
				if (property->try_as<enumeration>())
					return true;
			}

			return get_type_name<TypeSignature>() == property->signature;
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

	template<typename Data> [[nodiscard]]
	const Data& property::get_as() const
	{
		auto* result = try_as<Data>();
		LOUPE_ASSERT(result, "The property's data type was not the expected value.");

		return result;
	}

	template<typename Data> [[nodiscard]]
	const Data* property::try_as() const
	{
		if constexpr (std::is_same_v<Data, pointer>)
		{
			return std::get_if<pointer>(&data);
		}
		else if constexpr (std::is_same_v<Data, array>)
		{
			return std::get_if<array>(&data);
		}
		else if constexpr (std::is_same_v<Data, map>)
		{
			return std::get_if<map>(&data);
		}
		else if constexpr (std::is_same_v<Data, variant>)
		{
			return std::get_if<variant>(&data);
		}
		else if constexpr (std::is_same_v<Data, type>)
		{
			const auto* result = std::get_if<const type*>(&data);
			return result ? *result : nullptr;
		}
		else if constexpr (std::is_same_v<Data, fundamental>)
		{
			if (const auto* result = std::get_if<const type*>(&data))
			{
				return std::get_if<fundamental>(&(*result)->data);
			}

			return nullptr;
		}
		else if constexpr (std::is_same_v<Data, structure>)
		{
			if (const auto* result = std::get_if<const type*>(&data))
			{
				return std::get_if<structure>(&(*result)->data);
			}

			return nullptr;
		}
		else if constexpr (std::is_same_v<Data, enumeration>)
		{
			if (const auto* result = std::get_if<const type*>(&data))
			{
				return std::get_if<enumeration>(&(*result)->data);
			}

			return nullptr;
		}
		else
		{
			static_assert(false, "Unsupported data category.");
		}
	}

	template<typename Tag> [[nodiscard]]
	bool metadata_container::has() const
	{
		for (const entry& e : entries)
		{
			if (get_type_name<Tag>() == e.type->name)
			{
				return true;
			}
		}

		return false;
	}

	template<typename Tag> [[nodiscard]]
	const Tag* metadata_container::find() const
	{
		static_assert(!std::is_empty_v<Tag>, "Metadata tag type is empty and has no value to get, use 'has()' instead.");

		for (const entry& e : entries)
		{
			if (get_type_name<Tag>() == e.type->name)
			{
				return std::any_cast<Tag>(&e.value);
			}
		}

		return nullptr;
	}

	template<typename To> [[nodiscard]]
	To* member::offset_from(void* base_struct_pointer) const
	{
		LOUPE_ASSERT(base_struct_pointer, "Base pointer cannot be null.");
		LOUPE_ASSERT(std::is_void_v<To> || detail::matches_signature<To>(data), "Requested target type does not match the property's signature.");

		return reinterpret_cast<To*>(static_cast<std::byte*>(base_struct_pointer) + offset);
	}

	template<typename To> [[nodiscard]]
	const To* member::offset_from(const void* base_struct_pointer) const
	{
		LOUPE_ASSERT(base_struct_pointer, "Base pointer cannot be null.");
		LOUPE_ASSERT(std::is_void_v<To> || detail::matches_signature<To>(data), "Requested target type does not match the property's signature.");

		return reinterpret_cast<const To*>(static_cast<const std::byte*>(base_struct_pointer) + offset);
	}

	template<typename To> [[nodiscard]]
	To member::get_copy_from(const void* base_struct_pointer) const
	{
		LOUPE_ASSERT(base_struct_pointer, "Base pointer cannot be null.");
		LOUPE_ASSERT(detail::matches_signature<To>(data), "Requested target type does not match the property's signature.");

		To result;
		if (getter)
		{
			auto* func = static_cast<To (*)(const void*)>(getter);
			result = func(base_struct_pointer);
		}
		else
		{
			result = *offset_from<To>(base_struct_pointer);
		}

		return result;
	}

	template<typename From>
	void member::set_on(void* base_struct_pointer, From value) const
	{
		static_assert(!std::is_const_v<From>, "The new value cannot be passed as a const copy.");

		LOUPE_ASSERT(base_struct_pointer, "Base pointer cannot be null.");
		LOUPE_ASSERT(detail::matches_signature<From>(data), "Provided type does not match the property's signature.");

		if (setter)
		{
			auto* func = static_cast<void(*)(void*, From&)>(setter);
			func(base_struct_pointer, value);
		}
		else
		{
			From* ptr = offset_from<From>(base_struct_pointer);
			if constexpr (std::is_move_assignable_v<From>)
			{
				*ptr = std::move(value);
			}
			else
			{
				*ptr = value;
			}
		}
	}

	template<typename Functor> requires std::invocable<Functor, const member&>
	void structure::walk_members(Functor&& func) const
	{
		for (const base& base : bases)
		{
			std::get<structure>(base.type->data).walk_members(func);
		}

		for (const member& member : members)
		{
			func(member);
		}
	}

	template<typename Functor> requires std::invocable<Functor, const member&, void*>
	void structure::walk_members(void* base_struct_pointer, Functor&& func) const
	{
		LOUPE_ASSERT(base_struct_pointer, "A valid object pointer is required to walk members.");

		for (const base& base : bases)
		{
			const auto& base_structure = std::get<loupe::structure>(base.type->data);

			base_structure.walk_members(static_cast<std::byte*>(base_struct_pointer) + base.offset, func);
		}

		for (const member& member : members)
		{
			func(member, base_struct_pointer);
		}
	}

	template<typename Functor> requires std::invocable<Functor, const member&, const void*>
	void structure::walk_members(const void* base_struct_pointer, Functor&& func) const
	{
		LOUPE_ASSERT(base_struct_pointer, "A valid object pointer is required to walk members.");

		for (const base& base : bases)
		{
			const auto& base_structure = std::get<loupe::structure>(base.type->data);

			base_structure.walk_members(static_cast<const std::byte*>(base_struct_pointer) + base.offset, func);
		}

		for (const member& member : members)
		{
			func(member, base_struct_pointer);
		}
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
