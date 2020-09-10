#include "SquirrelRNG.h"

#include <limits>

SquirrelRNG::SquirrelRNG(u32 seed)
	: m_seed(seed)
{
	
}

SquirrelRNG::~SquirrelRNG()
{
	
}

void SquirrelRNG::ResetSeed(u32 seed, i32 position)
{
	m_seed = seed;
	m_position = position;
}

u32 SquirrelRNG::Get1DNoiseUint(i32 positionX, u32 seed)
{
	const u32 BIT_NOISE1 = 0x68E31DA4;
	const u32 BIT_NOISE2 = 0xB5297A4D;
	const u32 BIT_NOISE3 = 0x1B56C4E9;

	u32 mangledBits = (u32)positionX;
	mangledBits *= BIT_NOISE1;
	mangledBits += seed;
	mangledBits ^= (mangledBits >> 8);
	mangledBits *= BIT_NOISE2;
	mangledBits ^= (mangledBits << 8);
	mangledBits *= BIT_NOISE3;
	mangledBits ^= (mangledBits >> 8);
	return mangledBits;
}

u32 SquirrelRNG::RollRandomUint32()
{
	return Get1DNoiseUint(m_position++, m_seed);
}

u16 SquirrelRNG::RollRandomUint16()
{
	return (u16)Get1DNoiseUint(m_position++, m_seed);
}

u8 SquirrelRNG::RollRandomByte()
{
	return (u8)Get1DNoiseUint(m_position++, m_seed);
}

u32 SquirrelRNG::RollRandomLessThan(u32 maxValueNotInclusive)
{
	return Get1DNoiseUint(m_position++, m_seed) % maxValueNotInclusive;
}

i32 SquirrelRNG::RollRandomIntInRange(i32 minValueInclusive, i32 maxValueInclusive)
{
	u32 result = Get1DNoiseUint(m_position++, m_seed);
	result %= maxValueInclusive - minValueInclusive + 1;
	return (i32)result + minValueInclusive;
}

float SquirrelRNG::RollRandomFloatZeroToOne()
{
	return (float)Get1DNoiseUint(m_position++, m_seed) / (float)std::numeric_limits<u32>().max();
}

float SquirrelRNG::RollRandomFloatInRange(float minValueInclusive, float maxValueInclusive)
{
	float result = RollRandomFloatZeroToOne();
	result = result * (maxValueInclusive - minValueInclusive);
	result += minValueInclusive;
	return result;
}

float SquirrelRNG::RollRandomFloatNegativeOneToOne()
{
	return RollRandomFloatInRange(-1.0f, 1.0f);
}

bool SquirrelRNG::RollRandomChance(float probabilityOfReturningTrue)
{
	float result = RollRandomFloatZeroToOne();
	return (result <= probabilityOfReturningTrue);
}

void SquirrelRNG::RollRandomDirection2D(float& outX, float& outY)
{
	outX = RollRandomFloatNegativeOneToOne();
	outY = RollRandomFloatNegativeOneToOne();
}
