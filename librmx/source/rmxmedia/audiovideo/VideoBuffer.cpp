/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxmedia.h"


// Tables for YUV -> RGB conversion
bool VideoBuffer::mConversionTablesInitialized = false;
uint32 VideoBuffer::mYTable[256];
uint32 VideoBuffer::mRVTable[256];
uint32 VideoBuffer::mGUTable[256];
uint32 VideoBuffer::mGVTable[256];
uint32 VideoBuffer::mBUTable[256];


VideoBuffer::VideoBuffer() :
	mWidth(0),
	mWidthUV(0),
	mHeight(0),
	mHeightUV(0),
	mFramesPerSecond(25.0f)
{
	for (int i = 0; i < 4; ++i) mCropRect[i] = 0.0f;
}

VideoBuffer::~VideoBuffer()
{
	clear();
}

void VideoBuffer::clear()
{
	for (int i = 0; i < (signed)mFrames.size(); ++i)
		deleteFrame(mFrames[i]);
	mFrames.clear();
	mWidth = 0;
	mWidthUV = 0;
	mHeight = 0;
	mHeightUV = 0;
}

void VideoBuffer::clear(int width, int height)
{
	clear(width, height, width/2, height/2);
}

void VideoBuffer::clear(int width, int height, int widthUV, int heightUV)
{
	clear();
	mWidth = width;
	mHeight = height;
	mWidthUV = widthUV;
	mHeightUV = heightUV;
}

void VideoBuffer::addImageRGBA(uint32* data, int stride)
{
	if (stride <= 0)
		stride = mWidth;
	VideoFrame* frame = new VideoFrame;
	frame->bufferRGBA = new uint32[mWidth*mHeight];
	for (int line = 0; line < mHeight; ++line)
		memcpy(&frame->bufferRGBA[line*mWidth], &data[line*stride], mWidth*sizeof(uint32));
	frame->bufferYUV[0] = 0;
	frame->bufferYUV[1] = 0;
	frame->bufferYUV[2] = 0;
	mFrames.push_back(frame);
}

void VideoBuffer::addImageYUV(uint8* dataY, uint8* dataU, uint8* dataV, int strideY, int strideUV)
{
	if (strideY <= 0)
		strideY = mWidth;
	if (strideUV <= 0)
		strideUV = mWidthUV;

	VideoFrame* frame = new VideoFrame;
	frame->bufferRGBA = 0;
	frame->bufferYUV[0] = new uint8[mWidth*mHeight];
	frame->bufferYUV[1] = new uint8[mWidthUV*mHeightUV];
	frame->bufferYUV[2] = new uint8[mWidthUV*mHeightUV];

	for (int line = 0; line < mHeight; ++line)
		memcpy(&frame->bufferYUV[0][line*mWidth], &dataY[line*strideY], mWidth);
	for (int line = 0; line < mHeightUV; ++line)
	{
		memcpy(&frame->bufferYUV[1][line*mWidthUV], &dataU[line*strideUV], mWidthUV);
		memcpy(&frame->bufferYUV[2][line*mWidthUV], &dataV[line*strideUV], mWidthUV);
	}
	mFrames.push_back(frame);
}

const uint32* VideoBuffer::getImageRGBA(int num)
{
	if (num < 0 || num >= (signed)mFrames.size())
		return 0;

	if (0 == mFrames[num]->bufferRGBA)
		convertYUVtoRGBA(num);
	return mFrames[num]->bufferRGBA;
}

const uint8* VideoBuffer::getImageY(int num)
{
	return getImageYUV(num, 0);
}

const uint8* VideoBuffer::getImageU(int num)
{
	return getImageYUV(num, 1);
}

const uint8* VideoBuffer::getImageV(int num)
{
	return getImageYUV(num, 2);
}

const uint8* VideoBuffer::getImageYUV(int num, int channel)
{
	if (num < 0 || num >= (signed)mFrames.size())
		return 0;
	return mFrames[num]->bufferYUV[channel];
}

void VideoBuffer::convertYUVtoRGBA(int num)
{
	if (num < 0 || num >= (signed)mFrames.size())
		return;

	VideoFrame* frame = mFrames[num];
	if (nullptr != frame->bufferRGBA || nullptr == frame->bufferYUV[0])
		return;

	// Lazy initialization of conversion tables
	if (!mConversionTablesInitialized)
	{
		for (int i = 0; i < 256; ++i)
		{
			mYTable[i]  = clamp(roundToInt(1.164f * (float)(i-16)), 0, 255);
			mRVTable[i] = roundToInt(1.596f * (float)(i-128));
			mGUTable[i] = roundToInt(0.391f * (float)(i-128));
			mGVTable[i] = roundToInt(0.813f * (float)(i-128));
			mBUTable[i] = roundToInt(2.018f * (float)(i-128));
		}
		mConversionTablesInitialized = true;
	}

	frame->bufferRGBA = new uint32[mWidth*mHeight];
	for (int y = 0; y < mHeight; ++y)
	{
		for (int x = 0; x < mWidth; ++x)
		{
			const int Y = frame->bufferYUV[0][y*mWidth+x];
			const int U = frame->bufferYUV[1][(y*mHeightUV/mHeight)*mWidthUV+(x*mWidthUV/mWidth)];
			const int V = frame->bufferYUV[2][(y*mHeightUV/mHeight)*mWidthUV+(x*mWidthUV/mWidth)];

			const int r = clamp((int)mYTable[Y] + mRVTable[V], 0, 255);
			const int g = clamp((int)mYTable[Y] - mGUTable[U] - mGVTable[V], 0, 255);
			const int b = clamp((int)mYTable[Y] + mBUTable[U], 0, 255);

			frame->bufferRGBA[y*mWidth+x] = 0xff000000 | (b << 16) | (g << 8) | r;
		}
	}
}

void VideoBuffer::getCropRect(float* croprect)
{
	for (int i = 0; i < 4; ++i)
		croprect[i] = mCropRect[i];
}

void VideoBuffer::setCropRect(float left, float top, float right, float bottom)
{
	mCropRect[0] = left;
	mCropRect[1] = top;
	mCropRect[2] = right;
	mCropRect[3] = bottom;
}

void VideoBuffer::setCropRect(int left, int top, int width, int height)
{
	setCropRect((float)left / (float)mWidth, (float)top / (float)mHeight, (float)(mWidth-left-width) / (float)mWidth, (float)(mHeight-top-height) / (float)mHeight);
}

void VideoBuffer::setObsoleteFrames(int count)
{
	if (count <= 0)
		return;

	count = std::min(count, (int)mFrames.size());
	for (int i = 0; i < count; ++i)
		deleteFrame(mFrames[i]);
	mFrames.erase(mFrames.begin(), mFrames.begin() + count);
}

void VideoBuffer::deleteFrame(VideoFrame* frame)
{
	if (nullptr == frame)
		return;
	if (nullptr != frame->bufferRGBA)
		delete[] frame->bufferRGBA;
	for (int k = 0; k < 3; ++k)
		if (nullptr != frame->bufferYUV[k])
			delete[] frame->bufferYUV[k];
	delete frame;
}
