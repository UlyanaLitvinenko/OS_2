#include "pch.h"
#include "CppUnitTest.h"

#include "BoostMessageManager.h"
#include "BoostMessageSender.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace boost::interprocess;

namespace UnitTest1
{
    TEST_CLASS(MessageSystemTests)
    {
    public:

        TEST_METHOD(InitializationTest)
        {
            shared_memory_object::remove("MessageSystemMemory");

            BoostMessageManager manager(5);
            Assert::IsTrue(manager.is_initialized());

            BoostMessageSender sender;
            Assert::IsTrue(sender.is_initialized());
        }

        TEST_METHOD(SendReceiveMessageTest)
        {
            shared_memory_object::remove("MessageSystemMemory");

            BoostMessageManager manager(3);
            Assert::IsTrue(manager.is_initialized());

            BoostMessageSender sender;
            Assert::IsTrue(sender.is_initialized());

            {
                std::string testMessage = "Hello from test!";
                managed_shared_memory segment(open_only, "MessageSystemMemory");

                typedef allocator<char, managed_shared_memory::segment_manager> CharAllocator;
                typedef basic_string<char, std::char_traits<char>, CharAllocator> shared_string;
                typedef allocator<shared_string, managed_shared_memory::segment_manager> StringAllocator;
                typedef circular_buffer<shared_string, StringAllocator> shared_circular_buffer;

                auto message_buffer = segment.find<shared_circular_buffer>("MessageBuffer").first;
                auto buffer_mutex   = segment.find<named_mutex>("BufferMutex").first;
                auto empty_slots    = segment.find<named_semaphore>("EmptySlotsSemaphore").first;
                auto full_slots     = segment.find<named_semaphore>("FullSlotsSemaphore").first;

                if (empty_slots->try_wait())
                {
                    scoped_lock<named_mutex> lock(*buffer_mutex);
                    CharAllocator char_alloc(segment.get_segment_manager());
                    shared_string shared_msg(testMessage.c_str(), char_alloc);
                    message_buffer->push_back(shared_msg);
                    full_slots->post();
                }
            }

            {
                managed_shared_memory segment(open_only, "MessageSystemMemory");
                auto message_buffer = segment.find<BoostMessageManager::shared_circular_buffer>("MessageBuffer").first;
                auto msg = message_buffer->front();
                Assert::AreEqual("Hello from test!", msg.c_str());
            }
        }
    };
}
