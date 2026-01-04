#include "pch.h"
#include "CppUnitTest.h"
#include "Header.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mytest1
{
    TEST_CLASS(mytest1)
    {
    public:

        TEST_METHOD(TestSum)
        {
            int arr[5] = { 1, 2, 3, 4, 5 };
            Assert::AreEqual(15, sum_array(arr, 5));
        }

        TEST_METHOD(TestAverage)
        {
            int arr[4] = { 2, 4, 6, 8 };
            Assert::AreEqual(5.0, average_array(arr, 4));
        }

        TEST_METHOD(TestReverse)
        {
            int arr[5] = { 1, 2, 3, 4, 5 };
            reverse_array(arr, 5);
            int expected[5] = { 5, 4, 3, 2, 1 };
            for (int i = 0; i < 5; i++) {
                Assert::AreEqual(expected[i], arr[i]);
            }
        }

        TEST_METHOD(TestReplaceMinMax)
        {
            int arr[5] = { 2, 7, 3, 9, 4 };
            double avg = 5.0;
            replace_min_max_with_avg(arr, 5, 2, 9, avg);
            Assert::AreEqual(5, arr[0]);
            Assert::AreEqual(5, arr[3]);
        }
    };
}