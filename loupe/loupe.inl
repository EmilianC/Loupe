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

	struct property_task
	{
		std::string_view signature;
		void (*initialize_property)(const reflection_blob&, property&) = nullptr;
	};

	struct type_task
	{
		std::string_view name;
		void (*initialize_type)(type&) = nullptr;
		void (*initialize_data)(reflection_blob&, std::vector<property>&, std::vector<property_task>&, type&, task_data_stage) = nullptr;
	};

	std::vector<type_task>& get_tasks();
	property* add_property(std::vector<property>& properties, std::string_view signature);

	template<typename Type>
	void scan_properties(reflection_blob& blob, std::vector<property>& properties, std::vector<property_task>& property_tasks)
	{
		const std::string_view property_signature = get_type_name<std::remove_cv_t<Type>>();
		if (blob.find_property(property_signature))
			return;

		add_property(properties, property_signature);

		void (*property_func)(const reflection_blob&, property&) = nullptr;
		if constexpr (adapters::pointer_adapter<Type>::value)
		{
			scan_properties<typename adapters::pointer_adapter<Type>::TargetType>(blob, properties, property_tasks);

			property_func = +[](const reflection_blob& blob, property& prop) {
				prop.data = adapters::pointer_adapter<Type>::make_data(blob);
			};
		}
		else if constexpr (adapters::array_adapter<Type>::value)
		{
			scan_properties<typename adapters::array_adapter<Type>::ElementType>(blob, properties, property_tasks);

			property_func = +[](const reflection_blob& blob, property& prop) {
				prop.data = adapters::array_adapter<Type>::make_data(blob);
			};
		}
		else if constexpr (adapters::map_adapter<Type>::value)
		{
			scan_properties<typename adapters::map_adapter<Type>::KeyType>(blob, properties, property_tasks);
			scan_properties<typename adapters::map_adapter<Type>::ValueType>(blob, properties, property_tasks);

			property_func = +[](const reflection_blob& blob, property& prop) {
				prop.data = adapters::map_adapter<Type>::make_data(blob);
			};
		}
		else if constexpr (adapters::variant_adapter<Type>::value)
		{
			auto scan_variant_properties = [&]<typename... Alternatives>([[maybe_unused]] std::tuple<Alternatives...>*) {
				( scan_properties<std::remove_cv_t<Alternatives>>(blob, properties, property_tasks), ... );
			};

			typename adapters::variant_adapter<Type>::Tuple* tag = nullptr;
			scan_variant_properties(tag);

			property_func = +[](const reflection_blob& blob, property& prop) {
				prop.data = adapters::variant_adapter<Type>::make_data(blob);
			};
		}
		else
		{
			static_assert(std::is_class_v<Type> || std::is_enum_v<Type> || std::is_fundamental_v<Type>, "Unsupported type category.");
			property_func = +[](const reflection_blob& blob, property& prop) {
				const type* type = blob.find<Type>();
				LOUPE_ASSERT(type, "A type required by a property was not registered.");

				prop.data = type;
			};
		}

		property_tasks.emplace_back(property_signature, property_func);
	}

	template<typename reflected_type>
	void init_type_data(type& type)
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
					LOUPE_ASSERT(location && (reinterpret_cast<std::uintptr_t>(location) % alignof(reflected_type) == 0),
						"Attempting to construct an object at an invalid address.");

					new (location) reflected_type;
				};
			}

			if constexpr (std::is_class_v<reflected_type>)
			{
				type.data = loupe::structure{};

				if constexpr (std::is_destructible_v<reflected_type> && !std::is_trivially_destructible_v<reflected_type>)
				{
					type.destruct_at = [](void* location) {
						LOUPE_ASSERT(location && (reinterpret_cast<std::uintptr_t>(location) % alignof(reflected_type) == 0),
							"Attempting to destruct an object at an invalid address.");

						static_cast<reflected_type*>(location)->~reflected_type();
					};
				}
			}
			else if constexpr (std::is_enum_v<reflected_type> && !std::is_same_v<reflected_type, std::byte>)
			{
				static_assert(std::is_same_v<std::underlying_type_t<reflected_type>, std::uint16_t>, "Reflected enumerations must be of uint16_t type.");
				type.data = loupe::enumeration {
					.strongly_typed = std::is_scoped_enum_v<reflected_type>
				};
			}
			else if constexpr (std::is_fundamental_v<reflected_type> || std::is_same_v<reflected_type, std::byte>)
			{
				type.data = loupe::fundamental{};
			}
			else
			{
				static_assert(always_false<reflected_type>::value, "Unsupported type category.");
			}
		}
	}

	template<typename... Tags>
	void add_metadata(const reflection_blob& blob, metadata_container& container, [[maybe_unused]] Tags&&... tags)
	{
		if constexpr (sizeof...(Tags) > 0)
		{
			container.entries.reserve(sizeof...(Tags));

			([&] {
				const type* tag_type = blob.find<Tags>();
				LOUPE_ASSERT(tag_type, "Metadata tag was not registered. Metadata tags must also be reflected separately.");

				if constexpr (std::is_empty_v<Tags>)
				{
					container.entries.push_back({ tag_type, {} });
				}
				else
				{
					container.entries.push_back({ tag_type, tags });
				}
			} (), ...);
		}
	}

	template<typename... Tags>
	enum_entry create_enum_entry(const reflection_blob& blob, std::string_view name, std::uint16_t value, Tags&&... tags)
	{
		enum_entry entry {
			.name = name,
			.value = value
		};

		add_metadata(blob, entry.metadata, std::forward<Tags>(tags)...);

		return entry;
	}

	template<typename Type, typename... Args>
	void* make_user_constructor()
	{
		static_assert(sizeof...(Args) > 0, "Use \"default_construct_at\" instead of user constructors if no arguments are required.");
		static_assert(std::is_class_v<Type>, "User Constructors are only supported on structure/class types.");

		return +[](void* location, Args&&... args) {
			new (location) Type(std::forward<Args>(args)...);
		};
	}

	template<typename Type, typename... Tags>
	member create_member(const reflection_blob& blob, std::string_view name, std::size_t offset, void* getter, void* setter, Tags&&... tags)
	{
		member member {
			.name = name,
			.offset = offset,
			.data = blob.find_property<Type>(),
			.getter = getter,
			.setter = setter
		};

		add_metadata(blob, member.metadata, std::forward<Tags>(tags)...);

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
