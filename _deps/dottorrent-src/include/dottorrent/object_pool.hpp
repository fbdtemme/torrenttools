#pragma once

#include <atomic>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <cassert>

#include "dottorrent/mpmcqueue.hpp"

namespace dottorrent::pool {

template <typename T>
    requires std::is_default_constructible<T>::value
struct object_pool_policy
{
    using value_type = T;
    using value_ptr = std::shared_ptr<value_type>;

    static auto construct() -> value_ptr
    { return std::make_shared<T>(); }

    static void recycle(value_ptr&) {};
};


/// A concurrent object pool.
///
template <typename T, typename Policy=object_pool_policy<T>>
class object_pool
{
public:
    /// The type of the managed object
    using value_type = T;
    using value_ptr = std::shared_ptr<value_type>;
    using size_type = std::ptrdiff_t ;
    using queue_type = rigtorp::mpmc::Queue<value_ptr>;

public:
    /// Default constructor, we only want this to be available
    /// i.e. the shared_object_pool to be default constructible if the
    /// value_type we build is default constructible.
    object_pool()
            : pool_(std::make_shared<impl>(-1))
    { }

    explicit object_pool(size_type capacity)
            : pool_(std::make_shared<impl>(capacity))
    { }

    explicit object_pool(size_type capacity, size_type size)
            : pool_(std::make_shared<impl>(capacity, size))
    { }

    /// Only move construction/assignment is supported.
    /// This function is not thread-safe!
    object_pool(const object_pool& other) = delete;
    object_pool(object_pool&& other) noexcept = default;
    object_pool& operator=(const object_pool& other) = delete;
    object_pool& operator=(object_pool&& other) noexcept = default;

    /// @returns the number of unused resources
    size_type capacity() const noexcept
    {
        assert(pool_);
        return pool_->capacity();
    }

    size_type size() const noexcept
    {
        assert(pool_);
        return pool_->size();
    }

    /// Free all unused resources
    void clear()
    {
        assert(pool_);
        pool_->clear();
    }

    /// @return A resource from the pool.
    auto get() -> value_ptr
    {
        assert(pool_);
        return pool_->get();
    }

private:
    // The actual pool implementation. We use the
    // enable_shared_from_this helper to make sure we can pass a
    // "back-pointer" to the pooled objects. The idea behind this
    // is that we need objects to be able to add themselves back
    // into the pool once they go out of scope.
    struct impl : public std::enable_shared_from_this<impl>
    {
        /// @copydoc shared_object_pool::shared_object_pool()
        explicit impl(size_type capacity)
            : queue_(capacity)
            , resources_left_(capacity)
        {}

        explicit impl(size_type capacity, size_type size)
            : queue_(capacity)
            , resources_left_(capacity - size)
        {
            assert(capacity >= size);

            for (size_type i = 0; i != size; ++i) {
                queue_.emplace(std::invoke(Policy::construct));
            }

//            assert(queue_.size() == initial_resources);
        }

        /// Only move construction/assignment is supported.
        /// This function is not thread-safe!
        impl(const impl& other) = delete;
        impl(impl&& other) noexcept = default;
        impl& operator=(const impl& other) = delete;
        impl& operator=(impl&& other) noexcept = default;

        /// Allocate a new value from the pool
        /// This function is not thread-safe!
        value_ptr get()
        {
            value_ptr resource;

            // try to retrieve an unused resource from the pool
            if (queue_.try_pop(resource)) {}
            else if (resources_left_.load(std::memory_order_relaxed) != 0) {
                resources_left_.fetch_sub(1, std::memory_order_relaxed);
                resource = std::invoke(Policy::construct);
            } else {
                queue_.pop(resource);
            }
            // Here we create a std::shared_ptr<T> with a naked
            // pointer to the resource and a custom deleter
            // object. The custom deleter object stores two
            // things:
            //
            //   1. A std::weak_ptr<T> to the pool (used when we
            //      need to put the resource back in the pool). If
            //      the pool dies before the resource then we can
            //      detect this with the weak_ptr and no try to
            //      access it.
            //
            //   2. A std::shared_ptr<T> that points to the actual
            //      resource and is the one actually keeping it alive.

            return value_ptr(resource.get(), deleter(impl::weak_from_this(), resource));
        }

