#ifndef LZL_QT_UTILS_SIGNALREGISTER_H
#define LZL_QT_UTILS_SIGNALREGISTER_H

#include "convert_qvariant.h"
#include "function_traits.h"

namespace lzl::qt_utils {

enum SIGNAL_REGISTER_DICT_TYPE : bool {
    MAP = false,
    HASH = true,
}; // Select whether SignalRegister uses QMap or QHash

template <typename KEY_T = QString, SIGNAL_REGISTER_DICT_TYPE DICT_T = SIGNAL_REGISTER_DICT_TYPE::HASH>
class SignalRegister
{
public:
    using KeyType = KEY_T;
    using DictType = typename std::conditional<DICT_T, QMap<KEY_T, QVariant>, QHash<KEY_T, QVariant>>::type;

    // registerSignal template
    template <typename Obj, typename Arg>
    void registerSignal(const KeyType& key, Obj* obj, void (Obj::*func)(Arg))
    {
        using arg_type = Arg;
        Q_ASSERT_X(
            !m_dict.contains(key),
            "lzl::qt_utils::Settings::registerSignal",
            QString("The signal: \"%1\", has already been registered.").arg(key).toStdString().c_str()
        );
        m_dict.insert(key, [obj, func](const QVariant& value) -> void {
            (obj->*func)(ConvertQVariant<arg_type>::convert(value));
        });
    }

#if __cplusplus >= 202002L
    template <lzl::utils::IsLambda F>
    void registerSignal(const QString& key, F&& Func)
#else
    template <typename F>
    auto registerSignal(const QString& key, F&& Func) ->
        typename std::enable_if<lzl::utils::is_lambda<decltype(&F::operator())>::value>::type
#endif
    {
        using args_type = typename lzl::utils::lambda_traits<decltype(&F::operator())>::args_type;
        Q_STATIC_ASSERT_X(
            std::tuple_size<args_type>::value == 1, "Here lambda must take exactly one argument."
        );
        using arg_type = typename std::tuple_element<0, args_type>::type;
        Q_ASSERT_X(
            !m_dict.contains(key),
            "lzl::qt_utils::Settings::registerSignal",
            QString("The signal: \"%1\", has already been registered.").arg(key).toStdString().c_str()
        );
        m_dict.insert(key, [Func](const QVariant& value) -> void {
            Func(ConvertQVariant<arg_type>::convert(value));
        });
    }

    void unRegisterSignal(const QString& key)
    {
        Q_ASSERT_X(
            m_dict.contains(key),
            "lzl::qt_utils::Settings::registerSignal",
            QString("The signal: \"%1\", is not registered.").arg(key).toStdString().c_str()
        );
        m_dict.remove(key);
    }

private:
    DictType m_dict;
};

} // namespace lzl::qt_utils

#endif // LZL_QT_UTILS_SIGNALREGISTER_H
