#pragma once
#include "Yggdrasil/Types.h"
#include "Yggdrasil/core/util/Log.h"
#include <stack>

namespace yggdrasil::memory
{
    template<class T, uint64_t elementCount>
    class Pool
    {
    public:
        Pool()
            : data( static_cast<T*>(malloc(sizeof(T) * elementCount)) )
        {
            stack.push(&data[0]);
        }

        ~Pool()
        {}

        T& operator[](uint64_t index)
        {
            return data[index];
        }

        const T& at(uint64_t index) const
        {
            return data[index];
        }

        T* allocate()
        {
            if (currentSize == elementCount)
            {
                YGGDRASIL_CORE_ERROR("Tried to allocate element from filled pool.");
                return nullptr;
            }
            T* result{ stack.top() };
            memset(result, 0, sizeof(T));
            stack.pop();
            stack.push(result++);
            currentSize++;
            return result;
        }

        void free(T* ptr)
        {
            if (ptr == nullptr)
            {
                YGGDRASIL_CORE_WARN("Tried to free nullptr from pool.");
                return;
            }
            if (ptr < &data[0] || ptr >= &data[elementCount])
            {
                YGGDRASIL_CORE_WARN("Tried to free unrelated memory from pool.");
                return;
            }
            stack.push(ptr);
            currentSize--;
        }

    private:
        T* data;
        std::stack<T*> stack{};
        uint64_t currentSize{ 0 };
    };
}
