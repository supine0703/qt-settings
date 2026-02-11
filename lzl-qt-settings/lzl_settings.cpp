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
const auto _g_connIdMetaTypeId = qRegisterMetaType<Settings::ConnId>("lzl::utils::Settings::ConnId");
} // namespace

// 实例构造
/* ========================================================================== */

Settings* Settings::s_instance = nullptr;
QString Settings::s_ini_directory = {};
QString Settings::s_ini_file_name = {};

Settings& Settings::instance()
{
    if (s_instance == nullptr)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (s_instance == nullptr)
        {
            s_instance = new Settings([] {
                if (!s_ini_file_name.isEmpty())
                {
                    return QDir(s_ini_directory).filePath(s_ini_file_name);
                }
                if (QDir::isRelativePath(CONFIG_INI))
                {
                    return QDir(s_ini_directory).filePath(QStringLiteral(CONFIG_INI));
                }
                return QStringLiteral(CONFIG_INI);
            }());
        }
    }
    return *s_instance;
}

Settings::Settings(const QString& file_name, QObject* parent) : m_q_settings(file_name, QSettings::IniFormat, parent) {}

void Settings::InitIniDirectory(const QString& directory) noexcept
{
    Q_ASSERT_X(
        s_instance == nullptr,
        Q_FUNC_INFO,
        QStringLiteral("The function must be called before 'Settings' initialization.").toUtf8().constData()
    );
    s_ini_directory = directory;
}

void Settings::InitIniFilePath(const QString& file_path)
{
    Q_ASSERT_X(
        s_instance == nullptr,
        Q_FUNC_INFO,
        QStringLiteral("The function must be called before 'Settings' initialization.").toUtf8().constData()
    );
    QFileInfo fileinfo(file_path);
    Q_ASSERT_X(
        fileinfo.suffix() == "ini", Q_FUNC_INFO, QStringLiteral("The file suffix must be 'ini'.").toUtf8().constData()
    );
    s_ini_file_name = fileinfo.fileName();
    s_ini_directory = fileinfo.path();
}

// 注册表相关类的成员函数
/* ========================================================================== */

Settings::RegData::~RegData()
{
    clearConns();
}

void Settings::RegData::clearConns() const
{
    // 从全局表中删除
    for (const auto id : conn_ids)
    {
        Q_ASSERT_X(
            s_conns.contains(id),
            Q_FUNC_INFO,
            QStringLiteral("Connection not found id: %1").arg(static_cast<std::size_t>(id)).toUtf8().constData()
        );
        s_conns.remove(id);
    }
    // 从自己中删除
    conn_ids.clear();
}

Settings::RegData* Settings::RegGroup::findData(const QString& key)
{
    auto [words, name] = parsePath(key);

    // 组为空（全局组），如果 name 也为空会返回 dataEnd();
    if (words.isEmpty())
    {
        if (auto data_it = dataset.find({name}); data_it != dataset.end())
        {
            return &(data_it.value());
        }
    }
    // 否则从组中查找
    else if (auto group = findGroup(words); group != nullptr)
    {
        if (auto data_it = group->dataset.find({name}); data_it != group->dataset.end())
        {
            return &(data_it.value());
        }
    }
    return nullptr;
}

Settings::RegGroup* Settings::RegGroup::findGroup(const QString& dir)
{
    return findGroup(detachPath(dir));
}

Settings::RegGroup* Settings::RegGroup::findGroup(const QStringList& words)
{
    // 从根节点开始查找
    auto group = this;
    for (const auto& word : words)
    {
        auto group_it = group->groupset.find(word);
        if (group_it == group->groupset.end())
        {
            return nullptr;
        }
        group = &group_it.value();
    }
    return group; // group 是最后一个有效节点
}

void Settings::RegGroup::insertData(const QString& key, const QVariant& default_value, CheckFunction check_func)
{
    Q_ASSERT(!key.isEmpty());

    // 查找组
    auto [words, name] = parsePath(key);
    auto group = this;
    for (const auto& word : std::as_const(words))
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
    QList<RegGroup*> groups = {this};
    auto [words, name] = parsePath(key);
    for (const auto& word : std::as_const(words))
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
        Q_ASSERT(words.size() == groups.size());
        group = groups.takeLast();
        group->groupset.remove(words.takeLast());
    }
}

