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

namespace lzl {

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

    class GroupId final
    {
        friend class Settings;

    public:
        GroupId(size_t id = 0) : m_id(id) {}
        GroupId(const GroupId&) = default;
        GroupId& operator=(const GroupId&) = default;
        ~GroupId() = default;

        operator size_t() const { return m_id; }
        friend bool operator==(const GroupId& lhs, const GroupId& rhs) { return lhs.m_id == rhs.m_id; }
        friend bool operator<(const GroupId& lhs, const GroupId& rhs) { return lhs.m_id < rhs.m_id; }

    private:
        size_t m_id;

        GroupId& operator=(size_t id)
        {
            m_id = id;
            return *this;
        }
    };

    /**
     * @brief reset 清空设置文件
     */
    static void reset() { instance().m_q_settings.clear(); }

    /**
     * @brief reset 清空设置
     * @param key 设置的键
     */
    static void reset(const QString& key);

    /**
      * @brief reset 清空设置
      * @param keys 设置的键列表
      */
    static void reset(const QStringList& keys);

    /**
     * @brief reset 清空设置
     * @param group 分组号
     */
    static void reset(GroupId group);

    /**
     * @brief sync 同步设置
     */
    static void sync() { instance().m_q_settings.sync(); }

    /**
     * @brief containsSetting 是否包含设置
     * @param key 设置的键
     * @return 是否包含
     */
    static bool containsSetting(const QString& key) { return instance().m_regedit.contains({key}); }

    /**
     * @brief registerSetting 注册设置
     * @param key 设置的键
     * @param default_value 默认值
     * @param check 检查默认值是否合法
     * @param group 分组号
     */
    static void registerSetting(
        const QString& key,
        const QVariant& default_value = QVariant(),
        std::function<bool(const QVariant&)> check = [](const QVariant&) { return true; },
        GroupId group = 0
    );

    /**
     * @brief registerSetting 注册设置
     * @param key 设置的键
     * @param default_value 默认值
     * @param obj 对象
     * @param check 对象成员函数检查默认值是否合法
     * @param group 分组号
     */
    template <typename Class>
    static void registerSetting(
        const QString& key,
        const QVariant& default_value,
        Class* obj,
        bool (Class::*check)(const QVariant&),
        GroupId group = 0
    );

    /**
     * @brief deRegisterSetting 注销设置
     * @param key 设置的键
     */
    static void deRegisterSetting(const QString& key);

    /**
     * @brief deRegisterSetting 注销设置
     * @param key 设置的键
     */
    static void deRegisterSettings(const QStringList& keys);

    /**
     * @brief deRegisterSettings 注销设置
     * @param group 分组号
     */
    static void deRegisterSettings(GroupId group);

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
    static void readValue(const QString& key, lzl::utils::trains_class_type<Func>* obj, Func func);

    /**
     * @brief connectReadValue 连接读取设置
     * @param key 设置的键
     * @param func 读取设置的回调函数
     * @param group 分组号
     * @return 连接的 id
     */
    template <typename Func, typename = std::enable_if_t<!std::is_member_function_pointer<Func>::value>>
    static ConnId connectReadValue(const QString& key, Func func, GroupId group = 0);

    /**
     * @brief connectReadValue 连接读取设置
     * @param key 设置的键
     * @param obj 对象
     * @param func 对象成员函数读取设置的回调函数
     * @param group 分组号
     * @return 连接的 id
     */
    template <typename Func, typename = std::enable_if_t<std::is_member_function_pointer<Func>::value>>
    static ConnId connectReadValue(
        const QString& key, lzl::utils::trains_class_type<Func>* obj, Func func, GroupId group = 0
    );

    /**
     * @brief disconnectReadValue 断开连接读取设置
     * @param id 连接的 id, Q_ASSERT(!id.isNull());
     */
    static void disconnectReadValue(ConnId id);

    /**
     * @brief disconnectReadValue 断开分组的连接
     * @param group 分组号
     */
    static void disconnectReadValue(GroupId group);

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
     * @brief emitReadValue 触发读取设置信号
     * @param group 分组号
     */
    static void emitReadValues(GroupId group);

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

    struct RegRecord
    {
        QString key;
        std::function<bool(const QVariant&)> check = nullptr;
        QVariant default_value = {};
        GroupId group = 0;
        mutable QMap<ConnId, GroupId> conns = {};
        friend bool operator==(const RegRecord& lhs, const RegRecord& rhs) { return lhs.key == rhs.key; }
        friend auto qHash(const RegRecord& setting) { return qHash(setting.key); }
    };

    using SetType = QSet<RegRecord>;
    SetType m_regedit;
    QSettings m_q_settings;
    QMap<GroupId, QSet<QString>> m_regs_groups;
    QMap<GroupId, QSet<ConnId>> m_conns_groups;
    QMap<ConnId, QPair<std::function<void(void)>, QString>> m_conns;

private:
    SetType::iterator find(const QString& key);
    QVariant getValue(const QString& key);
    static ConnId idGenerator();
};

/* ========================================================================== */

template <typename Class>
inline void Settings::registerSetting(
    const QString& key, const QVariant& default_value, Class* obj, bool (Class::*check)(const QVariant&), GroupId group
)
{
    registerSetting(key, default_value, std::bind(check, obj, std::placeholders::_1), group);
}

template <typename Func>
inline void Settings::readValue(const QString& key, Func func)
{
    using arg_type = typename lzl::utils::function_traits<Func>::template arg<0>::type;
    Q_STATIC_ASSERT(lzl::utils::function_traits<Func>::arity == 1);
    func(lzl::utils::ConvertQVariant<arg_type>::convert(instance().getValue(key)));
}

template <typename Func>
inline void Settings::readValue(const QString& key, lzl::utils::trains_class_type<Func>* obj, Func func)
{
    using arg_type = typename lzl::utils::function_traits<Func>::template arg<0>::type;
    Q_STATIC_ASSERT(lzl::utils::function_traits<Func>::arity == 1);
    (obj->*func)(lzl::utils::ConvertQVariant<arg_type>::convert(instance().getValue(key)));
}

template <typename Func, typename>
inline Settings::ConnId Settings::connectReadValue(const QString& key, Func func, GroupId group)
{
    auto id = idGenerator();
    instance().find(key)->conns.insert(id, group);
    instance().m_conns_groups[group].insert(id);
    instance().m_conns.insert(id, {[key, func]() { instance().readValue(key, func); }, key});
    return id;
}

template <typename Func, typename>
inline Settings::ConnId Settings::connectReadValue(
    const QString& key, lzl::utils::trains_class_type<Func>* obj, Func func, GroupId group
)
{
    auto id = idGenerator();
    instance().find(key)->conns.insert(id, group);
    instance().m_conns_groups[group].insert(id);
    instance().m_conns.insert(id, {[key, obj, func]() { instance().readValue(key, obj, func); }, key});
    return id;
}

} // namespace lzl

#endif // ___LZL_QT_UTILS__LZL_QT_SETTINGS_H__
