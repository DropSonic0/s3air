/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxbase.h"


const Color Color::BLACK  (0.0f, 0.0f, 0.0f);
const Color Color::GRAY   (0.5f, 0.5f, 0.5f);
const Color Color::WHITE  (1.0f, 1.0f, 1.0f);
const Color Color::RED    (1.0f, 0.0f, 0.0f);
const Color Color::YELLOW (1.0f, 1.0f, 0.0f);
const Color Color::GREEN  (0.0f, 1.0f, 0.0f);
const Color Color::CYAN   (0.0f, 1.0f, 1.0f);
const Color Color::BLUE   (0.0f, 0.0f, 1.0f);
const Color Color::MAGENTA(1.0f, 0.0f, 1.0f);
const Color Color::TRANSPARENT(0.0f, 0.0f, 0.0f, 0.0f);


Color Color::interpolateColor(const Color& c0, const Color& c1, float factor)
{
	Color result;
	result.interpolate(c0, c1, factor);
	return result;
}


uint32 Color::getRGBA32() const
{
	return ((uint32)(::saturate(r) * 255) << 24)
		 + ((uint32)(::saturate(g) * 255) << 16)
		 + ((uint32)(::saturate(b) * 255) << 8)
		 + ((uint32)(::saturate(a) * 255));
}

uint32 Color::getARGB32() const
{
	return ((uint32)(::saturate(r) * 255) << 16)
		 + ((uint32)(::saturate(g) * 255) << 8)
		 + ((uint32)(::saturate(b) * 255))
		 + ((uint32)(::saturate(a) * 255) << 24);
}

uint32 Color::getABGR32() const
{
#if defined(PLATFORM_PS3)
	// On PS3, we use ARGB as the 32-bit pixel format
	return ((uint32)(::saturate(a) * 255) << 24)
		 + ((uint32)(::saturate(r) * 255) << 16)
		 + ((uint32)(::saturate(g) * 255) << 8)
		 + ((uint32)(::saturate(b) * 255));
#else
	return ((uint32)(::saturate(r) * 255))
		 + ((uint32)(::saturate(g) * 255) << 8)
		 + ((uint32)(::saturate(b) * 255) << 16)
		 + ((uint32)(::saturate(a) * 255) << 24);
#endif
}

Color::Color(uint32 color, Encoding_t encoding) : Vec4f(Uninitialized)
{
	setByEncoding(color, encoding);
}

void Color::setByEncoding(uint32 color, Encoding_t encoding)
{
	switch (encoding)
	{
		case (Encoding_t)Encoding::RGBA_32:  setRGBA32(color);  break;
		case (Encoding_t)Encoding::ARGB_32:  setARGB32(color);  break;
		case (Encoding_t)Encoding::ABGR_32:  setABGR32(color);  break;
	}
}

void Color::setRGBA32(uint32 colorRGBA)
{
	r = (float)((colorRGBA >> 24) & 0xff) / 255.0f;
	g = (float)((colorRGBA >> 16) & 0xff) / 255.0f;
	b = (float)((colorRGBA >> 8)  & 0xff) / 255.0f;
	a = (float)((colorRGBA)       & 0xff) / 255.0f;
}

void Color::setARGB32(uint32 colorARGB)
{
	r = (float)((colorARGB >> 16) & 0xff) / 255.0f;
	g = (float)((colorARGB >> 8)  & 0xff) / 255.0f;
	b = (float)((colorARGB)       & 0xff) / 255.0f;
	a = (float)((colorARGB >> 24) & 0xff) / 255.0f;
}

void Color::setABGR32(uint32 colorABGR)
{
#if defined(PLATFORM_PS3)
	// On PS3, we use ARGB as the 32-bit pixel format
	a = (float)((colorABGR >> 24) & 0xff) / 255.0f;
	r = (float)((colorABGR >> 16) & 0xff) / 255.0f;
	g = (float)((colorABGR >> 8)  & 0xff) / 255.0f;
	b = (float)((colorABGR)       & 0xff) / 255.0f;
#else
	r = (float)((colorABGR)       & 0xff) / 255.0f;
	g = (float)((colorABGR >> 8)  & 0xff) / 255.0f;
	b = (float)((colorABGR >> 16) & 0xff) / 255.0f;
	a = (float)((colorABGR >> 24) & 0xff) / 255.0f;
#endif
}

