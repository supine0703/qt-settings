/**
 * License: LGPL-2.1
 * Copyright (c) 2024 李宗霖 (Li Zonglin)
 * Email: supine0703@outlook.com
 * GitHub: https://github.com/supine0703
 * Repository: qt-settings <https://github.com/supine0703/qt-settings>
 */

#ifndef ___LZL_QT_UTILS__LZL_QT_SETTINGS_H__
#define ___LZL_QT_UTILS__LZL_QT_SETTINGS_H__

#include "lzl_convert_qt_variant.h"
#include "lzl_function_traits.h"
#include "lzl_lib_settings_exports.h"

#include <QMap>
#include <QSet>
#include <QSettings>

namespace lzl::utils {

/** 
 * @version 0.3.x
 * @note 0.3 之后弃用 GroupId 而是直接使用路径作为分组（分节）
 * @note 注意变量名的含义：
 *   - @param key 设置的键，是完整路径，如：app/font/size
 *   - @param dir 组的路径，是完整路径，如：app/font
 *   - @param path 键或组的路径，如：app/font/size、app/font
 *   - @param word 路径的一部分，如：app、font、size
 *   - @param name 路径的最后一部分，如：size、font
 * @example
 */
class LZL_QT_SETTINGS_EXPORT Settings final
{
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(Settings&&) = delete;

    // 对外的接口
public:
    /**
     * @brief ConnId 绑定读取事件的 id 类
     */
    class ConnId final
    {
        friend class Settings;

    public:
        ConnId() = default;
        ConnId(const ConnId&) = default;
        ConnId& operator=(const ConnId&) = default;
        ~ConnId() = default;
        bool isNull() const { return this->m_id == 0; }

        operator void*() const { return m_id; }
        operator uintptr_t() const { return reinterpret_cast<uintptr_t>(m_id); }
        friend bool operator==(const ConnId& lhs, const ConnId& rhs) { return lhs.m_id == rhs.m_id; }
        friend bool operator<(const ConnId& lhs, const ConnId& rhs) { return lhs.m_id < rhs.m_id; }

    private:
        void* m_id;

