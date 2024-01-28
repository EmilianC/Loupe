// Copyright (c) 2022 Emilian Cioca
namespace loupe::detail
{
	enum class task_data_stage
	{
		//
		scan_properties,
		//
		enums,
		//
		bases,
		//
		members
	};

	template<typename Functor>
	struct on_scope_exit
	{
		on_scope_exit(Functor func)
			: functor(std::move(func))
		{
		}

		~on_scope_exit()
		{
			functor();
		}

		[[no_unique_address]] Functor functor;
	};

	struct task
	{
		std::string_view name;
		void (*initialize_type)(const reflection_blob&, type&) = nullptr;
		void (*initialize_data)(reflection_blob&, std::vector<property>&, type&, task_data_stage) = nullptr;
	};

	std::vector<task>& get_tasks();
	property* find_or_add_property(std::vector<property>& properties, std::string_view signature);

	template<typename Type>
	void register_property(reflection_blob& blob, std::vector<property>& properties)
	{
		property* property = find_or_add_property(properties, detail::get_type_name<Type>());

		if constexpr (adapters::pointer_adapter<Type>::value)
		{
			register_property<typename adapters::pointer_adapter<Type>::TargetType>(blob, properties);
			property->data = adapters::pointer_adapter<Type>::make_data(blob);
		}
		else if constexpr (adapters::array_adapter<Type>::value)
		{
			register_property<typename adapters::array_adapter<Type>::ElementType>(blob, properties);
			property->data = adapters::array_adapter<Type>::make_data(blob);
		}
		else if constexpr (adapters::map_adapter<Type>::value)
		{
			register_property<typename adapters::map_adapter<Type>::KeyType>(blob, properties);
			register_property<typename adapters::map_adapter<Type>::ValueType>(blob, properties);
			property->data = adapters::map_adapter<Type>::make_data(blob);
		}
		else if constexpr (adapters::variant_adapter<Type>::value)
		{
			auto register_variant_properties = [&]<typename... Alternatives>([[maybe_unused]] std::tuple<Alternatives...>*) {
				( find_or_add_property(properties, detail::get_type_name<Alternatives>()), ... );
			};

			typename adapters::variant_adapter<Type>::Tuple* tag = nullptr;
			register_variant_properties(tag);

			property->data = adapters::variant_adapter<Type>::make_data(blob);
		}
		else if constexpr (std::is_class_v<Type> || std::is_enum_v<Type> || std::is_fundamental_v<Type>)
		{
			const type* type = blob.find<Type>();
			LOUPE_ASSERT(type, "A type required by a property was not registered");

			property->data = type;
		}
		else
		{
			LOUPE_ASSERT(false, "Unsupported type category");
		}
	}

	template<typename reflected_type>
	void init_type_data(const reflection_blob& blob, type& type)
	{
		if constexpr (std::is_void_v<reflected_type>)
		{
			type.data = loupe::fundamental{};
			type.size = 0;
			type.alignment = 1;
		}
		else
		{
			type.size = sizeof(reflected_type);
			type.alignment = alignof(reflected_type);
			if constexpr (std::is_default_constructible_v<reflected_type>)
			{
				type.default_construct_at = [](void* location) {
					LOUPE_ASSERT(reinterpret_cast<std::uintptr_t>(location) % alignof(reflected_type) == 0, "Construction location for type is misaligned.");
					new (location) reflected_type;
				};
			}

			if constexpr (std::is_class_v<reflected_type>)
			{
				type.data = loupe::structure{};

				if constexpr (std::is_destructible_v<reflected_type> && !std::is_trivially_destructible_v<reflected_type>)
				{
					type.destruct_at = [](void* location) {
						static_cast<reflected_type*>(location)->~reflected_type();
					};
				}
			}
			else if constexpr (std::is_enum_v<reflected_type>)
			{
				type.data = loupe::enumeration {
					.underlying_type = blob.find<std::underlying_type_t<reflected_type>>(),
					.strongly_typed = std::is_scoped_enum_v<reflected_type>
				};
			}
			else if constexpr (std::is_fundamental_v<reflected_type>)
			{
				type.data = loupe::fundamental{};
			}
			else
			{
				LOUPE_ASSERT(false, "Unsupported type category.");
			}
		}
	}

	template<typename... Tags>
	enum_entry create_enum_entry(const reflection_blob& blob, std::string_view name, std::size_t value)
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

	template<typename Type, typename... Args>
	void* make_user_constructor()
	{
		static_assert(sizeof...(Args) > 0, "Use \"default_construct_at\" instead of user constructors if no arguments are required.");
		static_assert(std::is_class_v<Type>, "User Constructors are only supported on structure/class types.");

		return +[](void* location, Args&&...args) {
			new (location) Type(std::forward<Args>(args)...);
		};
	}

	template<typename Type, typename... Tags>
	member create_member(const reflection_blob& blob, std::string_view name, std::size_t offset)
	{
		member member {
			.name = name,
			.offset = offset,
			.data = blob.find_property<Type>()
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

	template<typename Base, typename Derived>
	const type* find_base(const reflection_blob& blob)
	{
		static_assert(std::is_base_of_v<Base, Derived>, "The reflected base is not actually a base class of the main type.");
		static_assert(std::is_class_v<Base>, "A reflected base must be either a class or struct type.");

		const type* base_type = blob.find<Base>();
		LOUPE_ASSERT(base_type, "Base class type not registered. Base classes must also be reflected separately.");

		return base_type;
	}
}
