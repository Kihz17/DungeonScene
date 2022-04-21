#include "UUID.h"

#include <random>

static std::random_device randomDevice;
static std::mt19937_64 engine(randomDevice());
static std::uniform_int_distribution<uint64_t> uniformDist64;

UUID::UUID() : uuid(uniformDist64(engine)) {}

UUID::UUID(uint64_t uuid) : uuid(uuid) {}

UUID::UUID(const UUID& uuid) : uuid(uuid.uuid) {}