void Color::setFromHSL(const Vec3f& hsl)
{
	// Conversion HSL -> RGB
	float hue = hsl.x / 360.0f;
	hue -= std::floor(hue);
	hue *= 6.0f;

	const float saturation = ::saturate(hsl.y);
	const float lightness = ::saturate(hsl.z);

	const float maximum = (lightness < 0.5f) ? (lightness * (1.0f + saturation)) : (saturation + lightness * (1.0f - saturation));
	const float minimum = 2.0f * lightness - maximum;

	r = (hue < 4.0f) ? (hue + 2.0f) : (hue - 4.0f);
	g = hue;
	b = (hue < 2.0f) ? (hue + 4.0f) : (hue - 2.0f);

	for (int i = 0; i < 3; ++i)
	{
		if (data[i] < 1.0f)
			data[i] = minimum + (maximum - minimum) * data[i];
		else if (data[i] < 3.0f)
			data[i] = maximum;
		else if (data[i] < 4.0f)
			data[i] = minimum + (maximum - minimum) * (4.0f - data[i]);
		else
			data[i] = minimum;
	}
}

Vec3f Color::getHSL() const
{
	// Conversion RGB -> HSL
	float minimum = std::min(std::min(r, g), b);
	float maximum = std::max(std::max(r, g), b);
	float delta = maximum - minimum;
	if (delta < 0.001f)
	{
		return Vec3f(0.0f, 0.0f, maximum);
	}

	float sum = minimum + maximum;
	float lightness = sum / 2.0f;
	float saturation = delta / ((sum < 1.0f) ? sum : (2.0f - sum));

	float hue;
	if (maximum == r)
		hue = (g - b) / delta * 60.0f + ((g < b) ? 360.0f : 0.0f);	// Purple to red to yellow (crossing the 0° / 360° line)
	else if (maximum == g)
		hue = (b - r) / delta * 60.0f + 120.0f;						// Yellow to green to cyan
	else
		hue = (r - g) / delta * 60.0f + 240.0f;						// Cyan to blue to purple

	return Vec3f(hue * 60.0f, saturation, lightness);
}

void Color::setFromHSV(const Vec3f& hsv)
{
	// Conversion HSV -> RGB
	float hue = hsv.x / 360.0f;
	hue -= std::floor(hue);
	hue *= 6.0f;

	const float saturation = ::saturate(hsv.y);
	const float value = ::saturate(hsv.z);

	const float delta = value * saturation;
	const float maximum = value;
	const float minimum = maximum - delta;

	const int region = (int)std::floor(hue);
	const float fraction = hue - (float)region;

	switch (region)
	{
		case 0:   r = maximum;  b = minimum;  g = minimum + delta * fraction;  break;	// Red to yellow
		case 1:   g = maximum;  b = minimum;  r = maximum - delta * fraction;  break;	// Yellow to green
		case 2:   g = maximum;  r = minimum;  b = minimum + delta * fraction;  break;	// Green to cyan
		case 3:   b = maximum;  r = minimum;  g = maximum - delta * fraction;  break;	// Cyan to blue
		case 4:   b = maximum;  g = minimum;  r = minimum + delta * fraction;  break;	// Blue to purple
		default:  r = maximum;  g = minimum;  b = maximum - delta * fraction;  break;	// Purple to red
	}
}

Vec3f Color::getHSV() const
{
	// Conversion RGB -> HSV
	const float minimum = std::min(std::min(r, g), b);
	const float maximum = std::max(std::max(r, g), b);
	const float delta = maximum - minimum;
	if (delta < 0.001f)
	{
		return Vec3f(0.0f, 0.0f, maximum);
	}

	const float value = maximum;
	const float saturation = delta / maximum;

	float hue;
	if (maximum == r)
		hue = (g - b) / delta * 60.0f + ((g < b) ? 360.0f : 0.0f);	// Purple to red to yellow (crossing the 0° / 360° line)
	else if (maximum == g)
		hue = (b - r) / delta * 60.0f + 120.0f;						// Yellow to green to cyan
	else
		hue = (r - g) / delta * 60.0f + 240.0f;						// Cyan to blue to purple

	return Vec3f(hue, saturation, value);
}

