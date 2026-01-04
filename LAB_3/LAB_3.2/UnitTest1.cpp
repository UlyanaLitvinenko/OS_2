#include "pch.h"
#include "CppUnitTest.h"
#include "Marker.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace VNPTests
{
    TEST_CLASS(MarkerLogicTests)
    {
    public:

        TEST_METHOD(TestMarkerInitialization)
        {
            MarkerData m;
            m.id = 1;
            Assert::AreEqual(1, m.id);
        }

        TEST_METHOD(TestMarkingSimulation)
        {
            std::vector<int> testArr(10, 0);
            int id = 2;
            int idx = 3;

            if (testArr[idx] == 0) {
                testArr[idx] = id;
            }

            Assert::AreEqual(id, testArr[idx]);
        }

        TEST_METHOD(TestUnmarking)
        {
            std::vector<int> testArr = { 2, 2, 0, 2 };
            std::vector<int> marked = { 0, 1, 3 };

            for (int i : marked) testArr[i] = 0;

            Assert::AreEqual(0, testArr[0]);
            Assert::AreEqual(0, testArr[1]);
            Assert::AreEqual(0, testArr[3]);
        }
    };
}