void Settings::RegGroup::removeGroup(const QString& dir)
{
    Q_ASSERT(!dir.isEmpty());

    // 查找组
    QList<RegGroup*> groups = {this};
    auto [pre_words, group_name] = parsePath(dir);
    for (const auto& word : std::as_const(pre_words))
    {
        groups.append(&groups.last()->groupset[word]);
    }

    auto group = groups.takeLast();
    Q_ASSERT_X(
        group->groupset.contains(group_name),
        Q_FUNC_INFO,
        QStringLiteral("Setting registration `group` not found: %1").arg(dir).toUtf8().constData()
    );

    // 删除组
    group->groupset.remove(group_name);

    // 清除空节点
    while (!groups.isEmpty() && group->dataset.isEmpty() && group->groupset.isEmpty())
    {
        group = groups.takeLast();
        group->groupset.remove(pre_words.takeLast());
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

Settings::RegGroup::ParsedPathPair Settings::RegGroup::parsePath(const QString& path)
{
    // 如果 path 为空，返回空列表和空字符串
    auto words = detachPath(path);
    auto name = words.isEmpty() ? QString() : words.takeLast();
    return {words, name};
}

// 主类的静态（对外接口）函数实现
/* ========================================================================== */

bool Settings::writeValue(const QString& key, const QVariant& value, bool emit_signal)
{
    Q_ASSERT(!key.isEmpty());

    // 如果数据符合检查
    if (instance().findRecord(key)->check_func(value))
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
        QStringLiteral("Connection not found id: %1").arg(static_cast<std::size_t>(id)).toUtf8().constData()
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

    auto group = instance().findRegGroup(dir);
    for (const auto& data : std::as_const(group->dataset))
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
        QStringLiteral("Connection not found id: %1").arg(static_cast<std::size_t>(id)).toUtf8().constData()
    );
    s_conns[id].read();
}

void Settings::emitReadValues(const QList<ConnId>& ids)
{
    for (const auto id : ids)
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
    for (const auto& conn_func : std::as_const(s_conns))
    {
        conn_func.read();
    }
}

QList<Settings::ConnId> Settings::getConnIdsFromKey(const QString& key)
{
    Q_ASSERT(!key.isEmpty());
    // 在 findRecord 中 Q_ASSERT_X 会确保 record 不为空
    return instance().findRecord(key)->conn_ids;
}

QList<Settings::ConnId> Settings::getConnIdsFromGroup(const QString& dir)
{
    Q_ASSERT(!dir.isEmpty());
    QList<ConnId> conn_ids;
    // 在 findRegGroup 中 Q_ASSERT_X 会确保 group 不为空
    getConnIdsFromGroup(instance().findRegGroup(dir), conn_ids);
    std::sort(conn_ids.begin(), conn_ids.end());
    return conn_ids;
}

// 主类的辅助函数的实现
/* ========================================================================== */

Settings::RegData* Settings::findRecord(const QString& key)
{
    auto data = m_regedit.findData(key);
    Q_ASSERT_X(
        data != nullptr,
        Q_FUNC_INFO,
        QStringLiteral("Setting registration `record` not found: %1").arg(key).toUtf8().constData()
    );
    return data;
}

Settings::RegGroup* Settings::findRegGroup(const QString& dir)
{
    auto group = m_regedit.findGroup(dir);
    Q_ASSERT_X(
        group != nullptr,
        Q_FUNC_INFO,
        QStringLiteral("Setting registration `group` not found: %1").arg(dir).toUtf8().constData()
    );
    return group;
}

QVariant Settings::getValue(const QString& key)
{
    auto record = findRecord(key);
    if (auto value = m_q_settings.value(key, record->default_value); record->check_func(value))
    {
        return value;
    }
    // 重置非法值
    m_q_settings.setValue(key, record->default_value);
    return record->default_value;
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
    data->conn_ids.append(id);
    s_conns[id] = {
        std::move(read_func),
        [data, id]() { data->conn_ids.removeOne(id); },
    };
    return id;
}

/* ========================================================================== */

void Settings::getConnIdsFromGroup(const RegGroup* group, QList<ConnId>& conn_ids)
{
    // 读取数据
    for (const auto& data : std::as_const(group->dataset))
    {
        for (const auto conn_id : data.conn_ids)
        {
            conn_ids.append(conn_id);
        }
    }
    // 递归读取子组
    for (const auto& sub_group : std::as_const(group->groupset))
    {
        getConnIdsFromGroup(&sub_group, conn_ids);
    }
}

} // namespace lzl::utils
