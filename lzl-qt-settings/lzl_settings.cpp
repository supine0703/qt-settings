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

Settings::Settings(const QString& filename, QObject* parent) : m_q_settings(filename, QSettings::IniFormat, parent)
{
    ::qRegisterMetaType<ConnId>("Settings::ConnId");
}

// 注册表相关类的成员函数
/* ========================================================================== */

Settings::RegData::~RegData()
{
    for (auto& conn : std::as_const(conns))
    {
        s_conns.remove(conn);
    }
}

Settings::RegGroup::dataset_iterator Settings::RegGroup::findData(const QString& key)
{
    auto [dir, name] = parsePath(key);

    if (auto group_it = findGroup(dir); group_it != groupEnd())
    {
        if (auto data_it = group_it->dataset.find({name}); data_it != group_it->dataset.end())
        {
            return data_it;
        }
    }

    return dataEnd();
}

Settings::RegGroup::groupset_iterator Settings::RegGroup::findGroup(const QString& dir)
{
    return findGroup(detachPath(dir));
}

Settings::RegGroup::groupset_iterator Settings::RegGroup::findGroup(const QStringList& dir)
{
    Settings::RegGroup::groupset_iterator group_it;
    auto group = this;
    for (auto& word : dir)
    {
        group_it = group->groupset.find(word);
        if (group_it == group->groupset.end())
        {
            return groupEnd();
        }
        group = &(group_it.value());
    }
    return group_it;
}

void Settings::RegGroup::insertData(
    const QString& key, const QVariant& default_value, std::function<bool(const QVariant&)> check_func
)
{
    Q_ASSERT(!key.isEmpty());

    // 查找组
    auto [dir, name] = parsePath(key);
    auto group = this;
    for (auto& word : std::as_const(dir))
    {
        group = &(group->groupset[word]);
    }

    Q_ASSERT_X(
        !group->dataset.contains({name}),
        Q_FUNC_INFO,
        QStringLiteral("Setting registration `record` already exists: %1").arg(key).toUtf8().constData()
    );
    Q_ASSERT_X(
        check_func(default_value),
        Q_FUNC_INFO,
        QStringLiteral("Setting default value check failed: %1").arg(key).toUtf8().constData()
    );

    // 插入数据
    group->dataset.insert(name, {default_value, check_func});
}

void Settings::RegGroup::removeData(const QString& key)
{
    Q_ASSERT(!key.isEmpty());

    // 查找组
    auto [dir, name] = parsePath(key);
    auto groups = QList({this});
    for (auto& word : std::as_const(dir))
    {
        groups.append(&(groups.last()->groupset[word]));
    }

    auto group = groups.takeLast();
    Q_ASSERT_X(
        group->dataset.contains({name}),
        Q_FUNC_INFO,
        QStringLiteral("Setting registration `record` not found: %1").arg(key).toUtf8().constData()
    );

    // 删除数据
    group->dataset.remove({name});

    // 清除空节点
    while (!groups.isEmpty() && group->dataset.isEmpty() && group->groupset.isEmpty())
    {
        group = groups.takeLast();
        group->groupset.remove(dir.takeLast());
    }
}

void Settings::RegGroup::removeGroup(const QString& dir)
{
    Q_ASSERT(!dir.isEmpty());

    // 查找组
    auto [preDir, groupName] = parsePath(dir);
    auto groups = QList({this});
    for (auto& word : std::as_const(preDir))
    {
        groups.append(&groups.last()->groupset[word]);
    }

    auto group = groups.takeLast();
    Q_ASSERT_X(
        group->groupset.contains(groupName),
        Q_FUNC_INFO,
        QStringLiteral("Setting registration `group` not found: %1").arg(dir).toUtf8().constData()
    );

    // 删除组
    group->groupset.remove(groupName);

    // 清除空节点
    while (!groups.isEmpty() && group->dataset.isEmpty() && group->groupset.isEmpty())
    {
        group = groups.takeLast();
        group->groupset.remove(preDir.takeLast());
    }
}

// 注册表相关类的静态
/* ========================================================================== */

QStringList Settings::RegGroup::detachPath(const QString& path)
{
    static const QRegularExpression re(QStringLiteral(R"([/\\])"));
    return path.split(re, Qt::SkipEmptyParts);
}

QPair<QStringList, QString> Settings::RegGroup::parsePath(const QString& path)
{
    auto dir = detachPath(path);
    auto name = dir.takeLast();
    return {dir, name};
}

