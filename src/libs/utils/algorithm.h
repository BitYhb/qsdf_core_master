#ifndef UTILS_ALGORITHM_H
#define UTILS_ALGORITHM_H

#include <algorithm>
#include <functional>
#include <iterator>

namespace Utils {

template<typename R, typename S, typename T>
decltype(auto) equal(R (S::*function)() const, T value)
{
    return std::bind<bool>(std::equal_to<T>(), value, std::bind(function, std::placeholders::_1));
}

template<typename R, typename S, typename T>
decltype(auto) equal(R S::*member, T value)
{
    return std::bind<bool>(std::equal_to<T>(), value, std::bind(member, std::placeholders::_1));
}

template<typename C>
inline std::back_insert_iterator<C> inserter(C &container)
{
    return std::back_inserter(container);
}

template<template<typename...> class C, typename... CArgs, typename = decltype(&C<CArgs...>::reserve)>
void reserve(C<CArgs...> &c, typename C<CArgs...>::size_type s)
{
    c.reserve(s);
}

template<typename ResultContainer, // complete result container type
         typename SC,              // input container type
         typename F>               // function type
[[nodiscard]] decltype(auto) transform(SC &&container, F function)
{
    ResultContainer result;
    reserve(result, typename ResultContainer::size_type(container.size()));
    std::transform(std::begin(container), std::end(container), inserter(result), function);
    return result;
}

template<template<typename> class C, // result container type
         typename SC,                // input container type
         typename F,                 // function type
         typename Value = typename std::decay_t<SC>::value_type,
         typename Result = std::decay_t<std::invoke_result_t<F, Value &>>,
         typename ResultContainer = C<Result>>
[[nodiscard]] decltype(auto) transform(SC &&container, F function)
{
    return transform<ResultContainer>(std::forward<SC>(container), function);
}

template<template<typename...> class C, // result container type
         typename SC,                   // input container type
         typename R,
         typename S>
[[nodiscard]] decltype(auto) transform(SC &&container, R (S::*p)() const)
{
    return transform<C>(std::forward<SC>(container), std::mem_fn(p));
}

template<typename ResultContainer, // complete result container type
         typename SC,              // input container type
         typename R,
         typename S>
[[nodiscard]] decltype(auto) transform(SC &&container, R (S::*p)() const)
{
    return transform<ResultContainer>(std::forward<SC>(container), std::mem_fn(p));
}

template<typename ResultContainer, // complete result container type
         typename SC,              // input container
         typename R,
         typename S>
[[nodiscard]] decltype(auto) transform(SC &&container, R S::*p)
{
    return transform<ResultContainer>(std::forward<SC>(container), std::mem_fn(p));
}

template<template<typename...> class C, // result container
         typename SC,                   // input container
         typename R,
         typename S>
[[nodiscard]] decltype(auto) transform(SC &&container, R S::*p)
{
    return transform<C>(std::forward<SC>(container), std::mem_fn(p));
}

template<template<typename...> class C, // container type
         typename F,                    // function type
         typename... CArgs>             // Arguments to SC
[[nodiscard]] decltype(auto) transform(const C<CArgs...> &container, F function)
{
    return transform<C, const C<CArgs...> &>(container, function);
}

template<template<typename...> class C, // container type
         typename F,                    // function type
         typename... CArgs>             // Arguments to SC
[[nodiscard]] decltype(auto) transform(C<CArgs...> &container, F function)
{
    return transform<C, C<CArgs...> &>(container, function);
}

template<typename C, typename F>
[[nodiscard]] C filtered(const C &container, F predicate)
{
    C out;
    std::copy_if(std::begin(container), std::end(container), inserter(out), predicate);
    return out;
}

template<typename C, typename R, typename S>
[[nodiscard]] C filtered(const C &container, R (S::*predicate)() const)
{
    C out;
    std::copy_if(std::begin(container), std::end(container), inserter(out), std::mem_fn(predicate));
    return out;
}

template<typename C, typename F>
[[nodiscard]] typename C::value_type findOr(const C &container, typename C::value_type other, F function)
{
    typename C::const_iterator begin = std::begin(container);
    typename C::const_iterator end = std::end(container);

    typename C::const_iterator it = std::find_if(begin, end, function);
    return it == end ? other : *it;
}

template<typename C, typename F>
[[nodiscard]] typename std::enable_if_t<std::is_copy_assignable<typename C::value_type>::value, typename C::value_type>
findOrDefault(const C &container, F function)
{
    return findOr(container, typename C::value_type(), function);
}

template<typename C, typename F>
bool anyOf(const C &container, F predicate)
{
    return std::any_of(std::begin(container), std::end(container), predicate);
}

template<typename C, typename F>
bool contains(const C &container, F function)
{
    return anyOf(container, function);
}

template<typename Container>
inline void sort(Container &container)
{
    std::stable_sort(std::begin(container), std::end(container));
}

template<typename C, typename F>
inline void sort(C &container, F function)
{
    std::stable_sort(std::begin(container), std::end(container), function);
}

template<typename Container, typename R, typename S>
inline void sort(Container &container, R S::*member)
{
    auto f = std::mem_fn(member);
    using const_ref = typename Container::const_reference;
    std::stable_sort(std::begin(container), std::end(container), [&f](const_ref a, const_ref b) { return f(a) < f(b); });
}

template<typename C, typename R, typename S>
inline void sort(C &container, R (S::*function)() const)
{
    auto f = std::mem_fn(function);
    using const_ref = typename C::const_reference;
    std::stable_sort(std::begin(container), std::end(container), [&f](const_ref a, const_ref b) { return f(a) < f(b); });
}

template<typename Container, typename Op>
inline void reverseForeach(const Container &container, const Op &operation)
{
    auto rend = container.rend();
    for (auto it = container.rbegin(); it != rend; ++it) {
        operation(*it);
    }
}

template<typename Container, typename F>
[[nodiscard]] int indexOf(const Container &container, F function)
{
    typename Container::const_iterator begin = std::begin(container);
    typename Container::const_iterator end = std::end(container);

    typename Container::const_iterator it = std::find_if(begin, end, function);
    return it == end ? -1 : std::distance(begin, it);
}

} // namespace Utils

#endif // UTILS_ALGORITHM_H
