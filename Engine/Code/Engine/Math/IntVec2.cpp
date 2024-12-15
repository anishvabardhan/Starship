#include "IntVec2.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <math.h>

IntVec2 const IntVec2::ZERO			= IntVec2(0, 0);
IntVec2 const IntVec2::ONE			= IntVec2(1, 1);
IntVec2 const IntVec2::NORTH		= IntVec2(0, 1);
IntVec2 const IntVec2::SOUTH		= IntVec2(0, -1);
IntVec2 const IntVec2::EAST			= IntVec2(1, 0);
IntVec2 const IntVec2::WEST			= IntVec2(-1, 0);
IntVec2 const IntVec2::NORTH_EAST	= IntVec2(1, 1);
IntVec2 const IntVec2::NORTH_WEST	= IntVec2(1, -1);
IntVec2 const IntVec2::SOUTH_EAST	= IntVec2(-1, 1);
IntVec2 const IntVec2::SOUTH_WEST	= IntVec2(-1, -1);

IntVec2::IntVec2(IntVec2 const& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}

IntVec2::IntVec2(int initialX, int initialY)
{
	x = initialX;
	y = initialY;
}

float IntVec2::GetLength() const
{
	float length = sqrtf(static_cast<float>((x * x) + (y * y)));
	return length;
}

int IntVec2::GetTaxicabLength() const
{
	int length = abs(x) + abs(y);
	return length;
}

int IntVec2::GetLengthSquared() const
{
	int length = (x * x) + (y * y);
	return length;
}

float IntVec2::GetOrientationRadians() const
{
	float degrees = Atan2Degrees(static_cast<float>(y), static_cast<float>(x));

	float radians = ConvertDegreesToRadians(degrees);

	return radians;
}

float IntVec2::GetOrientationDegrees() const
{
	float degrees = Atan2Degrees(static_cast<float>(y), static_cast<float>(x));

	return degrees;
}

IntVec2 const IntVec2::GetRotated90Degrees() const
{
	return IntVec2(-y, x);
}

IntVec2 const IntVec2::GetRotatedMinus90Degrees() const
{
	return IntVec2(y, -x);
}

void IntVec2::Rotate90Degrees()
{
	IntVec2 temp = IntVec2(-y, x);

	x = temp.x;
	y = temp.y;
}

void IntVec2::RotateMinus90Degrees()
{
	IntVec2 temp = IntVec2(y, -x);

	x = temp.x;
	y = temp.y;
}

IntVec2 IntVec2::SetFromText(char const* text)
{
	Strings splitValues = SplitStringOnDelimiter(text, ',');

	int xValue = std::atoi(splitValues[0].c_str());
	int yValue = std::atoi(splitValues[1].c_str());

	x = xValue;
	y = yValue;

	return IntVec2(x, y);
}

IntVec2 const IntVec2::operator+(IntVec2 const& copyFrom)
{
	return IntVec2(x + copyFrom.x, y + copyFrom.y);
}

IntVec2 const IntVec2::operator-(IntVec2 const& copyFrom)
{
	return IntVec2(x - copyFrom.x, y - copyFrom.y);
}

bool IntVec2::operator==(IntVec2 const& compare) const
{
	if (x == compare.x && y == compare.y)
		return true;

	return false;
}

bool IntVec2::operator!=(const IntVec2& compare) const
{
	return compare.x != x || compare.y != y;
}

void IntVec2::operator=(IntVec2 const& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}

bool operator<(IntVec2 const& a, IntVec2 const& b)
{
	if (a.y < b.y)
	{
		return true;
	}
	else if (b.y < a.y)
	{
		return false;
	}
	else
	{
		return a.x < b.x;
	}
}

std::size_t IntVec2Hash::operator()(const IntVec2& v) const noexcept // Hash function for IntVec2
{
	return std::hash<int>()(v.x) ^ (std::hash<int>{}(v.x) << 1); // Combine the hash of x and y to generate a hash for IntVec2
}

std::size_t IntVec2PairHash::operator()(const std::pair<IntVec2, IntVec2>& pair) const noexcept
{
	return IntVec2Hash()(pair.first) ^ (IntVec2Hash()(pair.second) << 1);
}

bool IntVec2PairEqual::operator()(const std::pair<IntVec2, IntVec2>& leftHash, const std::pair<IntVec2, IntVec2>& rightHash) const noexcept
{
	return leftHash.first == rightHash.first && leftHash.second == rightHash.second;
}