#pragma once
// Created by Billy Graban
/** \file SquirrelRNG.h */
/** RNG based on Squirrel3 Noise-Based number generation */

typedef unsigned int u32;
typedef int i32;
typedef unsigned short u16;
typedef unsigned char u8;

/** \class SquirrelRNG */
/** RNG based on Squirrel3 Noise-Based number generation */
class SquirrelRNG
{
public:
	// --------------------------------------------------------------------- //
	// Public Member Variables
	// --------------------------------------------------------------------- //


	// --------------------------------------------------------------------- //
	// Public Member Functions
	// --------------------------------------------------------------------- //
	/** Constructor */
	/** \param seed The seed to use with the generator */
	SquirrelRNG(u32 seed = 0);

	/** Default destructor */
	~SquirrelRNG();

	/** Resets the seed and position */
	/** \param seed The seed to use */
	/** \param position The starting position (default 0) */
	void ResetSeed(u32 seed, i32 position = 0);

	/** Returns the current seed value */
	u32		GetSeed() const						{ return m_seed; }
	/** Sets the current position */
	void	SetCurrentPosition(int position)	{ m_position = position; }
	/** Returns the current position */
	int		GetCurrentPosition()				{ return m_position; }

	// Each of the following advances the position of the RNG by 1
	u32 RollRandomUint32();															/**< Return an unsigned 32 bit integer */
	u16 RollRandomUint16();															/**< Return an unsigned 16 bit integer */
	u8 RollRandomByte();															/**< Return an unsigned 8 bit integer */
	u32 RollRandomLessThan(u32 maxValueNotInclusive);								/**< Return between 0 and (maxValue - 1) */
	i32 RollRandomIntInRange(i32 minValueInclusive, i32 maxValueInclusive);			/**< Return between min and max (inclusive) */
	float RollRandomFloatZeroToOne();												/**< Return betwee 0.0 and 1.0 (inclusive) */
	float RollRandomFloatInRange(float minValueInclusive, float maxValueInclusive); /**< Return between min and max (inclusive) */
	float RollRandomFloatNegativeOneToOne();										/**< Return between -1.0 and 1.0 (inclusive) */
	bool RollRandomChance(float probabilityOfReturningTrue);						/**< Return true if roll is less than probability.  Probability assumed to be (0.0 - 1.0) */
	void RollRandomDirection2D(float& outX, float& outY);							/**< Return two floats between -1.0 and 1.0.  Advances position twice */

private:
	// --------------------------------------------------------------------- //
	// Private Member Variables
	// --------------------------------------------------------------------- //
	u32 m_seed;
	i32 m_position;

	// --------------------------------------------------------------------- //
	// Private Member Functions
	// --------------------------------------------------------------------- //
	/** Returns an unsigned integer containing 32 reasonably-well-scrambled bits. */
	/** Based on a given (signed) integer input parameter (position/index) and */
	/** [optional] seed.  This was taken from Squirrel Eiserloh's GDC 17 talk: */
	/** "Math for Game Programmers: Noise-Based RNG" */
	u32 Get1DNoiseUint(i32 positionX, u32 seed);


public:
	// --------------------------------------------------------------------- //
	// Accessors & Mutators
	// --------------------------------------------------------------------- //


};