// 主类的静态（对外接口）函数实现
/* ========================================================================== */

bool Settings::writeValue(const QString& key, const QVariant& value, bool emit_signal)
{
    if (instance().findRecord(key)->check(value))
    {
        instance().m_q_settings.setValue(key, value);
        if (emit_signal)
        {
            emitReadValuesFromKey(key);
        }
        return true;
    }
    return false;
}

void Settings::disconnectReadValue(ConnId id)
{
    Q_ASSERT(!id.isNull());
    Q_ASSERT_X(
        s_conns.contains(id), Q_FUNC_INFO, QStringLiteral("Connection not found id: %1").arg(id).toUtf8().constData()
    );
    s_conns.take(id).disconnect();
}

void Settings::disconnectReadValuesFromKey(const QString& key)
{
    auto data_it = instance().findRecord(key);
    for (auto& conn : std::as_const(data_it->conns))
    {
        Q_ASSERT_X(
            s_conns.contains(conn),
            Q_FUNC_INFO,
            QStringLiteral("Connection not found id: %1").arg(conn).toUtf8().constData()
        );
        s_conns.remove(conn);
    }
    data_it->conns.clear();
}

void Settings::disconnectReadValuesFromGroup(const QString& dir)
{
    auto group_it = instance().findRegGroup(dir);
    for (auto& data : std::as_const(group_it->dataset))
    {
        for (auto& conn : std::as_const(data.conns))
        {
            Q_ASSERT_X(
                s_conns.contains(conn),
                Q_FUNC_INFO,
                QStringLiteral("Connection not found id: %1").arg(conn).toUtf8().constData()
            );
            s_conns.remove(conn);
        }
        data.conns.clear();
    }
}

void Settings::disconnectAllSettingsReadValues()
{
    for (auto it = s_conns.begin(); it != s_conns.end(); it = s_conns.erase(it))
    {
        it.value().disconnect();
    }
}

void Settings::emitReadValue(ConnId id)
{
    Q_ASSERT_X(
        s_conns.contains(id), Q_FUNC_INFO, QStringLiteral("Connection not found id: %1").arg(id).toUtf8().constData()
    );
    s_conns[id].read();
}

void Settings::emitReadValuesFromKey(const QString& key)
{
    auto data_it = instance().findRecord(key);
    for (auto& conn : std::as_const(data_it->conns))
    {
        emitReadValue(conn);
    }
}

void Settings::emitReadValuesFromGroup(const QString& dir)
{
    readValueFromGroup(&(instance().findRegGroup(dir).value()));
}

void Settings::emitAllSettingsReadValues()
{
    for (auto& conn : std::as_const(s_conns))
    {
        conn.read();
    }
}

// 主类的辅助函数的实现
/* ========================================================================== */

Settings::RegGroup::dataset_iterator Settings::findRecord(const QString& key)
{
    auto it = m_regedit.findData(key);
    Q_ASSERT_X(
        it != m_regedit.dataEnd(),
        Q_FUNC_INFO,
        QStringLiteral("Setting registration `record` not found: %1").arg(key).toUtf8().constData()
    );
    return it;
}

Settings::RegGroup::groupset_iterator Settings::findRegGroup(const QString& dir)
{
    auto it = m_regedit.findGroup(dir);
    Q_ASSERT_X(
        it != m_regedit.groupEnd(),
        Q_FUNC_INFO,
        QStringLiteral("Setting registration `group` not found: %1").arg(dir).toUtf8().constData()
    );
    return it;
}

QVariant Settings::getValue(const QString& key)
{
    auto it = findRecord(key);
    if (auto value = m_q_settings.value(key, it->default_value); it->check(value))
    {
        return value;
    }
    // 重置非法值
    m_q_settings.setValue(key, it->default_value);
    return it->default_value;
}

// 主类静态辅助函数的实现
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

QMap<Settings::ConnId, Settings::ConnFunctions> Settings::s_conns = {};

/* ========================================================================== */

void Settings::readValueFromGroup(const RegGroup* group)
{
    // 读取数据
    for (auto& data : std::as_const(group->dataset))
    {
        for (auto& conn : std::as_const(data.conns))
        {
            emitReadValue(conn);
        }
    }
    // 递归读取子组
    for (auto& subGroup : std::as_const(group->groupset))
    {
        readValueFromGroup(&subGroup);
    }
}
