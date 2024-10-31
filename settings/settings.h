#ifndef LZL_QT_UTILS_SETTINGS_H
#define LZL_QT_UTILS_SETTINGS_H

#include "convert_qvariant.h"
#include "function_traits.h"

#include <QSettings>
#include <cstddef>
#include <functional>
#include <qanystringview.h>
#include <qlist.h>
#include <qobjectdefs.h>
#include <qtypes.h>

namespace lzl::qt_utils {

// A Singleton `Settings` Class
class Settings : public QObject
{
    Q_OBJECT

    Settings(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings& operator=(Settings&&) = delete;

public:
    // registerSignal template
    template <typename Obj, typename Arg>
    static void registerSignal(const QString& key, Obj* obj, void (Obj::*func)(Arg))
    {
        using arg_type = Arg;
        Q_ASSERT_X(
            !instance().m_map.contains(key),
            "lzl::qt_utils::Settings::registerSignal",
            QString("The signal: \"%1\", has already been registered.")
                .arg(key)
                .toStdString()
                .c_str()
        );
        instance().m_map.insert(key, [obj, func](const QVariant& value) -> void {
            (obj->*func)(ConvertQVariant<arg_type>::convert(value));
        });
    }

// #if __cpp_concepts
//     template <lzl::utils::IsLambda F>
//     static void registerSignal(const QString& key, F&& Func)
// #else
//     template <typename F>
//     static auto registerSignal(const QString& key, F&& Func) ->
//         typename std::enable_if<lzl::utils::is_lambda<decltype(&F::operator())>::value>::type
// #endif
//     {
//         using args_type = typename lzl::utils::lambda_traits<decltype(&F::operator())>::args_type;
//         Q_STATIC_ASSERT_X(
//             std::tuple_size<args_type>::value == 1, "Here lambda must take exactly one argument."
//         );
//         using arg_type = typename std::tuple_element<0, args_type>::type;
//         Q_ASSERT_X(
//             !instance().m_map.contains(key),
//             "lzl::qt_utils::Settings::registerSignal",
//             QString("The signal: \"%1\", has already been registered.")
//                 .arg(key)
//                 .toStdString()
//                 .c_str()
//         );
//         instance().m_map.insert(key, [Func](const QVariant& value) -> void {
//             Func(ConvertQVariant<arg_type>::convert(value));
//         });
//     }

    static void unRegisterSignal(const QString& key)
    {
        Q_ASSERT_X(
            instance().m_map.contains(key),
            "lzl::qt_utils::Settings::registerSignal",
            QString("The signal: \"%1\", is not registered.").arg(key).toStdString().c_str()
        );
        instance().m_map.remove(key);
    }


    static void setValue(QAnyStringView key, const QVariant& value);

    static QVariant value(QAnyStringView key);

    // registerSignal function template, which accepts only Lambda expressions


    // static void run()
    // {
    //     for (const auto& a : qAsConst(instance().m_map))
    //     {
    //         a(QVariant(999));
    //     }
    // }


private:
    static Settings& instance();

private:
    Settings() : m_settings(CONFIG_INI, QSettings::IniFormat) {}
    QSettings m_settings;
    QMap<QString, std::function<void(const QVariant&)>> m_map;


    struct SettingItem
    {
        enum ValueErrorProcessing : size_t
        {
            // 保持当前值、恢复默认值、自定义处理
            KeepCurrentValue = 0,
            RestoreDefaultValue,
            CustomProcessing
        };

        QString key;
        QVariant defaultValue;
        std::function<void(const QVariant&)> callbackFunc;
        std::function<bool(void)> checkValueFunc;
        ValueErrorProcessing errorProcessing;
        std::function<void(void)> customProcessingFunc;
    };

    QList<SettingItem> m_settingsList;
    QHash<QAnyStringView, qsizetype> m_settingsIndex;

    

signals:
    void configChanged(QAnyStringView, const QVariant&);
};

} // namespace lzl::qt_utils

#endif // LZL_QT_UTILS_SETTINGS_H
