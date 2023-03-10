#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <iostream>
#include <utility>
#include <string>

using namespace std;

template <typename Type>
class SingleLinkedList
{
    // Узел списк
    struct Node
    {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next)
        {
        }
        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator
    {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node)
        {
            node_ = node;
        }
    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept
        {
            this->node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept
        {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept
        {
            return this->node_ != rhs.node_;
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept
        {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept
        {
            return this->node_ != rhs.node_;
        }

        BasicIterator& operator++() noexcept
        {
            assert(node_ != nullptr);
            this->node_ = node_->next_node;
            return *this;
        }

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        BasicIterator operator++(int) noexcept
        {
            assert(node_ != nullptr);
            auto* old = node_;
            this->node_ = node_->next_node;
            return BasicIterator(old);
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] reference operator*() const noexcept
        {
            assert(node_ != nullptr);
            return (*node_).value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept
        {
            assert(node_ != nullptr);
            return &(*node_).value;
        }

    private:
        Node* node_ = nullptr;
    };
public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList() : size_(0) {}

    ~SingleLinkedList()
    {
        Clear();
    }

    SingleLinkedList(std::initializer_list<Type> values)
    {
        SingleLinkedList temp;
        Iterator current(temp.before_begin());
        for (const auto& element : values)
        {
            current = temp.InsertAfter(current, element);
        }
        swap(temp);
    }

    SingleLinkedList(const SingleLinkedList& other)
    {
        SingleLinkedList temp;
        Iterator current(temp.before_begin());
        for (const auto& element : other)
        {
            current = temp.InsertAfter(current, element);
        }
        swap(temp);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs)
    {
        SingleLinkedList temp(rhs);
        swap(temp);
        return *this;
    }

    

    // Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept
    {
        std::swap(head_.next_node, other.head_.next_node);

        std::swap(size_, other.size_);
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept
    {
        return size_;
    }

    void PushFront(const Type& value)
    {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void PopFront() noexcept
    {
        if (IsEmpty())
        {
            return;
        }
        Node* new_front = head_.next_node->next_node;
        delete head_.next_node;
        head_.next_node = new_front;
        --size_;
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value)
    {
        assert(pos.node_ != nullptr);
        Node* insert_node = new Node(value, nullptr);
        insert_node->next_node = pos.node_->next_node;
        pos.node_->next_node = insert_node;
        ++size_;
        return Iterator(pos.node_->next_node);
    }

    Iterator EraseAfter(ConstIterator pos) noexcept
    {
        assert(pos.node_ != nullptr);
        Node* erase_node = pos.node_->next_node;
        pos.node_->next_node = erase_node->next_node;
        delete erase_node;
        --size_;
        return Iterator(pos.node_->next_node);
    }

    void Clear() noexcept
    {
        while (head_.next_node != nullptr)
        {
            auto temp = head_.next_node;
            head_.next_node = temp->next_node;
            delete temp;
            --size_;
        }
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept
    {
        return (size_ == 0);
    }

    [[nodiscard]] Iterator begin() noexcept
    {
        return Iterator(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept
    {
        return Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator begin() const noexcept
    {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator end() const noexcept
    {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept
    {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator cend() const noexcept
    {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] Iterator before_begin() noexcept
    {
        return Iterator(&head_);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept
    {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept
    {
        return ConstIterator(const_cast<Node*>(&head_));
    }
private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept
{
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return (lhs.GetSize() == rhs.GetSize() && equal(lhs.cbegin(), lhs.cend(), rhs.cbegin()));
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return (lhs < rhs || lhs == rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return (rhs < lhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(lhs < rhs);
}