/**
 * License: GPL-3.0
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
 * @note 0.3 之后启用 GroupId 而是直接使用路径作为分组（分节）
 */
class LZL_QT_SETTINGS_EXPORT Settings final
{
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(Settings&&) = delete;

public:
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
     * @brief reset 清空设置文件
     */
    static void reset() { instance().m_q_settings.clear(); }

    /**
     * @brief reset 清空设置
     * @param path 设置的键或组
     */
    static void reset(const QString& path) { instance().m_q_settings.remove(path); }

    /**
     * @brief sync 同步设置
     */
    static void sync() { instance().m_q_settings.sync(); }

    /**
     * @brief containsKey 是否注册过设置
     * @param key 注册的键
     * @return 是否注册过
     */
    static bool containsKey(const QString& key) { return instance().m_regedit.containsData(key); }

    /**
     * @brief containsGroup 是否存在组
     * @param path 组的路径
     * @return 是否存在组
     */
    static bool containsGroup(const QString& path) { return true; }

    /**
     * @brief registerSetting 注册设置
     * @param key 设置的键
     * @param default_value 默认值
     * @param check_func 检查默认值是否合法
     */
    static void registerSetting(
        const QString& key,
        const QVariant& default_value = QVariant(),
        std::function<bool(const QVariant&)> check_func = [](const QVariant&) { return true; }
    );

    /**
     * @brief registerSetting 注册设置
     * @param key 设置的键
     * @param default_value 默认值
     * @param object 对象
     * @param check_func 对象成员函数检查默认值是否合法
     */
    template <typename Class>
    static void registerSetting(
        const QString& key, const QVariant& default_value, Class* object, bool (Class::*check_func)(const QVariant&)
    );

    /**
     * @brief deRegisterSetting 注销设置
     * @param key 设置的键
     */
    static void deRegisterSetting(const QString& key) { instance().m_regedit.removeData(key); }

    /**
     * @brief deRegisterSettings 注销设置
     * @param path 组的路径
     */
    static void deRegisterGroup(const QString& path) { instance().m_regedit.removeGroup(path); }

    /**
     * @brief deRegisterAllSettings 注销所有设置
     */
    static void deRegisterAllSettings();

    /**
     * @brief writeValue 写入设置
     * @param key 设置的键
     * @param value 设置的值
     * @param emitRead 是否触发读取设置信号
     * @return 是否写入成功
     */
    static bool writeValue(const QString& key, const QVariant& value, bool emitSignal = false);

    /**
     * @brief readValue 读取设置
     * @param key 设置的键
     * @param func 读取设置的回调函数
     */
    template <typename Func>
    static void readValue(const QString& key, Func func);

    /**
     * @brief readValue 读取设置
     * @param key 设置的键
     * @param obj 对象
     * @param func 对象成员函数读取设置的回调函数
     */
    template <typename Func>
    static void readValue(const QString& key, trains_class_type<Func>* obj, Func func);

    /**
     * @brief connectReadValue 连接读取设置
     * @param key 设置的键
     * @param func 读取设置的回调函数
     * @return 连接的 id
     */
    template <typename Func, typename = std::enable_if_t<!std::is_member_function_pointer<Func>::value>>
    static ConnId connectReadValue(const QString& key, Func func);

    /**
     * @brief connectReadValue 连接读取设置
     * @param key 设置的键
     * @param obj 对象
     * @param func 对象成员函数读取设置的回调函数
     * @param group 分组号
     * @return 连接的 id
     */
    template <typename Func, typename = std::enable_if_t<std::is_member_function_pointer<Func>::value>>
    static ConnId connectReadValue(const QString& key, trains_class_type<Func>* obj, Func func);

    /**
     * @brief disconnectReadValue 断开连接读取设置
     * @param id 连接的 id, Q_ASSERT(!id.isNull());
     */
    static void disconnectReadValue(ConnId id);

    /**
     * @brief disconnectAllReadValues 断开所有读取设置连接
     */
    static void disconnectAllReadValues();

    /**
     * @brief emitReadValue 触发读取设置信号
     * @param key 设置的键
     */
    static void emitReadValue(const QString& key);

    /**
     * @brief emitReadValues 触发读取设置信号
     * @param keys 设置的键列表
     */
    static void emitReadValues(const QStringList& keys);

