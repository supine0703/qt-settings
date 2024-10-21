#ifndef TYPE_HANDLING_H
#define TYPE_HANDLING_H

#include <QVariant>

// 将QVariant类型转换

template <typename T>
struct ConvertQVariant;

// 针对不同类型特化
template <>
struct ConvertQVariant<int>
{
    static int convert(const QVariant& value) { return value.toInt(); }
};

template <>
struct ConvertQVariant<bool>
{
    static bool convert(const QVariant& value) { return value.toBool(); }
};

template <>
struct ConvertQVariant<double>
{
    static double convert(const QVariant& value) { return value.toDouble(); }
};

template <>
struct ConvertQVariant<QString>
{
    static QString convert(const QVariant& value) { return value.toString(); }
};

// 对于参数列表类型的处理

// 提取参数列表的参数
// function_traits模板
template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<std::function<R(Args...)>>
{
    using args_type = std::tuple<Args...>;
};

// lambda_traits模板
template <typename F>
struct lambda_traits : public lambda_traits<decltype(&F::operator())>
{
};

template <typename C, typename R, typename... Args>
struct lambda_traits<R (C::*)(Args...) const>
{
    using args_type = std::tuple<Args...>;
};

// 检测是否是Lambda表达式
template <typename T>
struct is_lambda : std::false_type
{
};

template <typename C, typename R, typename... Args>
struct is_lambda<R (C::*)(Args...) const> : std::true_type
{
};

#endif // TYPE_HANDLING_H
