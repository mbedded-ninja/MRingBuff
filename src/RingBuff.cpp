//!
//! @file 				RingBuff.cpp
//! @author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.cladlab.com)
//! @created 			2013-07-30
//! @last-modified		2014-07-29
//! @brief 				Implements the ring buffer.
//! @details
//!						See README.rst in root dir for more info.

#ifndef __cplusplus
	#error Please build with C++ compiler
#endif

//===============================================================================================//
//========================================= INCLUDES ============================================//
//===============================================================================================//

// System libraries
//#include <iostream>		// debug
#include <stdint.h>		// uint32_t, e.t.c
#include <stdlib.h>		// calloc();

// User includes
#include "../include/Config.hpp"
#include "../include/RingBuff.hpp"

//===============================================================================================//
//======================================== NAMESPACE ============================================//
//===============================================================================================//

namespace RingBuffNs
{
	
	//===============================================================================================//
	//================================ PUBLIC METHOD DECLARATIONS ===================================//
	//===============================================================================================//

	RingBuff::RingBuff(uint32_t capacity):
		capacity(capacity),
		headPos(0),
		tailPos(0),
		numElements(0)
	{			
		// Create space for buffer, also 0 buffer (although
		// not strictly needed)
		this->buffMemPtr = (uint8_t*)calloc(capacity, sizeof(char));
		
		if(this->buffMemPtr == NULL)
		{
			// Memory allocation failed
			this->isInitSuccess = false;
			return;
		}
		
		// Init completed successfully
		this->isInitSuccess = true;
	}
	
	RingBuff::~RingBuff()
	{
		if(!this->isInitSuccess)
			return;

		// Free memory allocated in constructor
		free(this->buffMemPtr);
	}

	bool RingBuff::IsInitSuccess() const
	{
		return this->isInitSuccess;
	}
 
	uint32_t RingBuff::Write(const uint8_t *buff, uint32_t numBytes, ReadWriteLogic writeLogic)
	{
		if(!this->isInitSuccess)
			return 0;

		//std::cout << "numBytes = '" << numBytes << "' .numElements = '" << this->numElements << "'." << std::endl;

		// Check whether user only wants to write data if all data
		// will fit in buffer
		if(writeLogic == ReadWriteLogic::ALL)
		{
			if(numBytes > (this->capacity - this->numElements))
			{
				// Not enough space in buffer to write all elements,
				// return 0
				//! @debug
				//std::cout << "numBytes '" << numBytes << "' > numElements '" << this->numElements << "'." << std::endl;
				return 0;
			}
		}

		//std::cout << "Code makes it to 2!" << std::endl;
	
		int i;
		const uint8_t * currPos;
		currPos = buff;
		 
		for(i = 0; i < numBytes; i++)
		{
			if(this->numElements >= this->capacity)
			{
				// We have run out of space! Return how many bytes
				// we managed to write
				//std::cout << "Run out of space!" << std::endl;
				return i;
			}

			// Write one byte to buffer
			buffMemPtr[this->headPos] = *currPos++;

			//! @debug
			//std::cout << "Element '" << buffMemPtr[this->headPos] << "' written to buff pos '" << this->headPos << "'." << std::endl;

			// Increment the head
			this->headPos++;

			// Increment the number of elements
			this->numElements++;

			// Check for wrap-around
			if(this->headPos == this->capacity)
			{
				headPos = 0;
			}

		}
		
		// All bytes where written
		return numBytes;
	}
	
	uint32_t RingBuff::Write(const uint8_t *buff, uint32_t numBytes)
	{
		// Simplified overload of Write(const uint8_t *buff, uint32_t numBytes, ReadWriteLogic writeLogic),
		// default behaviour is to only write to buffer if all elements will fit
		return this->Write(buff, numBytes, ReadWriteLogic::ALL);
	}

	uint32_t RingBuff::Write(const char *string, ReadWriteLogic writeLogic)
	{
		if(!this->isInitSuccess)
			return 0;

		bool nullFound = false;
		uint32_t x;

		// Null character can be one more element past capacity, since
		// it won't be written to the buffer
		for(x = 0; x < this->capacity + 1; x++)
		{
			// Look for null-terminating string.
			if(string[x] == '\0')
			{
				nullFound = true;
				break;
			}
		}

		if(nullFound)
		{
			// Null has been found with limits of buffer capacity, valid string, write to buffer
			std::cout << "Null found! Writing '" << x << "' elements." << std::endl;
			return this->Write((const uint8_t*)string, x, writeLogic);
		}
		else
		{
			// Null has not be found within limits of buffer capacity, either string was not null-terminated, or was too large
			// to fit in buffer completely.
			std::cout << "Null not found!" << std::endl;

			// In this case, only write stuff if ReadWriteLogic is ANY
			if(writeLogic == ReadWriteLogic::ANY)
				return this->Write((const uint8_t*)string, this->capacity, writeLogic);
			else if(writeLogic == ReadWriteLogic::ALL)
				// Since we know the string cannot fit in the buffer, return 0 here
				return 0;
		}

		// If code gets here something bad has happened
		return 0;
	}

