//====================================================================================================
// Filename:	MemoryStream.h
// Created by:	Peter Chan
//====================================================================================================

#pragma once

namespace Network
{
	class MemoryStream
	{
	public:
		MemoryStream();
		MemoryStream(uint32_t capacity);
		MemoryStream(const uint8_t* data, uint32_t size, bool takeOwnership = false);
		~MemoryStream();

		void Reset();

		const uint8_t* GetData() const { return mBuffer; }
		uint32_t GetSize() const { return mHead; }
		uint32_t GetCapacity() const { return mCapacity; }

	private:
		MemoryStream(const MemoryStream&) = delete;
		MemoryStream& operator=(const MemoryStream&) = delete;

		friend class StreamWriter;
		friend class StreamReader;

		void ReallocBuffer(uint32_t newCapacity);

		uint8_t*	mBuffer;
		uint32_t	mHead;
		uint32_t	mCapacity;
	};

} // namespace Network
