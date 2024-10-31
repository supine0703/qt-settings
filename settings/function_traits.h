/**
 * License: MIT
 * Copyright (c) 2024 Li Zonglin (李宗霖) github: <https://github.com/supine0703>
 * Repositories: lzl-cpp-lib <https://github.com/supine0703/lzl-cpp-lib>
 */

#ifndef LZL_FUNCTIONAL_FUNCTION_TRAITS_H
#define LZL_FUNCTIONAL_FUNCTION_TRAITS_H

#include <functional>

namespace lzl::utils {

// prototype
template <typename T>
struct function_traits;

// Ordinary function
template <typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
    static constexpr size_t arity = sizeof...(Args);

    using return_type = Ret;
    using args_tuple = std::tuple<Args...>;

    template <size_t I, typename = std::enable_if_t<(I < arity)>> // C++17
    struct arg
    {
        using type = typename std::tuple_element<I, args_tuple>::type;
    };

    using pointer = Ret (*)(Args...);
    using function_type = Ret(Args...);
    using stl_function_type = std::function<Ret(Args...)>;
};

// function pointer
template <typename Ret, typename... Args>
struct function_traits<Ret (*)(Args...)> : function_traits<Ret(Args...)>
{
};

// std::function
template <typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)>> : function_traits<Ret(Args...)>
{
};

// member function
template <typename Ret, typename Class, typename... Args>
struct function_traits<Ret (Class::*)(Args...)> : function_traits<Ret(Args...)>
{
};

template <typename Ret, typename Class, typename... Args>
struct function_traits<Ret (Class::*)(Args...) const> : function_traits<Ret(Args...)>
{
};

template <typename Ret, typename Class, typename... Args>
struct function_traits<Ret (Class::*)(Args...) volatile> : function_traits<Ret(Args...)>
{
};

template <typename Ret, typename Class, typename... Args>
struct function_traits<Ret (Class::*)(Args...) const volatile> : function_traits<Ret(Args...)>
{
};

// lambda or functor
template <typename Callable>
struct function_traits : function_traits<decltype(&Callable::operator())>
{
};

} // namespace lzl::utils

#endif //LZL_FUNCTIONAL_FUNCTION_TRAITS_H