	uint32_t RingBuff::Write(const char *string)
	{
		// Simplified overload of Write(const uint8_t *buff, uint32_t numBytes, ReadWriteLogic writeLogic),
		// default behaviour is to only write to buffer if all elements will fit
		return this->Write(string, ReadWriteLogic::ALL);
	}

	uint32_t RingBuff::Read(uint8_t *buff, uint32_t numBytes)
	{
		if(!this->isInitSuccess)
			return 0;

		uint32_t i;
		uint8_t *currPos;
		currPos = buff;

		for(i = 0; i < numBytes; i++)
		{
			//
			// Check if any data is available
			if(this->numElements == 0)
			{
				// No more data available, return the number of elements
				// we managed to read
				return i;
			}

			// Read one byte from the FIFO buffer
			*currPos++ = this->buffMemPtr[this->tailPos];

			// Decrement the number of elements
			this->numElements--;

			 // Increment the tail
			this->tailPos++;

			// Check for wrap-around
			if(this->tailPos == this->capacity)
			{
				// Reset tail
				this->tailPos = 0;
			}
		}

		// All bytes were read
		return numBytes;
	}

	uint8_t RingBuff::Read()
	{
		if(!this->isInitSuccess)
			return 0;

		if(!this->IsData())
			return 0;

		// There is data present, so read out
		// 1 element
		uint8_t data;
		RingBuff::Read(&data, 1);

		// Return data element (note that this could be 0)
		return data;
	}

	void RingBuff::Clear()
	{
		if(!this->isInitSuccess)
			return;

		// Does not 0 data, as this does not matter,
		// Just resets tail, head and number of elements
		this->tailPos = 0;
		this->headPos = 0;
		this->numElements = 0;
	}

	uint32_t RingBuff::Capacity() const
	{
		if(!this->isInitSuccess)
			return 0;

		// Just return the saved size of the buffer
		return this->capacity;
	}


	uint32_t RingBuff::NumElements() const
	{
		if(!this->isInitSuccess)
			return 0;

		return this->numElements;
	}

	bool RingBuff::IsData() const
	{
		if(!this->isInitSuccess)
			return 0;

		// Return true if there are any data elements currently in the buffer
		if(this->numElements > 0)
			return true;
		else
			return false;
	}

	bool RingBuff::Resize(uint32_t newCapacity)
	{
		if(!this->isInitSuccess)
			return 0;

		// First, shuffle all data backwards so that tailPos is at 0
		// this makes resizing easier
		this->ShiftElementsSoTailPosIsZero();

		// Now realloc() the memory. realloc() is guaranteed to preserve as much data as possible
		// Since elements have been shifted to start of buffer this should work as intended
		this->buffMemPtr = (uint8_t*)realloc(this->buffMemPtr, newCapacity);

		// Make sure realloc() was successful
		if(this->buffMemPtr != NULL)
		{
			// Update headPos if old headpos is larger than new capacity, as headPos will be
			// currently out of range
			if(this->numElements > newCapacity)
			{
				// Update headpos to end of memory (memory is full with data)
				this->headPos = newCapacity - 1;
				this->numElements = newCapacity;
			}

			// Update capacity
			this->capacity = newCapacity;

			return true;
		}
		else
			// realloc failed! Don't update capacity and return false
			return false;

	}

	//===============================================================================================//
	//=============================== PRIVATE METHOD DECLARATIONS ===================================//
	//===============================================================================================//

	void RingBuff::ShiftElementsSoTailPosIsZero()
	{
		uint32_t currNumElements = this->NumElements();

		// Create a temp buffer memory space for copying
		// existing data into
		uint8_t tempBuffMem[currNumElements];

		// Read all data into temp memory
		this->Read(tempBuffMem, currNumElements);

		// Reset the head and tail positions back to 0, numElements does not change.
		this->headPos = 0;
		this->tailPos = 0;

		// Now write all data back, tailPos should stay at 0
		this->Write(tempBuffMem, currNumElements);

		// Done!
	}


} // namespace RingBuffNs

// EOF
