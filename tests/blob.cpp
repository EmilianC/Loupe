#include "catch/catch.hpp"
#include "loupe/loupe.h"

TEST_CASE("Reflection Tests - Blobs")
{
	loupe::reflection_blob ref      = loupe::reflect();
	loupe::reflection_blob ref_copy = loupe::reflect();

	REQUIRE(!ref.types.empty());
	REQUIRE(ref.types.size() == ref_copy.types.size());
	for (unsigned i = 0; i < ref.types.size(); ++i)
	{
		CHECK(ref.types[i].name == ref_copy.types[i].name);
		CHECK(ref.types[i].size == ref_copy.types[i].size);
		CHECK(ref.types[i].alignment == ref_copy.types[i].alignment);
		CHECK(ref.types[i].data.index() == ref_copy.types[i].data.index());
		CHECK(!!ref.types[i].construct == !!ref_copy.types[i].construct);
		CHECK(!!ref.types[i].construct_at == !!ref_copy.types[i].construct_at);
	}
}
