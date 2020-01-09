#ifndef MATHS_H
#define MATHS_H
#include <cstdint>

/// <summary>
/// Unsigned 32-bit LEB decoder. Takes in 4 bytes, returns an int. <br>
/// See https://en.wikipedia.org/wiki/LEB128 for the algorithm
/// </summary>
inline uint32_t u_decLEB32(char * bytes)
{
	// We assume that it is made of 4 bytes because it is 32 bit
	uint32_t result = 0;
	int shift = 0;
	unsigned char byte;

	for(int i = 0; i < 4; ++i)
	{
		byte = bytes[i];
		result |= (((1 << 7) - 1) & byte) << shift;
		if (((1 << 7) & byte) == 0)
			break;
		shift += 7;
	}

	return result;
}

/// <summary>
/// Unsigned 64-bit LEB decoder. Takes in 8 bytes, returns an int <br>
/// See https://en.wikipedia.org/wiki/LEB128 for pseudo-code</summary>
inline uint64_t u_decLEB64(char * bytes)
{
	// Same as u32, but now with 8 bytes
	uint64_t result = 0;
	int shift = 0;
	unsigned char byte;

	for(int i = 0; i < 8; ++i)
	{
		byte = bytes[i];
		result |= (((1 << 7) - 1) & (int)byte) << shift;
		if (((1 << 7) & byte) == 0)
			break;
		shift += 7;
	}
	return result;
}

inline int32_t s_decLEB32(char * bytes)
{
	int32_t result = 0;
	int shift = 0;

	int size = 32;
	int i = 0;
	unsigned char byte;

	do
	{
		byte = bytes[i];
		result |= (((1 << 7) - 1) & byte) << shift;
		shift += 7;
	}
	while (((1 << 7) & byte) != 0);

	if((shift < size) && ((byte & 0x40) == 64))
	{
		result |= (~0 << shift);
	}

	return result;
}

/// <summary>Function used to decode 4 bytes into a float
/// in little endian format.
/// <br> See: https://stackoverflow.com/a/3991555 
/// </summary>
inline float decodeFloat(char* bytes)
{
	// We are only expecting 4 bytes
	float ret;
	unsigned char b[] = { bytes[0], bytes[1], bytes[2], bytes[3] };
	memcpy(&ret, &b, sizeof(ret));
	return ret;
}

/// <summary>Function used to decode 8 bytes into a double
/// from a Little Endian byte order.
/// <br> See: https://stackoverflow.com/a/3991555
/// </summary>
inline double decodeDouble(char* bytes)
{
	double d;
	unsigned char b[] = { bytes[0], bytes[1], bytes[2], bytes[3], // Recast to provide length limits
						  bytes[4], bytes[5], bytes[6], bytes[7] };
	memcpy(&d, &b, sizeof(d));
	return d;
}


/// <summary>Given a Byte string, we can figure out the bytes in the next u32
/// number.</summary>
inline int nextu32Length(std::vector<unsigned char> in, int offset)
{
	// Grab the first byte. If it is less than 0x80, we're done.
	// Continue for up to 4 bytes max
	for(int i = offset; i < offset + 4; ++i)
	{
		unsigned char byte = in[i];
		if (byte < 0x80)
			return i - offset + 1;
	}
	return 4;
}

/// <summary>Grab the next instance of a u32 number. Returns the number of bytes it occupied.
/// The value is passed back by reference. </summary>
inline int getNextu32(std::vector<unsigned char> in, int offset, int& value)
{
	int length = nextu32Length(in, offset);

	// Knowing the length, extract those bytes and pass it to the decoder.
	char temp[4] = {static_cast<char>(0x80), static_cast<char>(0x80), static_cast<char>(0x80), static_cast<char>(0x80)};
	
	for(int i = offset; i < offset + length; ++i)
	{
		temp[i - offset] = in[i];
	}
	value = u_decLEB32(temp);

	return length;
}

#endif // MATHS_H