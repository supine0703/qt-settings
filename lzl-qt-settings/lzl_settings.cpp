/**
 * License: GPLv3 LGPLv3
 * Copyright (c) 2024-2025 李宗霖 (Li Zonglin)
 * Email: supine0703@outlook.com
 * GitHub: https://github.com/supine0703
 * Repository: https://github.com/supine0703/qt-settings
 */

#include "lzl_settings.h"

#include <QDir>
#include <QMutex>
#include <QRegularExpression>

#ifndef CONFIG_INI
    #define CONFIG_INI "config.ini"
#endif

namespace lzl::utils {

namespace {
const auto _connIdMetaTypeId = qRegisterMetaType<Settings::ConnId>("lzl::utils::Settings::ConnId");
} // namespace

// 实例构造
/* ========================================================================== */

Settings* Settings::s_instance = nullptr;
QString Settings::s_ini_directory = {};
QString Settings::s_ini_fileName = {};

Settings& Settings::instance()
{
    if (s_instance == nullptr)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (s_instance == nullptr)
        {
            auto ini_path =
                !s_ini_fileName.isEmpty()
                    ? QDir(s_ini_directory).filePath(s_ini_fileName)
                    : (QDir::isRelativePath(CONFIG_INI) ? QDir(s_ini_directory).filePath(CONFIG_INI) : CONFIG_INI);
            s_instance = new Settings(ini_path);
        }
    }
    return *s_instance;
}

Settings::Settings(const QString& filename, QObject* parent) : m_q_settings(filename, QSettings::IniFormat, parent) {}

void Settings::InitIniDirectory(const QString& directory) noexcept
{
    Q_ASSERT_X(
        s_instance == nullptr,
        Q_FUNC_INFO,
        QStringLiteral("The function must be called before 'Settings' initialization.").toUtf8().constData()
    );
    s_ini_directory = directory;
}

void Settings::InitIniFilePath(const QString& filePath)
{
    Q_ASSERT_X(
        s_instance == nullptr,
        Q_FUNC_INFO,
        QStringLiteral("The function must be called before 'Settings' initialization.").toUtf8().constData()
    );
    QFileInfo fileinfo(filePath);
    Q_ASSERT_X(
        fileinfo.suffix() == "ini", Q_FUNC_INFO, QStringLiteral("The file suffix must be 'ini'.").toUtf8().constData()
    );
    s_ini_fileName = fileinfo.fileName();
    s_ini_directory = fileinfo.path();
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

void Settings::RegData::clearConns() const
{
    // 从全局表中删除
    for (auto& conn : std::as_const(conns))
    {
        Q_ASSERT_X(
            s_conns.contains(conn),
            Q_FUNC_INFO,
            QStringLiteral("Connection not found id: %1").arg(static_cast<quintptr>(conn)).toUtf8().constData()
        );
        s_conns.remove(conn);
    }
    // 从自己中删除
    this->conns.clear();
}

Settings::RegGroup::dataset_iterator Settings::RegGroup::findData(const QString& key)
{
    auto [dir, name] = parsePath(key);

    // 组为空（全局组），如果 name 也为空会返回 dataEnd();
    if (dir.isEmpty())
    {
        return dataset.find({name});
    }
    // 否则从组中查找
    else if (auto group_it = findGroup(dir); group_it != groupEnd())
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
    auto group_it = groupEnd();
    // 从根节点开始查找
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
    group->dataset.insert(name, {default_value, std::move(check_func)});
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
    // 如果 path 为空，返回空列表
    static const QRegularExpression re(QStringLiteral(R"([/\\])"));
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    return path.split(re, Qt::SkipEmptyParts);
#else
    return path.split(re, QString::SkipEmptyParts);
#endif
}

QPair<QStringList, QString> Settings::RegGroup::parsePath(const QString& path)
{
    // 如果 path 为空，返回空列表和空字符串
    auto dir = detachPath(path);
    auto name = dir.isEmpty() ? QString() : dir.takeLast();
    return {dir, name};
}

// 主类的静态（对外接口）函数实现
/* ========================================================================== */

bool Settings::writeValue(const QString& key, const QVariant& value, bool emit_signal)
{
    Q_ASSERT(!key.isEmpty());

    // 如果数据符合检查
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
        s_conns.contains(id),
        Q_FUNC_INFO,
        QStringLiteral("Connection not found id: %1").arg(static_cast<quintptr>(id)).toUtf8().constData()
    );
    s_conns.take(id).disconnect();
}

void Settings::disconnectReadValuesFromKey(const QString& key)
{
    Q_ASSERT(!key.isEmpty());
    instance().findRecord(key)->clearConns();
}

void Settings::disconnectReadValuesFromGroup(const QString& dir)
{
    Q_ASSERT(!dir.isEmpty());

    auto group_it = instance().findRegGroup(dir);
    for (auto& data : std::as_const(group_it->dataset))
    {
        data.clearConns();
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
        s_conns.contains(id),
        Q_FUNC_INFO,
        QStringLiteral("Connection not found id: %1").arg(static_cast<quintptr>(id)).toUtf8().constData()
    );
    s_conns[id].read();
}

void Settings::emitReadValues(const QList<ConnId>& ids)
{
    for (auto& id : std::as_const(ids))
    {
        emitReadValue(id);
    }
}

void Settings::emitReadValuesFromKey(const QString& key)
{
    emitReadValues(getConnIdsFromKey(key));
}

void Settings::emitReadValuesFromGroup(const QString& dir)
{
    emitReadValues(getConnIdsFromGroup(dir));
}

void Settings::emitAllSettingsReadValues()
{
    for (auto& conn : std::as_const(s_conns))
    {
        conn.read();
    }
}

QList<Settings::ConnId> Settings::getConnIdsFromKey(const QString& key)
{
    Q_ASSERT(!key.isEmpty());
    return instance().findRecord(key)->conns;
}

QList<Settings::ConnId> Settings::getConnIdsFromGroup(const QString& dir)
{
    Q_ASSERT(!dir.isEmpty());
    QList<ConnId> conns;
    getConnIdsFromGroup(&(instance().findRegGroup(dir).value()), conns);
    std::sort(conns.begin(), conns.end());
    return conns;
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

QMap<Settings::ConnId, Settings::ConnFunctions> Settings::s_conns = {};

Settings::ConnId Settings::generateId()
{
    static ConnId id;
    do
    {
        ++id;
    } while (id.isNull() || s_conns.contains(id));
    return id; // 理论上不会轮回到 0
}

Settings::ConnId Settings::insertConn(const RegData* data, std::function<void(void)>&& read_func)
{
    auto id = generateId();
    data->conns.append(id);
    s_conns[id] = {
        std::move(read_func),
        [data, id]() { data->conns.removeOne(id); },
    };
    return id;
}

/* ========================================================================== */

void Settings::getConnIdsFromGroup(const RegGroup* group, QList<ConnId>& conns)
{
    // 读取数据
    for (auto& data : std::as_const(group->dataset))
    {
        for (auto& conn : std::as_const(data.conns))
        {
            conns.append(conn);
        }
    }
    // 递归读取子组
    for (auto& subGroup : std::as_const(group->groupset))
    {
        getConnIdsFromGroup(&subGroup, conns);
    }
}

} // namespace lzl::utils
