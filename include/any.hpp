#ifndef ANY_HPP
#define ANY_HPP


#include <type_traits>
#include <utility>


namespace shadow
{
// abstract base class providing interface to held value
class holder_base
{
public:
    virtual holder_base* copy() const = 0;
    virtual void placement_copy(
        std::aligned_storage_t<sizeof(holder_base*)>* buffer) const = 0;
    virtual ~holder_base() = default;
};


// concrete subclass holding value of any type
template <class T>
class holder : public holder_base
{
    friend class any;

public:
    holder(const T& value) : value_(value)
    {
    }

    holder(T&& value) : value_(std::move(value))
    {
    }

    virtual holder_base*
    copy() const override
    {
        return new holder<T>(value_);
    }

    virtual void
    placement_copy(
        std::aligned_storage_t<sizeof(holder_base*)>* buffer) const override
    {
        new(buffer) holder<T>(value_);
    }

private:
    T value_;
};


template <class T>
struct is_small_buffer_type
{
    static const bool value =
        sizeof(std::decay_t<T>) <
        sizeof(std::aligned_storage_t<sizeof(holder_base*)>);
};

template <class T>
constexpr bool is_small_buffer_type_v = is_small_buffer_type<T>::value;

// type erasure wrapper for value of any type
class any
{
public:
    // construct any without contained object, ie. empty
    any();

    // construct any with contained object of type std::decay_t<T>
    // small buffer overload
    template <
        class T,
        class = std::enable_if_t<is_small_buffer_type_v<T> &&
                                 !std::is_same<std::decay_t<T>, any>::value>>
    any(T&& value) : on_heap_(false)
    {
        new(&stack) holder<std::decay_t<T>>(std::forward<T>(value));
    }

    // construct any with contained object of type std::decay_t<T>
    // heap overload
    template <
        class T,
        class = std::enable_if_t<!is_small_buffer_type_v<T> &&
                                 !std::is_same<std::decay_t<T>, any>::value>,
        class = void>
    any(T&& value)
        : on_heap_(true),
          heap(new holder<std::decay_t<T>>(std::forward<T>(value)))
    {
    }

    // copy constructor
    any(const any& other);

    // move constructor
    any(any&& other);

    // copy assignment
    any& operator=(const any& other);

    // move assignment
    any& operator=(any&& other);

    // swap
    void swap(any& other);

    ~any();

public:
    bool has_value() const;
    bool on_heap() const;

    template <class T>
    std::decay_t<T>& get();

    template <class T>
    const std::decay_t<T>& get() const;

private:
    bool on_heap_;
    union {
        holder_base* heap;
        std::aligned_storage_t<sizeof heap> stack;
    };
};

static void swap(any& lhs, any& rhs);
} // namespace shadow


namespace shadow
{
inline any::any() : on_heap_(true), heap(nullptr)
{
}

inline any::any(const any& other) : on_heap_(other.on_heap_)
{
    if(other.on_heap_)
    {
        if(other.heap == nullptr)
        {
            heap = nullptr;
        }
        else
        {
            heap = other.heap->copy();
        }
    }
    else
    {
        reinterpret_cast<const holder_base*>(&other.stack)
            ->placement_copy(&stack);
    }
}

inline any::any(any&& other) : on_heap_(other.on_heap_)
{
    if(other.on_heap_)
    {
        heap = other.heap;
        other.heap = nullptr;
    }
    else
    {
        stack = other.stack;
        other.on_heap_ = true;
        other.heap = nullptr;
    }
}

inline any&
any::operator=(const any& other)
{
    auto temp = other;
    temp.swap(*this);

    return *this;
}

inline any&
any::operator=(any&& other)
{
    shadow::any temp(std::move(other));
    swap(temp);

    return *this;
}

inline void
any::swap(any& other)
{
    using std::swap;

    if(on_heap_ == true && other.on_heap_ == true)
    {
        swap(heap, other.heap);
        return;
    }
    if(on_heap_ == true && other.on_heap_ == false)
    {
        // create heap copy
        auto ptr_copy = heap;
        stack = other.stack;
        other.heap = ptr_copy;

        swap(on_heap_, other.on_heap_);
        return;
    }
    if(on_heap_ == false && other.on_heap_ == true)
    {
        // create heap copy
        auto ptr_copy = other.heap;
        other.stack = stack;
        heap = ptr_copy;

        swap(on_heap_, other.on_heap_);
        return;
    }
    if(on_heap_ == false && other.on_heap_ == false)
    {
        swap(stack, other.stack);
        return;
    }
}

inline any::~any()
{
    if(on_heap_)
    {
        delete heap;
    }
    else
    {
        reinterpret_cast<holder_base*>(&stack)->~holder_base();
    }
}

inline bool
any::has_value() const
{
    if(on_heap_)
    {
        return heap != nullptr;
    }

    return true;
}

inline bool
any::on_heap() const
{
    return on_heap_;
}

template <class T>
inline std::decay_t<T>&
any::get()
{
    if(on_heap_)
    {
        return static_cast<holder<std::decay_t<T>>*>(heap)->value_;
    }

    return reinterpret_cast<holder<std::decay_t<T>>*>(&stack)->value_;
}

template <class T>
inline const std::decay_t<T>&
any::get() const
{
    if(on_heap_)
    {
        return static_cast<const holder<std::decay_t<T>>*>(heap)->value_;
    }

    return reinterpret_cast<const holder<std::decay_t<T>>*>(&stack)->value_;
}

inline void
swap(any& lhs, any& rhs)
{
    lhs.swap(rhs);
}
}


namespace std
{
template <>
inline void
swap(shadow::any& lhs, shadow::any& rhs)
{
    lhs.swap(rhs);
}
}

#endif
