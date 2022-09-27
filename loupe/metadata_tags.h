// Copyright (c) 2022 Emilian Cioca
#pragma once
namespace loupe
{
	// Takes a lambda in the template arg (with a this* to the surrounding type)
	struct condition
	{
		bool(*Func)(void*);
	};

	struct float_range
	{
		float min;
		float max;
	};
}
