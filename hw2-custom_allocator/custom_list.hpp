#pragma once

#include <memory>
#include <utility>
#include <iterator>

#include "common/debug.hpp"



template <typename T, typename Alloc = std::allocator<T>>
class custom_list
{
public:
	struct node_s
	{
		explicit node_s(T const& val)
			: m_val(val)
		{ }

		T          m_val;
		node_s*    m_next = nullptr;
	};

	class iterator_c
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type        = T;
		using difference_type   = std::ptrdiff_t;
		using pointer           = T*;
		using reference         = T&;

		iterator_c(node_s* node) noexcept : m_node(node) { }
		iterator_c(iterator_c const&)            noexcept = default;
		iterator_c& operator=(iterator_c const&) noexcept = default;

		iterator_c& operator++() noexcept
		{
			if (m_node != nullptr) { m_node = m_node->m_next; }
			return *this;
		}

		reference operator*() noexcept { return m_node->m_val; }
		pointer operator->() noexcept  { return &m_node->m_val; }
		bool operator==(iterator_c const& o) const noexcept { return m_node == o.m_node; }
		bool operator!=(iterator_c const& o) const noexcept { return !(*this == o); }

	private:
		node_s*    m_node = nullptr;
	};


	using alloc_type      = Alloc;
	using node_alloc_type = typename alloc_type::template rebind<node_s>::other;
	using value_type      = T;
	using iter_type       = iterator_c;


	explicit custom_list(alloc_type const& alloc)
		: m_alloc(alloc)
	{ }
	custom_list() = default;
	~custom_list()                 { clear(); }

	bool empty() const noexcept    { return m_head == nullptr; }
	operator bool() const noexcept { return not empty(); }

	size_t size() const noexcept   { return list_size(); }

	value_type& push_back(value_type const& v)
	{
		using node_alloc_t = std::allocator_traits<node_alloc_type>;
		node_s* p = node_alloc_t::allocate(m_alloc, 1);
		node_alloc_t::construct(m_alloc, p, v);
		list_add(p);
		return p->m_val;
	}

	void clear()
	{
		using node_alloc_t = std::allocator_traits<node_alloc_type>;
		if (empty()) { return; }
		node_s* node = m_head;
		node_s* next = nullptr;
		do {
			next = node->m_next;
			node_alloc_t::destroy(m_alloc, node);
			node_alloc_t::deallocate(m_alloc, node, 1);
			node = next;
		} while (next);
		list_clear();
	}

	iter_type begin() noexcept { return iter_type{m_head}; }
	iter_type end()   noexcept { return iter_type{nullptr}; }

private:
	void list_add(node_s* node) noexcept
	{
		if (node_s* last = list_last()) { last->m_next = node; }
		else                            { m_head = node; }
	}

	node_s* list_last() noexcept
	{
		if (not m_head) { return nullptr; }
		node_s* node = m_head;
		while (node->m_next) { node = node->m_next; }
		return node;
	}

	size_t list_size() const noexcept
	{
		size_t size = 0;
		node_s* node  = m_head;
		while (node) { ++size; node = node->m_next; }
		return size;
	}

	void list_clear() noexcept
	{
		m_head = nullptr;
	}

private:
	node_alloc_type    m_alloc;
	node_s*            m_head = nullptr;
};

