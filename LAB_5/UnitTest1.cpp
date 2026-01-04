#include "pch.h"
#include "CppUnitTest.h"
#include <vector>
#include <string>
#include <fstream>

#include "server.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
    TEST_CLASS(UnitTest1)
    {
    public:

        TEST_METHOD(TestNaytiIndex_Nayden)
        {
            dannye.clear();
            rabotnik r1{ 1, "Ivan", 40.0 };
            rabotnik r2{ 2, "Petr", 35.5 };
            dannye.push_back(r1);
            dannye.push_back(r2);

            int idx = naytiIndex(2);
            Assert::AreEqual(1, idx);
        }

        TEST_METHOD(TestNaytiIndex_NeNayden)
        {
            dannye.clear();
            rabotnik r1{ 1, "Ivan", 40.0 };
            dannye.push_back(r1);

            int idx = naytiIndex(99);
            Assert::AreEqual(-1, idx);
        }

        TEST_METHOD(TestObrabotat_Nayden)
        {
            dannye.clear();
            fail = L"testdata2.bin";
            rabotnik r1{ 1, "Ivan", 40.0 };
            dannye.push_back(r1);

            zapros req{};
            req.cmd = CHITAT_NACHALO;
            req.id = 1;
            otvet rep{};

            obrabotat(req, rep);
            Assert::AreEqual((int)OK, (int)rep.res);
            Assert::AreEqual(r1.id, rep.rec.id);
        }

        TEST_METHOD(TestObrabotat_NeNayden)
        {
            dannye.clear();
            fail = L"testdata3.bin";

            zapros req{};
            req.cmd = CHITAT_NACHALO;
            req.id = 999;
            otvet rep{};

            obrabotat(req, rep);
            Assert::AreEqual((int)NE_NAYDENO, (int)rep.res);
        }
    };
}
