/**
 * License: GPL-3.0
 * Copyright (c) 2024 李宗霖 (Li Zonglin)
 * Email: supine0703@outlook.com
 * GitHub: https://github.com/supine0703
 * Repository: qt-settings <https://github.com/supine0703/qt-settings>
 */

#include "lzl_settings.h"

#include <QRegularExpression>

#ifndef CONFIG_INI
    #define CONFIG_INI "config.ini"
#endif

using namespace lzl::utils;

Settings& Settings::instance()
{
    static Settings settings(CONFIG_INI);
    return settings;
}

Settings::Settings(const QString& filename) : m_q_settings(filename, QSettings::IniFormat)
{
    ::qRegisterMetaType<ConnId>("Settings::ConnId");
}

/* ========================================================================== */

Settings::RegGroup::dataset_iterator Settings::RegGroup::findData(const QString& key)
{
    auto [path, name] = parsePath(key);
    auto group_it = findGroup(path);
    if (group_it == groupEnd())
    {
        return dataEnd();
    }
    auto data_it = group_it->dataset.find({name});
    return data_it == group_it->dataset.end() ? dataEnd() : &(*data_it);
}

Settings::RegGroup::groupset_iterator Settings::RegGroup::findGroup(const QStringList& path)
{
    auto group = this;
    for (auto& word : path)
    {
        auto it = group->groupset.find(word);
        if (it == group->groupset.end())
        {
            return groupEnd();
        }
        group = &it.value();
    }
    return group;
}

void Settings::RegGroup::insertData(
    const QString& key, const QVariant& default_value, std::function<bool(const QVariant&)> check
)
{
    auto [path, name] = RegGroup::parsePath(key);
    auto group = this;
    for (auto& word : path)
    {
        group = &(group->groupset[word]);
    }

    Q_ASSERT_X(
        !group->dataset.contains({name}),
        Q_FUNC_INFO,
        QStringLiteral("Setting registration record already exists: %1").arg(key).toUtf8().constData()
    );
    Q_ASSERT_X(
        check(default_value),
        Q_FUNC_INFO,
        QStringLiteral("Setting default value check failed: %1").arg(key).toUtf8().constData()
    );

    group->dataset.insert({key, default_value, check});
}

void Settings::RegGroup::removeData(const QString& key)
{
    if (key.isEmpty())
    {
        return;
    }

    // 查找组
    auto [path, name] = parsePath(key);
    auto groups = QList({this});
    for (auto& word : path)
    {
        groups.append(&groups.last()->groupset[word]);
    }

    Q_ASSERT_X(
        groups.last()->dataset.contains({name}),
        Q_FUNC_INFO,
        QStringLiteral("Setting registration record not found: %1").arg(key).toUtf8().constData()
    );

    // 删除数据
    groups.last()->dataset.remove({name});
    while (!groups.isEmpty())
    {
        if (!groups.last()->dataset.isEmpty() || !groups.last()->groupset.isEmpty())
        {
            break;
        }
        groups.pop_back();
        if (!groups.isEmpty())
        {
            groups.last()->groupset.remove(path.takeLast());
        }
    }
}

void Settings::RegGroup::removeGroup(const QString& path)
{
    if (path.isEmpty())
    {
        return;
    }

    // 查找组
    auto [prePath, groupName] = parsePath(path);
    auto groups = QList({this});
    for (auto& word : prePath)
    {
        groups.append(&groups.last()->groupset[word]);
    }

    Q_ASSERT_X(
        groups.last()->dataset.contains({groupName}),
        Q_FUNC_INFO,
        QStringLiteral("Group registration record not found: %1").arg(path).toUtf8().constData()
    );

    // 删除组
    groups.pop_back();
    while (!groups.isEmpty())
    {
        if (!groups.last()->dataset.isEmpty() || !groups.last()->groupset.isEmpty())
        {
            break;
        }
        groups.pop_back();
        if (!groups.isEmpty())
        {
            groups.last()->groupset.remove(prePath.takeLast());
        }
    }
}

QPair<QStringList, QString> Settings::RegGroup::parsePath(const QString& key)
{
    static const QRegularExpression re(QStringLiteral(R"([/\\])"));
    auto path = key.split(re, Qt::SkipEmptyParts);
    auto name = path.takeLast();
    return {path, name};
}

/* ========================================================================== */

void Settings::deRegisterAllSettings()
{
    // instance().m_regedit.clear();
    // instance().m_regs_groups.clear();
    // instance().m_conns.clear();
    // instance().m_conns_groups.clear();
}

bool Settings::writeValue(const QString& key, const QVariant& value, bool emitSignal)
{
    // if (instance().findKey(key)->check(value))
    // {
    //     instance().m_q_settings.setValue(key, value);
    //     if (emitSignal)
    //     {
    //         emitReadValue(key);
    //     }
    //     return true;
    // }
    // return false;
}

void Settings::disconnectReadValue(ConnId id)
{
    // Q_ASSERT(!id.isNull());
    // Q_ASSERT_X(
    //     instance().m_conns.contains(id),
    //     Q_FUNC_INFO,
    //     QStringLiteral("Connection not found id: %1").arg(id).toUtf8().constData()
    // );
    // auto key = instance().m_conns.take(id).second; // remove from m_conns
    // auto it = instance().findKey(key);
    // Q_ASSERT_X(
    //     it->conns.contains(id),
    //     Q_FUNC_INFO,
    //     QStringLiteral("Connection not found key: %1").arg(key).toUtf8().constData()
    // );
    // auto group = it->conns.take(id); // remove from regedit
    // Q_ASSERT_X(
    //     instance().m_conns_groups.contains(group),
    //     Q_FUNC_INFO,
    //     QStringLiteral("Group not found id: %1").arg(group).toUtf8().constData()
    // );
    // instance().m_conns_groups[group].remove(id); // remove from m_conns_groups
    // if (instance().m_conns_groups[group].isEmpty())
    // {
    //     instance().m_conns_groups.remove(group);
    // }
}

void Settings::disconnectAllReadValues()
{
    // instance().m_conns.clear();
    // instance().m_conns_groups.clear();
    // for (auto& setting : std::as_const(instance().m_regedit))
    // {
    //     setting.conns.clear();
    // }
}

void Settings::emitReadValue(const QString& key)
{
    // const auto conns = instance().findKey(key)->conns.keys();
    // for (auto& conn : conns)
    // {
    //     instance().m_conns[conn].first();
    // }
}

void Settings::emitReadValues(const QStringList& keys)
{
    // for (auto& key : keys)
    // {
    //     emitReadValue(key);
    // }
}

void Settings::emitReadAllValues()
{
    // for (auto& [func, _] : std::as_const(instance().m_conns))
    // {
    //     func();
    // }
}

QList<Settings::ConnId> Settings::readValueIds(const QString& key)
{
    // if (key.isEmpty())
    // {
    //     return instance().m_conns.keys();
    // }
    // auto it = instance().findKey(key);
    // return it->conns.keys();
}

/* ========================================================================== */

/* ========================================================================== */

Settings::ConnId Settings::idGenerator()
{
    static ConnId id = 0;
    do
    {
        ++id;
    } while (id.isNull() || s_conns.contains(id));
    return id;
}
