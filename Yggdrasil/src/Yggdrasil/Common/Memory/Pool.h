// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include <stack>

namespace Ygg::Memory
{
    template<class T, uint64_t elementCount>
    class Pool
    {
    public:
        Pool()
            : m_Data(static_cast<T*>(malloc(sizeof(T)* elementCount)))
        {
            this->m_Stack.push(this->m_Data);
        }

        ~Pool()
        {}

        T& operator[](uint64_t index)
        {
            return this->m_Data[index];
        }

        const T& At(uint64_t index) const
        {
            return this->m_Data[index];
        }

        T* Allocate()
        {
            if (this->m_CurrentSize == elementCount)
            {
                YGGDRASIL_CORE_ERROR("Tried to Allocate element from filled pool.");
                return nullptr;
            }
            T* result{ this->m_Stack.top() };
            this->m_Stack.pop();
            *result = T{};
            T* next{ result };
            next++;
            this->m_Stack.push(next);
            this->m_CurrentSize++;
            return result;
        }

        void Free(T* ptr)
        {
            if (ptr == nullptr)
            {
                YGGDRASIL_CORE_WARN("Tried to Free nullptr from pool.");
                return;
            }
            if (ptr < &this->m_Data[0] || ptr >= &this->m_Data[elementCount])
            {
                YGGDRASIL_CORE_WARN("Tried to Free unrelated memory from pool.");
                return;
            }
            ptr->~T();
            this->m_Stack.push(ptr);
            this->m_CurrentSize--;
        }

    private:
        T* m_Data;
        std::m_Stack<T*> m_Stack{};
        uint64_t m_CurrentSize{ 0 };
    };
}