    /**
     * @brief emitReadAllValues 触发所有读取设置信号
     */
    static void emitReadAllValues();

    /**
     * @brief readValueIds 读取设置的 id 列表
     * @param key 设置的键 为空则返回所有 id 列表
     * @return id 列表
     */
    static QList<Settings::ConnId> readValueIds(const QString& key = QString());

private:
    static Settings& instance();
    Settings(const QString& filename);
    ~Settings() = default;

private:
    struct RegData
    {
        QString key;
        QVariant default_value = {};
        std::function<bool(const QVariant&)> check = nullptr;
        mutable QList<ConnId> conns = {};
        friend bool operator==(const RegData& l, const RegData& r) { return l.key == r.key; }
        friend auto qHash(const RegData& setting) { return qHash(setting.key); }
    };
    struct RegGroup
    {
        using DataSet = QSet<RegData>;
        using GroupSet = QMap<QString, RegGroup>;
        DataSet dataset;
        GroupSet groupset;

        using dataset_iterator = const RegData*;
        using groupset_iterator = const RegGroup*;

        bool containsData(const QString& key) { return findData(key) != dataEnd(); }
        bool containsGroup(const QStringList& path) { return findGroup(path) != groupEnd(); }

        dataset_iterator dataEnd() { return nullptr; }
        groupset_iterator groupEnd() { return nullptr; }

        dataset_iterator findData(const QString& key);
        groupset_iterator findGroup(const QStringList& path);

        void insertData(const QString& key, const QVariant& default_value, std::function<bool(const QVariant&)> check);
        void removeData(const QString& key);
        void removeGroup(const QString& path);

        static QPair<QStringList, QString> parsePath(const QString& path);
    };

    RegGroup m_regedit;
    QSettings m_q_settings;

private:
    static ConnId idGenerator();

    static QPair<QStringList, QString> parsePath(const QString& path);

private:
    static QMap<ConnId, QPair<std::function<void(void)>, std::function<void(void)>>> s_conns;

private:
    // SetType::iterator findKey(const QString& key);
    // QPair<SetType::iterator, SetType::iterator> findGroup(const QString& group);
    QVariant getValue(const QString& key);
};

/* ========================================================================== */

inline void Settings::registerSetting(
    const QString& key, const QVariant& default_value, std::function<bool(const QVariant&)> check
)
{
    instance().m_regedit.insertData(key, default_value, check);
}

template <typename Class>
inline void Settings::registerSetting(
    const QString& key, const QVariant& default_value, Class* obj, bool (Class::*check)(const QVariant&)
)
{
    registerSetting(key, default_value, [obj, check](const QVariant& value) { return (obj->*check)(value); });
}

template <typename Func>
inline void Settings::readValue(const QString& key, Func func)
{
    using arg_type = typename function_traits<Func>::template arg<0>::type;
    Q_STATIC_ASSERT(function_traits<Func>::arity == 1);
    func(ConvertQVariant<arg_type>::convert(instance().getValue(key)));
}

template <typename Func>
inline void Settings::readValue(const QString& key, trains_class_type<Func>* obj, Func func)
{
    using arg_type = typename function_traits<Func>::template arg<0>::type;
    Q_STATIC_ASSERT(function_traits<Func>::arity == 1);
    (obj->*func)(ConvertQVariant<arg_type>::convert(instance().getValue(key)));
}

template <typename Func, typename>
inline Settings::ConnId Settings::connectReadValue(const QString& key, Func func)
{
    auto id = idGenerator();
    // instance().findKey(key)->conns.insert(id, group);
    // instance().m_conns_groups[group].insert(id);
    // instance().m_conns.insert(id, {[key, func]() { instance().readValue(key, func); }, key});
    return id;
}

template <typename Func, typename>
inline Settings::ConnId Settings::connectReadValue(const QString& key, trains_class_type<Func>* obj, Func func)
{
    auto id = idGenerator();
    // instance().findKey(key)->conns.insert(id, group);
    // instance().m_conns_groups[group].insert(id);
    // instance().m_conns.insert(id, {[key, obj, func]() { instance().readValue(key, obj, func); }, key});
    return id;
}

} // namespace lzl::utils

#endif // ___LZL_QT_UTILS__LZL_QT_SETTINGS_H__