        ConnId(void* id) : m_id(id) {}
        ConnId& operator=(void* id)
        {
            m_id = id;
            return *this;
        }
        ConnId& operator++()
        {
            m_id = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_id) + 1);
            return *this;
        }
    };

    /**
     * @brief sync 同步设置
     */
    static void sync() { instance().m_q_settings.sync(); }

    /**
     * @brief reset 清空设置文件
     */
    static void reset() { instance().m_q_settings.clear(); }

    /**
     * @brief reset 清空设置
     * @param path 键或组的路径
     */
    static void reset(const QString& path) { instance().m_q_settings.remove(path); }

    /**
     * @brief containsKey 是否注册过设置
     * @param key 键的值
     * @return 是否注册过
     */
    static bool containsKey(const QString& key) { return instance().m_regedit.containsData(key); }

    /**
     * @brief containsGroup 是否存在组
     * @param dir 组的路径
     * @return 是否存在组
     */
    static bool containsGroup(const QString& dir) { return instance().m_regedit.containsGroup(dir); }

    /**
     * @brief registerSetting 注册设置
     * @param key 要注册的键
     * @param default_value 默认值
     * @param check_func 检查默认值是否合法
     */
    static void registerSetting(
        const QString& key,
        const QVariant& default_value = QVariant(),
        std::function<bool(const QVariant&)> check_func = [](const QVariant&) { return true; }
    )
    {
        instance().m_regedit.insertData(key, default_value, check_func);
    }

    /**
     * @brief registerSetting 注册设置
     * @param key 要注册的键
     * @param default_value 默认值
     * @param object 对象
     * @param check_func 对象成员函数检查默认值是否合法
     */
    template <typename Class>
    static void registerSetting(
        const QString& key, const QVariant& default_value, Class* object, bool (Class::*check_func)(const QVariant&)
    );

    /**
     * @brief deRegisterSettingKey 注销设置
     * @param key 注册过的键
     */
    static void deRegisterSettingKey(const QString& key) { instance().m_regedit.removeData(key); }

    /**
     * @brief deRegisterSettingGroup 注销设置
     * @param dir 存在的组
     */
    static void deRegisterSettingGroup(const QString& dir) { instance().m_regedit.removeGroup(dir); }

    /**
     * @brief deRegisterAllSettings 注销所有设置
     */
    static void deRegisterAllSettings() { instance().m_regedit.clear(); }

    /**
     * @brief writeValue 写入设置
     * @param key 注册过的键
     * @param value 设置的值
     * @param emit_signal 是否触发读取事件信号
     * @return 是否写入成功
     */
    static bool writeValue(const QString& key, const QVariant& value, bool emit_signal = false);

    /**
     * @brief readValue 读取设置
     * @param key 注册过的键
     * @param read_func 读取设置的回调函数
     */
    template <typename Func>
    static void readValue(const QString& key, Func read_func);

    /**
     * @brief readValue 读取设置
     * @param key 注册过的键
     * @param object 对象
     * @param read_func 对象成员函数读取设置的回调函数
     */
    template <typename Func>
    static void readValue(const QString& key, trains_class_type<Func>* object, Func read_func);

    /**
     * @brief connectReadValue 绑定读取事件
     * @param key 注册过的键
     * @param read_func 读取设置的回调函数
     * @return 读取事件的 id
     */
    template <typename Func, typename = std::enable_if_t<!std::is_member_function_pointer<Func>::value>>
    static ConnId connectReadValue(const QString& key, Func read_func);

    /**
     * @brief connectReadValue 绑定读取事件
     * @param key 注册过的键
     * @param object 对象
     * @param read_func 对象成员函数读取设置的回调函数
     * @param group 分组号
     * @return 读取事件的 id
     */
    template <typename Func, typename = std::enable_if_t<std::is_member_function_pointer<Func>::value>>
    static ConnId connectReadValue(const QString& key, trains_class_type<Func>* object, Func read_func);

    /**
     * @brief disconnectReadValue 解绑读取事件
     * @param id 读取事件的 id, Q_ASSERT(!id.isNull());
     */
    static void disconnectReadValue(ConnId id);

    /**
     * @brief disconnectReadValuesFromKey 解绑读取事件
     * @param key 注册过的键
     */
    static void disconnectReadValuesFromKey(const QString& key);

    /**
     * @brief disconnectReadValuesFromGroup 解绑读取事件
     * @param dir 存在的组
     */
    static void disconnectReadValuesFromGroup(const QString& dir);

    /**
     * @brief disconnectAllSettingsReadValues 解绑所有读取事件
     */
    static void disconnectAllSettingsReadValues();

    /**
     * @brief emitReadValue 触发读取事件信号
     * @param id 读取事件的 id, Q_ASSERT(!id.isNull());
     */
    static void emitReadValue(ConnId id);

    /**
     * @brief emitReadValuesFromKey 触发读取事件信号
     * @param key 注册过的键
     */
    static void emitReadValuesFromKey(const QString& key);

    /**
     * @brief emitReadValuesFromGroup 触发读取事件信号
     * @param dir 存在的组
     */
    static void emitReadValuesFromGroup(const QString& dir);

    /**
     * @brief emitAllSettingsReadValues 触发所有读取事件信号
     */
    static void emitAllSettingsReadValues();

    /**
     * @brief getConnIds 获取所有的读取事件 id 列表
     * @return id 列表
     */
    static QList<ConnId> getConnIds() { return s_conns.keys(); }

    // 构造析构
private:
    static Settings& instance();
    explicit Settings(const QString& filename, QObject* parent = nullptr);
    ~Settings() = default;

    // 定义注册表
