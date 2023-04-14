#include "catch/catch.hpp"
#include "loupe/archiver.h"
#include "loupe/loupe.h"
#include "test_data.h"

#include <cereal/archives/json.hpp>

TEST_CASE("Reflection Tests - Serialization")
{
	loupe::reflection_blob ref = loupe::reflect();
}
