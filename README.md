![Loupe Logo](docs/logo.png)
#
An extendible reflection library for modern C++, designed with game development in mind.

# Design goals
Loupe's API is designed to be small and easy to use. Instead of scanning pre-main, Loupe produces your application's reflection tables when requested - you may store the result of this globally if you wish. Your reflection tables can be serialized to disk along with your data, allowing you to read data back safely even as your application continues to change.

The core Loupe API does not require Run-Time Type Information (RTTI) or Exceptions, and is dependent only on the C++ standard library. Serialization backends should be implemented user-side so that you can chose your own archiver and better handle custom data. However, an archiver using `Cereal` is provided by default (which can at least serve as an example for creating your own).

# 1.0 Target Features
- [ ] Diff between blobs to detect and reconcile changes
- [x] Support for standard containers (vector, array, map, set, etc.)
- [x] Support for user defined containers
- [x] Reflecting getters + setters
- [ ] Serialization ready (bring your own archiver)
- [x] User-defined metadata attributes
- [x] Stateful metadata (such as a range bound on members)

# Post 1.0 Features
- Reflecting and invoking functions
- Single header include option
- Blob coverage detection, serializing only visited properties
- Support beyond the Windows platform
- Debugger .natvis files
- Support for custom allocators

# Quick Usage

```cpp
//-- data.h --//
enum class small_enum : uint16_t
{
	value0,
	value1,
	COUNT
};

struct vec4
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
};
```
```cpp
//-- data.cpp --//
#include "data.h"
#include "loupe.h"
#include "loupe/metadata.h"

REFLECT(small_enum) ENUM_VALUES {
	REF_VALUE(value0, description("A value"))
	REF_VALUE(value1)
	REF_VALUE(COUNT, hidden())
} REF_END;

REFLECT(vec4) MEMBERS {
	REF_MEMBER(x)
	REF_MEMBER(y)
	REF_MEMBER(z)
	REF_MEMBER(w, readonly(), hidden())
} REF_END;
```
```cpp
//-- main.cpp --//
#include "data.h"
#include "loupe.h"
#include "loupe/metadata.h"

int main()
{
	using namespace loupe::metadata;

	loupe::reflection_blob blob = loupe::reflect();
	// Reclaim a little memory if you don't plan on calling reflect() again.
	loupe::clear_reflect_tasks();

	// Type lookup can be fully dynamic,
	const loupe::type* enum_type = blob.find("small_enum");
	// or done directly with known types.
	const loupe::type* vec4_type = blob.find<vec4>();

	print(enum_type->name); // "small_enum"
	print(enum_type->size); // 2
	print(vec4_type->name); // "vec4"
	print(vec4_type->size); // 16

	// The category of the type can be fetched directly when known.
	const loupe::enumeration& enum_data = std::get<loupe::enumeration>(enum_type->data);
	for (const loupe::enum_entry& entry : enum_data.entires)
	{
		auto* tooltip = entry.metadata.find<description>();

		print(entry.name);                   // "value0",  "value1", "COUNT"
		print(entry.value);                  //  0,         1,        2
		print(entry.metadata.has<hidden>()); // "false",   "false",  "true"
		print(tooltip ? tooltip->text : ""); // "A value", "",       ""
	}

	// Otherwise, the visitation pattern can respond to the three possible categories.
	vec4_type->visit(
		[&](const loupe::fundamental& data) {
			/*...*/
		},
		[&](const loupe::structure& data) {
			const loupe::property* float_property = blob.find_property<float>();
			for (const loupe::member& m : data.members)
			{
				print(m.name);                   // "x",     "y",     "z",     "w"
				print(m.offset);                 //  0,       4,       8,       12
				print(m.data == float_property); // "true",  "true",  "true",  "true"
				print(m.metadata.has<hidden>()); // "false", "false", "false", "true"
			}
		},
		[&](const loupe::enumeration& data) {
			/*...*/
		}
	);
}
```

# Default Serialization Dependencies
- [Cereal 1.3.2](https://github.com/EmilianC/cereal/tree/fork-stable)

# Test Project Dependencies
- [Catch 2.13.9](https://github.com/catchorg/Catch2/tree/v2.x)
