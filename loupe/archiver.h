// Copyright (c) 2022 Emilian Cioca
#pragma once
#include "loupe.h"

#include <cereal/archives/json.hpp>

namespace loupe
{
	struct serializable {};

	template<typename Archive>
	struct archiver
	{
		archiver(const reflection_blob& blob, Archive& archive)
			: archive(&archive)
		{
		}

		void serialize(void* data, const type* type)
		{
			type->visit(
				[&](const structure& data) {
					for (const member& member : data.members)
					{

					}
				},
				[&](const enumeration& data) { /*...*/ },
				[&](const pointer& data) { /*...*/ },
				[&](const array& data) { /*...*/ },
				[&](const map& data) { /*...*/ },
				[&](const variant& data) { /*...*/ },
				[&](const fundamental& data) { /*...*/ }
			);
		}

		void serialize(short int data)               { (*archive)(data); }
		void serialize(int data)                     { (*archive)(data); }
		void serialize(long int data)                { (*archive)(data); }
		void serialize(long long int data)           { (*archive)(data); }
		void serialize(unsigned short int data)      { (*archive)(data); }
		void serialize(unsigned int data)            { (*archive)(data); }
		void serialize(unsigned long int data)       { (*archive)(data); }
		void serialize(unsigned long long int data)  { (*archive)(data); }
		void serialize(char data)                    { (*archive)(data); }
		void serialize(unsigned char data)           { (*archive)(data); }
		void serialize(char8_t data)                 { (*archive)(data); }
		void serialize(char16_t data)                { (*archive)(data); }
		void serialize(char32_t data)                { (*archive)(data); }
		void serialize(wchar_t data)                 { (*archive)(data); }
		void serialize(bool data)                    { (*archive)(data); }
		void serialize(float data)                   { (*archive)(data); }
		void serialize(double data)                  { (*archive)(data); }
		void serialize(long double data)             { (*archive)(data); }
		void serialize(std::byte data)               { (*archive)(data); }
		void serialize(std::string data)             { (*archive)(data); }
		void serialize(std::string_view data)        { (*archive)(data); }

		Archive* archive;
	};
}
