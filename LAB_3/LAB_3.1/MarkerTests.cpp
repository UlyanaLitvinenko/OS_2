#include "pch.h"
#include "CppUnitTest.h"
#include "Header.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mytest1
{
    TEST_CLASS(mytest1)
    {
    public:

        TEST_METHOD(TestTryMarkSuccess)
        {
            std::vector<int> arr(5, 0);
            bool result = try_mark(arr, 3, 2); 
            Assert::IsTrue(result);
            Assert::AreEqual(3, arr[2]);
        }

        TEST_METHOD(TestTryMarkFail)
        {
            std::vector<int> arr(5, 0);
            arr[1] = 7;
            bool result = try_mark(arr, 3, 6); 
            Assert::IsFalse(result);
            Assert::AreEqual(7, arr[1]);
        }

        TEST_METHOD(TestClearMarks)
        {
            std::vector<int> arr = { 3, 3, 0, 3, 0 };
            std::vector<int> marked = { 0, 1, 3 };
            clear_marks(arr, marked);
            Assert::AreEqual(0, arr[0]);
            Assert::AreEqual(0, arr[1]);
            Assert::AreEqual(0, arr[3]);
        }
    };
}
