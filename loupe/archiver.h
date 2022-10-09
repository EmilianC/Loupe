// Copyright (c) 2022 Emilian Cioca
#pragma once
#ifdef LOUPE_ARCHIVER_CEREAL
#include "loupe.h"

namespace loupe
{
	struct serializable {};

	template<typename Archive>
	struct archiver
	{
		archiver(const reflection_blob& blob, Archive& archive)
			: archive(&archive)
		{
			int_type = blob.find<int>();
			float_type = blob.find<float>();
		}

		void serialize(void* data, const type_descriptor* type)
		{
			if (type->members.empty())
			{
				if (type == int_type)
				{
					serialize(static_cast<int*>(data));
				}
				else if (type == float_type)
				{
					serialize(static_cast<int*>(data));
				}
			}
			else
			{
				for (const member_descriptor& member : type->members)
				{
					//if (!member.has_metadata<serializable>())
					//	continue;

					serialize(static_cast<std::byte*>(data) + member.offset, member.type);
				}
			}
		}

		void serialize(short int data)        { (*archive)(data); }
		void serialize(int data)              { (*archive)(data); }
		void serialize(unsigned int data)     { (*archive)(data); }
		void serialize(long data)             { (*archive)(data); }
		void serialize(char data)             { (*archive)(data); }
		void serialize(unsigned char data)    { (*archive)(data); }
		void serialize(bool data)             { (*archive)(data); }
		void serialize(float data)            { (*archive)(data); }
		void serialize(double data)           { (*archive)(data); }
		void serialize(std::byte data)        { (*archive)(data); }
		void serialize(std::string data)      { (*archive)(data); }
		void serialize(std::string_view data) { (*archive)(data); }

		const type_descriptor* int_type;
		const type_descriptor* float_type;

		Archive* archive;
	};
}

#endif
