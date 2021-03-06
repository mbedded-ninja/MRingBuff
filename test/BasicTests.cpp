//!
//! @file 			BasicTests.cpp
//! @author 		Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
//! @created		2014-07-24
//! @last-modified 	2014-09-25
//! @brief 			Contains basic tests.
//! @details
//!					See README.rst in root dir for more info.

//===== SYSTEM LIBRARIES =====//
#include <stdio.h>

//===== USER LIBRARIES =====//
#include "MUnitTest/api/MUnitTestApi.hpp"

//===== USER SOURCE =====//
#include "../api/MRingBuffApi.hpp"

namespace RingBuffTestsNs
{
	MTEST_GROUP(BasicTests)
	{

		MTEST(ConstructorWorksTest)
		{

			RingBuffNs::RingBuff ringBuff(100);
			
			// If we got here, test passed!
			CHECK(true);
		}

		MTEST(BasicCapacityTest)
		{
			RingBuffNs::RingBuff ringBuff(50);

			// Check capacity is calculated correctly
			CHECK_EQUAL(ringBuff.Capacity(), 50);
		}

		MTEST(BasicReadWriteTest)
		{

			RingBuffNs::RingBuff ringBuff(100);

			char * someChars = "testing";
			ringBuff.Write(someChars, RingBuffNs::RingBuff::ReadWriteLogic::ANY);

			char readBuff[8];
			ringBuff.Read((uint8_t*)readBuff, 8);

			// Check read data is equal to written data
			CHECK_EQUAL(readBuff, "testing");
		}

		MTEST(BasicReadWhenEmptyTest)
		{
			RingBuffNs::RingBuff ringBuff(100);

			uint8_t readBuff[10] = {0};
			// Read while empty
			ringBuff.Read(readBuff, 8);

			// Check buffer is still empty
			CHECK_EQUAL(readBuff[0], 0);
		}
		
		MTEST(ClearTest)
		{
			RingBuffNs::RingBuff ringBuff(100);

			char someChars[] = "testing";
			ringBuff.Write(someChars);

			// Clear the buffer
			ringBuff.Clear();

			char readBuff[8] = {0};
			ringBuff.Read((uint8_t*)readBuff, 8);

			// Check read data is empty
			CHECK_EQUAL(readBuff, "");
		}

		MTEST(MultipleWritesThenReadTest)
		{
			RingBuffNs::RingBuff ringBuff(100);

			ringBuff.Write((uint8_t*)"12", 2);
			ringBuff.Write((uint8_t*)"34", 2);
			ringBuff.Write((uint8_t*)"56", 2);
			ringBuff.Write((uint8_t*)"78", 2);

			char readBuff[9];
			ringBuff.Read((uint8_t*)readBuff, 8);
			readBuff[9] = '\0';

			// Check read data is empty
			CHECK_EQUAL(readBuff, "12345678");
		}

	} // GROUP(BasicTests)
} // namespace RingBuffTestsNs
