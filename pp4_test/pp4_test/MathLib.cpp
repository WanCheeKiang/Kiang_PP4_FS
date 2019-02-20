#include "MathLib.h"



MathLib::MathLib()
{
}


MathLib::~MathLib()
{
}

float MathLib::Clamp(float Min, float Max, float value)
{
	if (value > Max)
		return Max;
	else if (value < Min)
		return Min;
	return value;
}