private:
    struct RegData
    {
        QVariant default_value = {};
        std::function<bool(const QVariant&)> check = nullptr;
        mutable QList<ConnId> conns = {};
        ~RegData();
    };
    struct RegGroup
    {
        using DataSet = QMap<QString, RegData>;
        using GroupSet = QMap<QString, RegGroup>;
        DataSet dataset;
        GroupSet groupset;

        // 基本函数
        bool isEmpty() const { return dataset.isEmpty() && groupset.isEmpty(); }
        void clear()
        {
            dataset.clear();
            groupset.clear();
        }

        // 通过迭代器 增删改查
        using dataset_iterator = DataSet::iterator;
        using groupset_iterator = GroupSet::iterator;

        dataset_iterator dataEnd() { return dataset.end(); }
        groupset_iterator groupEnd() { return groupset.end(); }

        bool containsData(const QString& key) { return findData(key) != dataEnd(); }
        bool containsGroup(const QString& dir) { return findGroup(dir) != groupEnd(); }
        bool containsGroup(const QStringList& dir) { return findGroup(dir) != groupEnd(); }

        dataset_iterator findData(const QString& key);
        groupset_iterator findGroup(const QString& dir);
        groupset_iterator findGroup(const QStringList& dir);

        void insertData(
            const QString& key, const QVariant& default_value, std::function<bool(const QVariant&)> check_func
        );
        void removeData(const QString& key);
        void removeGroup(const QString& dir);

        static QStringList detachPath(const QString& path);
        static QPair<QStringList, QString> parsePath(const QString& path);
    };

    RegGroup m_regedit;
    QSettings m_q_settings;

    // 一些非静态的辅助函数
private:
    RegGroup::dataset_iterator findRecord(const QString& key);
    RegGroup::groupset_iterator findRegGroup(const QString& dir);
    QVariant getValue(const QString& key);

    // 静态的辅助函数
private:
    static ConnId idGenerator();

    // 用作递归
    static void readValueFromGroup(const RegGroup* group);

    // 静态数据
private:
    struct ConnFunctions
    {
        std::function<void(void)> read;
        std::function<void(void)> disconnect;
    };
    static QMap<ConnId, ConnFunctions> s_conns;
};

// 下面是模板函数的实现
/* ========================================================================== */

template <typename Class>
inline void Settings::registerSetting(
    const QString& key, const QVariant& default_value, Class* object, bool (Class::*check_func)(const QVariant&)
)
{
    registerSetting(key, default_value, [object, check_func](const QVariant& value) {
        return (object->*check_func)(value);
    });
}

template <typename Func>
inline void Settings::readValue(const QString& key, Func read_func)
{
    using arg_type = typename function_traits<Func>::template arg<0>::type;
    Q_STATIC_ASSERT(function_traits<Func>::arity == 1);
    read_func(ConvertQVariant<arg_type>::convert(instance().getValue(key)));
}

template <typename Func>
inline void Settings::readValue(const QString& key, trains_class_type<Func>* object, Func read_func)
{
    using arg_type = typename function_traits<Func>::template arg<0>::type;
    Q_STATIC_ASSERT(function_traits<Func>::arity == 1);
    (object->*read_func)(ConvertQVariant<arg_type>::convert(instance().getValue(key)));
}

template <typename Func, typename>
inline Settings::ConnId Settings::connectReadValue(const QString& key, Func read_func)
{
    auto id = idGenerator();
    auto data = &(instance().findRecord(key).value());
    data->conns.append(id);
    s_conns.insert(
        id,
        {
            [key, read_func]() { instance().readValue(key, read_func); },
            [data, id]() { data->conns.removeOne(id); },
        }
    );
    return id;
}

template <typename Func, typename>
inline Settings::ConnId Settings::connectReadValue(const QString& key, trains_class_type<Func>* object, Func read_func)
{
    return connectReadValue(key, [object, read_func](auto&& value) {
        (object->*read_func)(std::forward<decltype(value)>(value));
    });
}

} // namespace lzl::utils

#endif // ___LZL_QT_UTILS__LZL_QT_SETTINGS_H__
