// Copyright (c) 2022 Emilian Cioca
namespace loupe
{
	template<typename Type>
	const type_descriptor* reflection_blob::find_type() const
	{
		const type_descriptor* descriptor = detail::type_handler<Type>::descriptor;
		if (descriptor)
		{
			return find_type(descriptor->full_name);
		}

		return nullptr;
	}



	// default supported types
	namespace detail
	{
		extern template struct detail::type_handler<float>;
		extern template struct detail::type_handler<double>;
		extern template struct detail::type_handler<char>;
		extern template struct detail::type_handler<int>;
		extern template struct detail::type_handler<unsigned>;
	}
}
