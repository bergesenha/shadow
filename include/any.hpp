#ifndef ANY_HPP
#define ANY_HPP


#include <type_traits>
#include <utility>
#include <memory>


namespace shadow
{
class ptr_holder_base;


// abstract base class providing interface to held value
class holder_base
{
public:
    virtual holder_base* copy() const = 0;
    virtual void placement_copy(
        std::aligned_storage_t<sizeof(holder_base*)>* buffer) const = 0;
    virtual ~holder_base() = default;
    virtual ptr_holder_base* make_value_ptr() = 0;
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

    T
    get_value() const
    {
        return value_;
    }

    virtual ptr_holder_base* make_value_ptr() override;

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
    friend class any_reference;

public:
    template <class T>
    struct get_specializer
    {
        typedef std::decay_t<T>& return_type;

        static return_type
        get(any& a)
        {
            if(a.on_heap_)
            {
                return static_cast<holder<std::decay_t<T>>*>(a.heap)->value_;
            }

            return reinterpret_cast<holder<std::decay_t<T>>*>(&a.stack)->value_;
        }
    };

    template <class T>
    struct get_specializer<T&&>
    {
        typedef std::decay_t<T>&& return_type;

        static return_type
        get(any& a)
        {
            if(a.on_heap_)
            {
                return std::move(
                    static_cast<holder<std::decay_t<T>>*>(a.heap)->value_);
            }

            return std::move(
                reinterpret_cast<holder<std::decay_t<T>>*>(&a.stack)->value_);
        }
    };

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
    typename get_specializer<T>::return_type get();

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
inline typename any::get_specializer<T>::return_type
any::get()
{
    return std::forward<typename get_specializer<T>::return_type>(
        get_specializer<T>::get(*this));
}

template <class T>
inline const std::decay_t<T>&
any::get() const
{
    // meaningless to specialize for T&& since 'this' is const
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


namespace shadow
{

class ptr_holder_base
{
public:
    virtual ptr_holder_base* copy() const = 0;

    virtual holder_base* make_heap_value() const = 0;
    virtual void make_stack_value(
        std::aligned_storage_t<sizeof(holder_base*)>* buffer) const = 0;
    virtual bool fits_on_stack() const = 0;
    virtual void assign_from_holder(const holder_base*) = 0;
};

template <class T>
class ptr_holder : public ptr_holder_base
{
    friend class any_reference;

public:
    ptr_holder() : ptr_value_(nullptr)
    {
    }

    ptr_holder(T& value) : ptr_value_(&value)
    {
    }

    virtual ptr_holder_base*
    copy() const override
    {
        auto ptrh = new ptr_holder<T>;
        ptrh->ptr_value_ = ptr_value_;
        return ptrh;
    }

    virtual holder_base*
    make_heap_value() const override
    {
        return new holder<T>(*ptr_value_);
    }

    virtual void
    make_stack_value(
        std::aligned_storage_t<sizeof(holder_base*)>* buffer) const override
    {
        new(buffer) holder<T>(*ptr_value_);
    }

    virtual bool
    fits_on_stack() const override
    {
        return is_small_buffer_type<T>::value;
    }

    virtual void
    assign_from_holder(const holder_base* h) override
    {
        *ptr_value_ = static_cast<const holder<T>*>(h)->get_value();
    }

private:
    T* ptr_value_;
};

template <class T>
inline ptr_holder_base*
holder<T>::make_value_ptr()
{
    return new ptr_holder<T>(value_);
}


class any_reference
{
public:
    any_reference() = default;

    template <class T>
    any_reference(T& value) : reference_(new ptr_holder<std::decay_t<T>>(value))
    {
    }

    any_reference(any_reference& other) : reference_()
    {
        if(other.has_reference())
        {
            reference_.reset(other.reference_->copy());
        }
    }

    any_reference(const any_reference& other)
        : reference_(other.reference_->copy())
    {
    }

    any_reference(any_reference&& other) = default;

    any_reference(any& value) : reference_()
    {
        if(value.has_value())
        {
            if(value.on_heap())
            {
                reference_.reset(value.heap->make_value_ptr());
            }
            else
            {
                reference_.reset(reinterpret_cast<holder_base*>(&value.stack)
                                     ->make_value_ptr());
            }
        }
    }

    any_reference&
    operator=(any_reference other)
    {
        std::swap(reference_, other.reference_);
        return *this;
    }

    operator any()
    {
        any out;

        if(has_reference())
        {
            if(reference_->fits_on_stack())
            {
                reference_->make_stack_value(&out.stack);
                out.on_heap_ = false;
            }
            else
            {
                out.heap = reference_->make_heap_value();
                out.on_heap_ = true;
            }
        }

        return out;
    }

    any_reference&
    operator=(const any& other)
    {
        if(has_reference())
        {
            if(other.on_heap())
            {
                reference_->assign_from_holder(other.heap);
            }
            else
            {
                reference_->assign_from_holder(
                    reinterpret_cast<const holder_base*>(&other.stack));
            }
        }
        return *this;
    }

public:
    template <class T>
    std::decay_t<T>&
    get()
    {
        return *(static_cast<ptr_holder<T>*>(reference_.get())->ptr_value_);
    }

    template <class T>
    const std::decay_t<T>&
    get() const
    {
        return *(static_cast<ptr_holder<T>*>(reference_.get())->ptr_value_);
    }

    bool
    has_reference() const
    {
        return bool(reference_);
    }

private:
    std::unique_ptr<ptr_holder_base> reference_;
};
}


namespace shadow
{
}
#endif
