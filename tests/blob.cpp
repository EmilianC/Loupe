#include "catch/catch.hpp"
#include "loupe/loupe.h"

TEST_CASE("Reflection Tests - Blobs")
{
	loupe::reflection_blob ref      = loupe::reflect();
	loupe::reflection_blob ref_copy = loupe::reflect();
	loupe::reflection_blob ref_local;

	CHECK(ref_local.get_version() == 0);
	REQUIRE(ref.get_version() == 1);
	REQUIRE(ref_copy.get_version() == 1);

	REQUIRE(!ref.get_types().empty());
	REQUIRE(ref.get_types().size() == ref_copy.get_types().size());
	for (unsigned i = 0; i < ref.get_types().size(); ++i)
	{
		CHECK(ref.get_types()[i].name == ref_copy.get_types()[i].name);
		CHECK(ref.get_types()[i].size == ref_copy.get_types()[i].size);
		CHECK(ref.get_types()[i].alignment == ref_copy.get_types()[i].alignment);
		CHECK(ref.get_types()[i].data.index() == ref_copy.get_types()[i].data.index());
		CHECK(ref.get_types()[i].construct == ref_copy.get_types()[i].construct);
		CHECK(ref.get_types()[i].construct_at == ref_copy.get_types()[i].construct_at);
	}

	REQUIRE(!ref.get_properties().empty());
	REQUIRE(ref.get_properties().size() == ref_copy.get_properties().size());
	for (unsigned i = 0; i < ref.get_properties().size(); ++i)
	{
		CHECK(ref.get_properties()[i] == ref_copy.get_properties()[i]);
		CHECK(ref.get_properties()[i].signature == ref_copy.get_properties()[i].signature);
		CHECK(ref.get_properties()[i].data.index() == ref_copy.get_properties()[i].data.index());
	}
}
