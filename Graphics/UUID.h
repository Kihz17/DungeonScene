#pragma once

#include <functional>
#include <stdint.h>

// A class to represent a "unique" ID. This will simply generate a random 64-bit integer. The possibility of a clash is extremely low.
class UUID
{
public:
	UUID();
	UUID(uint64_t uuid);
	UUID(const UUID& uuid);

	operator uint64_t () { return this->uuid; }
	operator const uint64_t() const { return this->uuid; }

	static inline UUID Empty() { return UUID(0); }
private:
	uint64_t uuid;
};

namespace std
{
	template<> // Override hash for UUID type (this will allow us to hash the object in things like hashmaps)
	struct hash<UUID>
	{
		std::size_t operator()(const UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}