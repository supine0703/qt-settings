#include "lzl_settings.h"

#ifndef CONFIG_INI
    #define CONFIG_INI "config.ini"
#endif

using namespace lzl;

Settings& Settings::instance()
{
    static Settings settings(CONFIG_INI);
    return settings;
}

void Settings::reset(const QString& key)
{
    instance().m_q_settings.remove(instance().find(key)->key);
}

void Settings::reset(const QStringList& keys)
{
    for (auto& key : keys)
    {
        reset(key);
    }
}

void Settings::reset(GroupId group)
{
    Q_ASSERT_X(
        instance().m_regs_groups.contains(group),
        Q_FUNC_INFO,
        QStringLiteral("Group not found id: %1").arg(group).toUtf8().constData()
    );
    for (auto& key : instance().m_regs_groups[group])
    {
        instance().m_q_settings.remove(key);
    }
}

void Settings::registerSetting(
    const QString& key, const QVariant& default_value, std::function<bool(const QVariant&)> check, GroupId group
)
{
    RegRecord setting{key, check, default_value, group};
    Q_ASSERT_X(
        !instance().m_regedit.contains(setting),
        Q_FUNC_INFO,
        QStringLiteral("Setting registration record already exists: %1").arg(key).toUtf8().constData()
    );
    Q_ASSERT_X(
        check(default_value),
        Q_FUNC_INFO,
        QStringLiteral("Setting default value check failed: %1").arg(key).toUtf8().constData()
    );
    instance().m_regedit.insert(setting);
    instance().m_regs_groups[group].insert(key);
}

void Settings::deRegisterSetting(const QString& key)
{
    auto it = instance().find(key);
    // 取消读取绑定
    const auto conns = it->conns.keys();
    for (auto conn : conns)
    {
        disconnectReadValue(conn);
    }
    // 从组中移除
    instance().m_regs_groups[it->group].remove(key);
    if (instance().m_regs_groups[it->group].isEmpty())
    {
        instance().m_regs_groups.remove(it->group);
    }
    // 从注册表中移除
    instance().m_regedit.remove(*it);
}

void Settings::deRegisterSettings(const QStringList& keys)
{
    for (auto& key : keys)
    {
        deRegisterSetting(key);
    }
}

void Settings::deRegisterSettings(GroupId group)
{
    Q_ASSERT_X(
        instance().m_regs_groups.contains(group),
        Q_FUNC_INFO,
        QStringLiteral("Group not found id: %1").arg(group).toUtf8().constData()
    );
    deRegisterSettings(instance().m_regs_groups[group].values());
}

void Settings::deRegisterAllSettings()
{
    instance().m_regedit.clear();
    instance().m_regs_groups.clear();
    instance().m_conns.clear();
    instance().m_conns_groups.clear();
}

bool Settings::writeValue(const QString& key, const QVariant& value, bool emitSignal)
{
    if (instance().find(key)->check(value))
    {
        instance().m_q_settings.setValue(key, value);
        if (emitSignal)
        {
            emitReadValue(key);
        }
        return true;
    }
    return false;
}

void Settings::disconnectReadValue(ConnId id)
{
    Q_ASSERT(!id.isNull());
    Q_ASSERT_X(
        instance().m_conns.contains(id),
        Q_FUNC_INFO,
        QStringLiteral("Connection not found id: %1").arg(id).toUtf8().constData()
    );
    auto key = instance().m_conns.take(id).second; // remove from m_conns
    auto it = instance().find(key);
    Q_ASSERT_X(
        it->conns.contains(id),
        Q_FUNC_INFO,
        QStringLiteral("Connection not found key: %1").arg(key).toUtf8().constData()
    );
    auto group = it->conns.take(id); // remove from regedit
    Q_ASSERT_X(
        instance().m_conns_groups.contains(group),
        Q_FUNC_INFO,
        QStringLiteral("Group not found id: %1").arg(group).toUtf8().constData()
    );
    instance().m_conns_groups[group].remove(id); // remove from m_conns_groups
    if (instance().m_conns_groups[group].isEmpty())
    {
        instance().m_conns_groups.remove(group);
    }
}

void Settings::disconnectReadValue(GroupId group)
{
    Q_ASSERT_X(
        instance().m_conns_groups.contains(group),
        Q_FUNC_INFO,
        QStringLiteral("Group not found id: %1").arg(group).toUtf8().constData()
    );
    const auto ids = instance().m_conns_groups.take(group); // remove from m_conns_groups
    for (auto& id : ids)
    {
        auto key = instance().m_conns.take(id).second; // remove from m_conns
        auto it = instance().find(key);
        Q_ASSERT_X(
            it->conns.contains(id),
            Q_FUNC_INFO,
            QStringLiteral("Connection not found key: %1").arg(key).toUtf8().constData()
        );
        it->conns.remove(id); // remove from regedit
    }
}

void Settings::disconnectAllReadValues()
{
    instance().m_conns.clear();
    instance().m_conns_groups.clear();
    for (auto& setting : std::as_const(instance().m_regedit))
    {
        setting.conns.clear();
    }
}

void Settings::emitReadValue(const QString& key)
{
    const auto conns = instance().find(key)->conns.keys();
    for (auto& conn : conns)
    {
        instance().m_conns[conn].first();
    }
}

void Settings::emitReadValues(const QStringList& keys)
{
    for (auto& key : keys)
    {
        emitReadValue(key);
    }
}

void Settings::emitReadValues(GroupId group)
{
    Q_ASSERT_X(
        instance().m_conns_groups.contains(group),
        Q_FUNC_INFO,
        QStringLiteral("Group not found id: %1").arg(group).toUtf8().constData()
    );
    const auto conns = instance().m_conns_groups[group].values();
    for (auto& id : conns)
    {
        instance().m_conns[id].first();
    }
}

void Settings::emitReadAllValues()
{
    for (auto& [func, _] : std::as_const(instance().m_conns))
    {
        func();
    }
}

QList<Settings::ConnId> Settings::readValueIds(const QString& key)
{
    if (key.isEmpty())
    {
        return instance().m_conns.keys();
    }
    auto it = instance().find(key);
    return it->conns.keys();
}

/* ========================================================================== */

Settings::Settings(const QString& filename) : m_q_settings(filename, QSettings::IniFormat)
{
    ::qRegisterMetaType<ConnId>("Settings::ConnId");
    ::qRegisterMetaType<GroupId>("Settings::GroupId");
}

Settings::SetType::iterator Settings::find(const QString& key)
{
    auto it = m_regedit.find({key});
    Q_ASSERT_X(
        it != m_regedit.end(),
        Q_FUNC_INFO,
        QStringLiteral("Setting not registered id: %1").arg(key).toUtf8().constData()
    );
    return it;
}

QVariant Settings::getValue(const QString& key)
{
    auto it = find(key);
    auto value = m_q_settings.value(key, it->default_value);
    if (!it->check(value))
    {
        m_q_settings.remove(key);
        return it->default_value;
    }
    return value;
}

Settings::ConnId Settings::idGenerator()
{
    static ConnId id = 0;
    return (++id).isNull() ? ++id : id;
}
