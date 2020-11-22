#pragma once

#include <memory>       // std::unique_ptr
#include <stdexcept>    // std::runtime_error

#include "common/debug.hpp"



template <typename T>
class custom_allocator
{
    using buffer_t = std::unique_ptr<uint8_t[]>;

public:
    using value_type = T;

    template<typename U> struct rebind { using other = custom_allocator<U>; };

    explicit custom_allocator(size_t capacity) : m_capacity(capacity) {}
    custom_allocator() noexcept                              = default;
    ~custom_allocator()                                      = default;
    custom_allocator(custom_allocator&&) noexcept            = default;

    custom_allocator(const custom_allocator& o) noexcept
        : m_capacity(o.m_capacity)
    { }

    custom_allocator& operator=(custom_allocator&&) noexcept = default;
    custom_allocator& operator=(const custom_allocator&)     = delete;
    /*
    {
        if (this != &o)
        {
            m_buffer.reset();
            m_size = o.m_size;
            m_capacity = o.m_capacity;
        }
        return *this;
    }
    */

    template<typename U>
    custom_allocator(const custom_allocator<U>& o)
        : m_capacity(o.capacity())
    { }


    T* allocate(size_t n)
    {
        LM("[%p] \033[32m%s\033[0m    n = %zu; m_size = %zu", (void*)this, __PRETTY_FUNCTION__, n, m_size);
        if (n > max_size()) { throw std::bad_alloc(); }
        allocate_buffer_if_needed();
        T* retval = get_first_element() + std::exchange(m_size, m_size + n);
        for (size_t i = 0; i < n; ++i)
        {
            LM("    %p", (void*)(retval + i));
        }
        return retval;
    }

    template<typename U, typename ...Args>
    void construct(U* p, Args &&...args)
	{
        LM("[%p] \033[92m%s\033[0m\n    p = %p", (void*)this, __PRETTY_FUNCTION__, (void*)p);
        new(p) U(std::forward<Args>(args)...);
    }

    //TODO: deallocate when (m_size == 0)
    void deallocate(T*, [[maybe_unused]] size_t n) noexcept
	{
        LM("[%p] \033[31m%s\033[0m    n = %zu; m_size = %zu", (void*)this, __PRETTY_FUNCTION__, n, m_size);
    }

    void destroy(T *p) noexcept
	{
        LM("[%p] \033[91m%s\033[0m\n    p = %p", (void*)this, __PRETTY_FUNCTION__, (void*)p);
        p->~T();
    }

    size_t max_size() const noexcept
	{
		size_t max_size = m_capacity - m_size;
        LM("[%p] %s    max_size = %zu", (void*)this, __PRETTY_FUNCTION__, max_size);
        return max_size;
    }
    size_t size()     const noexcept     { return m_size; }
    size_t capacity() const noexcept     { return m_capacity; }
    void capacity(size_t v)
    {
        if (0 != m_size)
        {
            throw std::runtime_error("Allocator already contents some elements");
        }
        if (m_buffer) { m_buffer.reset(); }
        m_capacity = v;
    }

private:
    T* get_first_element() noexcept      { return reinterpret_cast<T*>(m_buffer.get()); }

	void allocate_buffer_if_needed()
	{
        if (m_buffer) { return; }
        if (0 == m_capacity) { throw std::runtime_error("Unexpected empty capacity"); }
        m_buffer.reset(new uint8_t[m_capacity * sizeof(value_type)]);

		[[maybe_unused]] T const* p = (T*)m_buffer.get();
        LM("[%p] \033[36m%s\033[0m", (void*)this, __PRETTY_FUNCTION__);
        for (size_t i = 0; i < m_capacity; ++i)
        {
            LM("\t\033[36m%p\033[0m", (void*)(p + i));
        }
	}

    buffer_t    m_buffer;
    size_t      m_size     = 0;
    size_t      m_capacity = 0;
};


template <class T, class U>
bool operator==(const custom_allocator<T>&, const custom_allocator<U>&) { return false; }

template <class T, class U>
bool operator!=(const custom_allocator<T>&, const custom_allocator<U>&) { return false; }

