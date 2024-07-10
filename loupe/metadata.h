// Copyright (c) 2024 Emilian Cioca
#pragma once
#include "assert.h"

#include <limits>
#include <string_view>

namespace loupe::metadata
{
	struct hidden {};
	struct readonly {};
	struct not_serialized {};

	struct description              { std::string_view text; };
	struct display_name             { std::string_view name; };
	struct category                 { std::string_view name; };
	struct previously_serialized_as { std::string_view name; };

	template<typename T>
	struct range
	{
		range() = default;
		range(T min, T max)
			: min(min), max(max)
		{
			LOUPE_ASSERT(min <= max, "Invalid value range.");
		};

		[[nodiscard]] bool has_lower_bound() const { return min != std::numeric_limits<T>::min(); }
		[[nodiscard]] bool has_upper_bound() const { return max != std::numeric_limits<T>::max(); }
		[[nodiscard]] bool has_both_bounds() const { return has_lower_bound() && has_upper_bound(); }

		T min = std::numeric_limits<T>::min();
		T max = std::numeric_limits<T>::max();
	};

	template<typename T> [[nodiscard]]
	range<T> min(T value)
	{
		return range<T>(value, std::numeric_limits<T>::max());
	}

	template<typename T> [[nodiscard]]
	range<T> max(T value)
	{
		return range<T>(std::numeric_limits<T>::min(), value);
	}
}
