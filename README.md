
# lzl-qt-settings <!-- omit in toc -->

## 目录 <!-- omit in toc -->
- [简单介绍](#简单介绍)
- [开发环境](#开发环境)
- [测试环境](#测试环境)
- [编译](#编译)
  - [windows](#windows)
  - [linux](#linux)
- [使用方法](#使用方法)
  - [使用示例](#使用示例)
- [关于配置文件](#关于配置文件)
- [关于设置的一些写法](#关于设置的一些写法)
  - [最低级的写法-直接开干](#最低级的写法-直接开干)
  - [稍好一点-单例+宏](#稍好一点-单例宏)
  - [最终实现-类型萃取+观察者模式](#最终实现-类型萃取观察者模式)
- [报告问题](#报告问题)
- [与我联系](#与我联系)

## 简单介绍

`QSettings` 是 `Qt` 对 `.ini` 和 `RegEdit` 的简单封装，操作较为简单底层，也意味着对于很多操作是**不方便的**、**难以维护的**，因此在我大量使用的过程中封装了这一套基于 `QSettings` ，利用类似信号槽机制（观察者模式）封装了一套库

## 开发环境

- Windows 11 24H2
  - Qt 5.15.2
  - Qt 6.8.2

## 测试环境

- Windows 11 24H2
  - Qt 5.15.2
  - Qt 6.8.2
- Ubuntu 20.04.6 LTS
  - Qt 5.12.8

## 编译

### windows

直接使用 `QtCreator` 进行编译，可修修改 `CMakeLists.txt` 的选项

### linux

```sh
# 检查 (并创建) 并进入 build文件夹
mkdir -p build && cd build

# 配置 CMakeCache
cmake -S$src_path -G "$GENERATOR" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
    -DCMAKE_BUILD_TYPE:STRING=Release \
    --no-warn-unused-cli

# 编译
cmake --build . --config Release --target all
```

## 使用方法

详细可见 [./lzl-qt-settings/lzl_settings.h](./lzl-qt-settings/lzl_settings.h)

```cpp
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
```

### 使用示例

可见 demo: [mainwindow.cpp](./mainwindow.cpp)  
其中搜索宏 `USE_LZL_QT_SETTINGS` 可以查看不封装和封装使用的差异

```cpp
// 注册设置
lzl::Settings::registerSetting(
    "app/font/size",
    font.pointSizeF() * 1.728,
    [](const QVariant& value) {
        return value.canConvert<double>() && 4 < value.toDouble() && value.toDouble() < 64;
    },
    1
);
lzl::Settings::registerSetting(
    "app/size", this->size(), [](const QVariant& value) { return value.canConvert<QSize>(); }, 1
);
lzl::Settings::registerSetting(
    "app/pos", this->pos(), [](const QVariant& value) { return value.canConvert<QPoint>(); }, 1
);
// 绑定设置事件
lzl::Settings::connectReadValue(
    "app/font/size",
    [button1](double size) {
        auto font = QApplication::font();
        font.setPointSizeF(size);
        QApplication::setFont(font);
        button1->setFont(font);
    },
    2
);
lzl::Settings::connectReadValue("app/size", [this](QSize size) { this->resize(size); }, 2);
lzl::Settings::connectReadValue("app/pos", [this](QPoint pos) { this->move(pos); }, 2);
// 读取设置
lzl::Settings::emitReadValues(2);
// ...
// 其他位置读取
lzl::Settings::writeValue("app/font/size", font.pointSizeF(), true);
lzl::Settings::writeValue("app/size", font.pointSizeF(), false);
```

## 关于配置文件

正常情况下，我们对软件进行的修改是不会保存的，这时便需要配置文件。
常用的配置文件有两种：

- `.ini`: 
  - 优点：简单、轻量，适合存储简单的键值对配置。读取和写入速度快，易于实现。
  - 缺点：不适合存储复杂的数据结构，如嵌套数据 (支持一层分级) 或数组。
  - 可以写入注册表
- `.json`: 
  - 优点：灵活、易读，适合存储复杂数据结构，如嵌套对象和数组。广泛支持，便于与其他系统和应用程序集成。
  - 缺点：比 INI 文件稍微复杂，解析和写入速度可能略慢。

## 关于设置的一些写法

### 最低级的写法-直接开干

**本质上是面向过程**

- 优点：
  - 简单、速成
- 缺点：
  - 没有设计感、容易出错、极难维护
  - 较难实现稍微复杂的逻辑，越复杂代码越冗余臃肿
  - ... ...
- 例如：
  - 每次需要创建一模一样的实例 (可以用单例解决)
  - 读写多次且多处出现字符串，可能会不小心出错 (可以用宏定义解决)
  - 对于值类型、值范围的检查？将会有大量冗余代码，特别是if-else (可以封装类，用封装的成员函数解决)

读取的例子
```cpp
QSettings settings(CONFIG_INI, QSettings::IniFormat);
auto font = QApplication::font();
if (settings.contains("app/font/size")) {
    font.setPointSizeF(settings.value("app/font/size").toDouble());
} else {
    font.setPointSizeF(font.pointSizeF() * 1.728); // 1.2 * 1.2 * 1.2
}
QApplication::setFont(font);
button->setFont(font);

if (settings.contains("app/size")) {
    this->resize(settings.value("app/size").toSize());
}
```

写入的例子
```cpp
void MainWindow::moveEvent(QMoveEvent* event) {
    QWidget::moveEvent(event);
    QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/pos", this->pos());
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/size", this->size());
}
// ... ...
```


### 稍好一点-单例+宏

- 优点：
  - 模块化封装、使用单例模式封装
  - 可以设置默认值和检查值的合法性
  - 操作更简便，使用类似变量赋值的方式
- 缺点：
  - 对于大量重复的操作同样不简单
  - 功能相对较少

可以看我之前的项目 [ [OS_QASys/config.h](https://github.com/supine0703/OS_QASys/blob/main/src/utils/config.h), [OS_QASys/config.cpp](https://github.com/supine0703/OS_QASys/blob/main/src/utils/config.cpp) ]

简单的使用例子
```cpp
#define _DOCK_GEOMETRY_ "state/dock_geometry"
#define _DOCK_TRACKING_ "state/dock_tracking"

// 注册
CFG().reg({
    { _DOCK_GEOMETRY_, { QRect(600, 400, 300, 300) } },
    { _DOCK_TRACKING_, { false, [](auto& v){return v.isValid();} } },
}); // 传入 { key, { value, check_fn } } ; check_fn 检查值是被允许
// 读取 
recdDock->setGeometry(CFG()[_DOCK_GEOMETRY_].toRect());
// 写入
CFG()[_DOCK_TRACKING_] = view_trace_all[2]->isChecked();
```

### 最终实现-类型萃取+观察者模式

查看 [使用示例](#使用示例)

## 报告问题

[你可以直接点击这里创建一个问题](https://github.com/supine0703/qt-settings/issues/new)

## 与我联系

如果有其他问题可以与我取得联系  
Email: supine0703@outlook.com