void Color::setFromYUV(const Vec3f& yuv)
{
	// Conversion YUV -> RGB
	r = yuv.x + yuv.z * 1.14f;
	g = yuv.x - yuv.y * 0.39466f - yuv.z * 0.5806f;
	b = yuv.x + yuv.y * 2.028f;
}

Vec3f Color::getYUV() const
{
	// Conversion RGB -> YUV
	Vec3f yuv;
	yuv.x = 0.299f * r + 0.587f * g + 0.114f * b;
	yuv.y = (b - yuv.x) * 0.493f;
	yuv.z = (r - yuv.x) * 0.877f;
	return yuv;
}

void Color::swapRedBlue()
{
	std::swap(x, z);
}

Color Color::blendOver(const Color& dest) const
{
	Color result;
	result.a = 1.0f - (1.0f - a) * (1.0f - dest.a);
	if (result.a > 0.0f)
	{
		float fs = a / result.a;
		float fd = dest.a * (1.0f - a) / result.a;
		result.r = r * fs + dest.r * fd;
		result.g = g * fs + dest.g * fd;
		result.b = b * fs + dest.b * fd;
	}
	return result;
}

void Color::serialize(VectorBinarySerializer& serializer)
{
	if (serializer.isReading())
	{
		uint32 abgr = serializer.read<uint32>();
#if defined(PLATFORM_PS3)
		// On PS3, ABGR32 was (a<<24)|(r<<16)|(g<<8)|b but on others it was r|(g<<8)|(b<<16)|(a<<24)
		// To maintain compatibility with Little-Endian save states, we must interpret the serialized 
		// uint32 as Little-Endian ABGR (a|b|g|r in memory) and convert to our internal representation.
		
		// The VectorBinarySerializer already swapped bytes if host is BE, so we have the LE value here.
		// LE value: a is highest byte, b, g, r is lowest. 
		// We want to call setABGR32 which on PS3 expects (a<<24)|(r<<16)|(g<<8)|b.
		uint32 converted = ((abgr & 0xff000000))        // Alpha
						 | ((abgr & 0x000000ff) << 16)  // Red
						 | ((abgr & 0x0000ff00))        // Green
						 | ((abgr & 0x00ff0000) >> 16); // Blue
		setABGR32(converted);
#else
		setABGR32(abgr);
#endif
	}
	else
	{
		uint32 abgr = getABGR32();
#if defined(PLATFORM_PS3)
		// Convert from internal PS3 ABGR32 (a<<24)|(r<<16)|(g<<8)|b
		// to standard LE ABGR32 (a<<24)|(b<<16)|(g<<8)|r
		uint32 converted = ((abgr & 0xff000000))        // Alpha
						 | ((abgr & 0x00ff0000) >> 16)  // Red
						 | ((abgr & 0x0000ff00))        // Green
						 | ((abgr & 0x000000ff) << 16); // Blue
		serializer.write<uint32>(converted);
#else
		serializer.write<uint32>(abgr);
#endif
	}
}

Color Color::operator+(const Color& other) const
{
	return Color(::saturate(r + other.r), ::saturate(g + other.g), ::saturate(b + other.b), ::saturate(a + other.a));
}

Color Color::operator-(const Color& other) const
{
	return Color(::saturate(r - other.r), ::saturate(g - other.g), ::saturate(b - other.b), ::saturate(a - other.a));
}

Color Color::operator*(const Color& other) const
{
	return Color(::saturate(r * other.r), ::saturate(g * other.g), ::saturate(b * other.b), ::saturate(a * other.a));
}

Color Color::operator*(float factor) const
{
	return Color(::saturate(r * factor), ::saturate(g * factor), ::saturate(b * factor), ::saturate(a * factor));
}

const Color& Color::operator+=(const Color& other)
{
	r += other.r;
	g += other.g;
	b += other.b;
	a += other.a;
	saturate();
	return *this;
}

const Color& Color::operator-=(const Color& other)
{
	r -= other.r;
	g -= other.g;
	b -= other.b;
	a -= other.a;
	saturate();
	return *this;
}

const Color& Color::operator*=(const Color& other)
{
	r *= other.r;
	g *= other.g;
	b *= other.b;
	a *= other.a;
	saturate();
	return *this;
}

const Color& Color::operator*=(float factor)
{
	r *= factor;
	g *= factor;
	b *= factor;
	a *= factor;
	saturate();
	return *this;
}