        /// @copydoc shared_object_pool::free_unused()
        /// not thread safe!
        void clear()
        { queue_.clear(); }

        /// @copydoc shared_object_pool::unused_resources()
        size_type capacity() const noexcept
        { return queue_.capacity(); }

        /// @copydoc shared_object_pool::unused_resources_approx()
        size_type size() const noexcept
        { return queue_.size(); }

        /// This function is called when a resource is added
        /// back into the pool. It will invoke the Policy::recycle function.
        void recycle(value_ptr&& resource)
        {
            std::invoke(Policy::recycle, resource);
            queue_.emplace(std::move(resource));
        }

    private:
        /// Stores all the free resources
        queue_type queue_;
        std::atomic<std::size_t> resources_left_;
    };

    /// The custom deleter object used by the std::shared_ptr<T>
    /// to de-allocate the object if the pool goes out of
    /// scope. When a std::shared_ptr wants to de-allocate the
    /// object contained it will call the operator() define here.
    struct deleter
    {
        /// @param pool. A weak_ptr to the pool
        deleter(std::weak_ptr<impl> pool, value_ptr resource)
            : pool_(std::move(pool))
            , resource_(std::move(resource))
        {
            assert(!pool_.expired());
            assert(resource_);
        }

        /// Call operator called by std::shared_ptr<T> when
        /// de-allocating the object.
        void operator()(value_type*)
        {
            // Place the resource in the free list
            auto pool = pool_.lock();

            if (pool) {
                pool->recycle(std::move(resource_));
            }

            // This reset() is needed because otherwise a circular
            // dependency can arise here in special situations.
            //
            // One example of such a situation is when the value_type
            // derives from std::enable_shared_from_this in that case,
            // the following will happen:
            //
            // The std::enable_shared_from_this implementation works by
            // storing a std::weak_ptr to itself. This std::weak_ptr
            // internally points to an "counted" object keeping track
            // of the reference count managing the raw pointer's release
            // policy (e.g. storing the custom deleter etc.) for all
            // the shared_ptr's. The "counted" object is both kept
            // alive by all std::shared_ptr and std::weak_ptr objects.
            //
            // In this specific case of std::enable_shared_from_this,
            // the custom deleter is not destroyed because the internal
            // std::weak_ptr still points to the "counted" object and
            // inside the custom deleter we are keeping the managed
            // object alive because we have a std::shared_ptr to it.
            //
            // The following diagram show the circular dependency where
            // the arrows indicate what is keeping what alive:
            //
            //  +----------------+                   +--------------+
            //  | custom deleter +--------------+    | real deleter |
            //  +----------------+              |    +--------------+
            //         ^                        |            ^
            //         |                        |            |
            //         |                        |            |
            //   +-----+--------+               |    +-------+------+
            //   | shared_count |               |    | shared_count |
            //   +--------------+               |    +--------------+
            //      ^    ^                      |            ^
            //      |    |                      |            |
            //      |    |                      |            |
            //      |    |                      v            |
            //      |    |  +------------+    +------------+ |
            //      |    +--+ shared_ptr |    | shared_ptr +-+
            //      |       +------------+    +----+-------+
            //      |                              |
            //      |                              |
            // +----+-----+            +--------+  |
            // | weak_ptr |<-----------+ object |<-+
            // +----------+            +--------+
            //
            // The std::shared_ptr on the right is the one managed by the
            // shared pool, it is the one actually deleting the
            // object when it goes out of scope. The shared_ptr on the
            // left is the one which contains the custom
            // deleter that will return the object into the resource
            // pool when it goes out of scope.
            //
            // By calling reset on the shared_ptr in the custom deleter
            // we break the cyclic dependency.
            resource_.reset();
        }

        // Pointer to the pool needed for recycling
        std::weak_ptr<impl> pool_;
        // The resource object
        value_ptr resource_;
    };

private:
    // The pool impl
    std::shared_ptr<impl> pool_;
};

} // namespace dottorrent::pool