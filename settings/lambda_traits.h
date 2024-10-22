#ifndef LZL_UTILS_LAMBDA_TRAITS_H
#define LZL_UTILS_LAMBDA_TRAITS_H

#include <functional>

namespace lzl::utils {

#if 0
// function_traits template
template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<std::function<R(Args...)>>
{
    using args_type = std::tuple<Args...>;
};
#endif

// lambda_traits template
template <typename F>
// struct LambdaTraits; // The recursion on the following line recognizes the std::function object
struct lambda_traits : public lambda_traits<decltype(&F::operator())>
{
};

template <typename C, typename R, typename... Args>
struct lambda_traits<R (C::*)(Args...) const>
{
    using args_type = std::tuple<Args...>;
};

#if __cplusplus >= 202002L
// C++20 or later
// Define the Lambda concept to check whether it is a Lambda expression
template <typename T>
concept IsLambda = requires(T t) {
    { &T::operator() };
};
#else
// template check whether it is a Lambda expression
template <typename T>
struct is_lambda : std::false_type
{
};

template <typename C, typename R, typename... Args>
struct is_lambda<R (C::*)(Args...) const> : std::true_type
{
};
#endif

} // namespace lzl::utils

#endif // LZL_UTILS_LAMBDA_TRAITS_H
