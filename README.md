
# lzl-qt-settings <!-- omit in toc -->

注意：0.3.x 版本完全重构，不兼容之前的版本

## 目录 <!-- omit in toc -->
- [简单介绍](#简单介绍)
- [开发环境](#开发环境)
- [测试环境](#测试环境)
- [编译](#编译)
  - [windows](#windows)
  - [linux](#linux)
- [使用方法](#使用方法)
  - [使用示例](#使用示例)
    - [注册设置](#注册设置)
    - [注销设置](#注销设置)
    - [绑定读取事件](#绑定读取事件)
    - [解除绑定读取事件](#解除绑定读取事件)
    - [读取或触发读取事件](#读取或触发读取事件)
    - [写入（可选：并触发读取）](#写入可选并触发读取)
- [关于配置文件](#关于配置文件)
- [关于设置的一些写法](#关于设置的一些写法)
  - [最低级的写法-直接开干](#最低级的写法-直接开干)
  - [稍好一点-单例+宏](#稍好一点-单例宏)
  - [最终实现-类型萃取+观察者模式](#最终实现-类型萃取观察者模式)
- [TODO](#todo)
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

如何加入自己的项目可以参看示例 [./CMakeLists.txt](./CMakeLists.txt)

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
 * @version 0.3.x
 * @note 0.3 之后弃用 GroupId 而是直接使用路径作为分组（分节）
 * @note 注意变量名的含义：
 *   - @param key 设置的键，是完整路径，如：app/font/size
 *   - @param dir 组的路径，是完整路径，如：app/font
 *   - @param path 键或组的路径，如：app/font/size、app/font
 *   - @param word 路径的一部分，如：app、font、size
 *   - @param name 路径的最后一部分，如：size、font
 */
class LZL_QT_SETTINGS_EXPORT Settings final
{
public:
    /**
     * @brief ConnId 绑定读取事件的 id 类
     */
    class ConnId final { /* ... */ };

    /**
     * @brief sync 同步设置
     */
    static void sync();

    /**
     * @brief reset 清空设置文件
     */
    static void reset();

    /**
     * @brief reset 清空设置
     * @param path 键或组的路径
     */
    static void reset(const QString& path);

    /**
     * @brief containsKey 是否注册过设置
     * @param key 键的值
     * @return 是否注册过
     */
    static bool containsKey(const QString& key);

    /**
     * @brief containsGroup 是否存在组
     * @param dir 组的路径
     * @return 是否存在组
     */
    static bool containsGroup(const QString& dir);

    /**
     * @brief registerSetting 注册设置
     * @param key 要注册的键，不可为空
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
     * @param key 要注册的键，不可为空
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
     * @param key 注册过的键，不可为空
     */
    static void deRegisterSettingKey(const QString& key);

    /**
     * @brief deRegisterSettingGroup 注销设置
     * @param dir 存在的组，不可为空
     */
    static void deRegisterSettingGroup(const QString& dir);

    /**
     * @brief deRegisterAllSettings 注销所有设置
     */
    static void deRegisterAllSettings();

    /**
     * @brief writeValue 写入设置
     * @param key 注册过的键，不可为空
     * @param value 设置的值
     * @param emit_signal 是否触发读取事件信号
     * @return 是否写入成功
     */
    static bool writeValue(const QString& key, const QVariant& value, bool emit_signal = false);

    /**
     * @brief readValue 读取设置
     * @param key 注册过的键，不可为空
     * @param read_func 读取设置的回调函数
     */
    template <typename Func>
    static void readValue(const QString& key, Func read_func);

    /**
     * @brief readValue 读取设置
     * @param key 注册过的键，不可为空
     * @param object 对象
     * @param read_func 对象成员函数读取设置的回调函数
     */
    template <typename Func>
    static void readValue(const QString& key, trains_class_type<Func>* object, Func read_func);

    /**
     * @brief connectReadValue 绑定读取事件
     * @param key 注册过的键，不可为空
     * @param read_func 读取设置的回调函数
     * @return 读取事件的 id
     */
    template <typename Func, typename = std::enable_if_t<!std::is_member_function_pointer<Func>::value>>
    static ConnId connectReadValue(const QString& key, Func read_func);

    /**
     * @brief connectReadValue 绑定读取事件
     * @param key 注册过的键，不可为空
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
     * @param key 注册过的键，不可为空
     */
    static void disconnectReadValuesFromKey(const QString& key);

    /**
     * @brief disconnectReadValuesFromGroup 解绑读取事件
     * @param dir 存在的组，不可为空
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
     * @brief emitReadValues 触发读取事件信号
     * @param ids 读取事件的 id 列表, Q_ASSERT(!id.isNull());
     */
    static void emitReadValues(const QList<ConnId>& ids);

    /**
     * @brief emitReadValuesFromKey 触发读取事件信号
     * @param key 注册过的键，不可为空
     */
    static void emitReadValuesFromKey(const QString& key);

    /**
     * @brief emitReadValuesFromGroup 触发读取事件信号
     * @param dir 存在的组，不可为空
     */
    static void emitReadValuesFromGroup(const QString& dir);

    /**
     * @brief emitAllSettingsReadValues 触发所有读取事件信号
     */
    static void emitAllSettingsReadValues();

    /**
     * @brief getConnIds 获取所有的读取事件 id 列表
     * @return id 列表, Q_ASSERT(!id.isNull());
     */
    static QList<ConnId> getConnIds();

    /**
     * @brief getConnIdsFromKey 获取键的读取事件 id 列表
     * @param key 注册过的键，不可为空
     * @return id 列表, Q_ASSERT(!id.isNull());
     */
    static QList<ConnId> getConnIdsFromKey(const QString& key);

    /**
     * @brief getConnIdsFromGroup 获取组的读取事件 id 列表
     * @param dir 存在的组，不可为空
     * @return id 列表, Q_ASSERT(!id.isNull());
     */
    static QList<ConnId> getConnIdsFromGroup(const QString& dir);
};
```

### 使用示例

可见 demo: [mainwindow.cpp](./mainwindow.cpp)  
其中搜索宏 `USE_LZL_QT_SETTINGS` 可以查看不封装和封装使用的差异

#### 注册设置

```cpp
auto screen_rect = QApplication::primaryScreen()->availableGeometry();
auto font = QApplication::font();
// 注册设置
lzl::Settings::registerSetting("app/font/size", font.pointSizeF() * 1.728, [](const QVariant& value) {
    return value.canConvert<double>() && 4 < value.toDouble() && value.toDouble() < 48;
});
lzl::Settings::registerSetting("app/window/size", this->size(), [screen_rect](const QVariant& value) {
    if (value.canConvert<QSize>())
    {
        auto size = value.toSize();
        return size.width() > 400 && size.height() > 300 && size.width() < screen_rect.width()
                && size.height() < screen_rect.height();
    }
    return false;
});
lzl::Settings::registerSetting("app/window/pos", this->pos(), [screen_rect](const QVariant& value) {
    if (value.canConvert<QPoint>())
    {
        auto pos = value.toPoint();
        return screen_rect.contains(pos);
    }
    return false;
});
```

#### 注销设置

```cpp
// 注销所有设置（可以没有注册过）
lzl::Settings::deRegisterAllSettings();
// 注销指定 key 的设置（会断言注册过）
lzl::Settings::deRegisterSettingKey("app/font/size");
// 注销指定 dir 的设置（会断言注册过）
lzl::Settings::deRegisterSettingGroup("app/font");
```

#### 绑定读取事件

```cpp
// 绑定设置事件
lzl::Settings::connectReadValue("app/font/size", [button1](double size) {
    auto font = QApplication::font();
    font.setPointSizeF(size);
    QApplication::setFont(font);
    button1->setFont(font);
});
lzl::Settings::connectReadValue("app/window/size", [this](QSize size) { this->resize(size); });
lzl::Settings::connectReadValue("app/window/pos", this, &MainWindow::move); // 这里不要有多参数重载，否则 lambda 是更好的选择
```

#### 解除绑定读取事件

```cpp
// 解绑所有读取设置事件
lzl::Settings::disconnectAllSettingsReadValues();
// 解绑指定 id 的读取设置事件（会断言存在）
lzl::Settings::disconnectReadValue(id1);
// 解绑指定 key 的读取设置事件（可以没有绑定过）
lzl::Settings::disconnectReadValuesFromKey("app/font/size");
// 解绑指定 dir 的读取设置事件（可以没有绑定过）
lzl::Settings::disconnectReadValuesFromGroup("app/font");
```

#### 读取或触发读取事件

```cpp
// 读取设置（会断言注册过）
lzl::Settings::readValue("app/font/size", [button1](double size) {
    auto font = QApplication::font();
    font.setPointSizeF(size);
    QApplication::setFont(font);
    button1->setFont(font);
});
// 触发所有读取事件
lzl::Settings::emitAllSettingsReadValues();
// 触发指定 id 的读取事件（会断言存在）
lzl::Settings::emitReadValue(id1);
// 触发指定 key 的读取事件（可以没有绑定过）
lzl::Settings::emitReadValuesFromKey("app/font/size");
// 触发指定 dir 的读取事件（可以没有绑定过）
lzl::Settings::emitReadValuesFromGroup("app/font");
```

#### 写入（可选：并触发读取）

```cpp
// 写入设置（会断言注册过）
lzl::Settings::writeValue("app/font/size", 12.0);
// 写入设置并触发读取事件
lzl::Settings::writeValue("app/font/size", 12.0, true);
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

## TODO

1. [x] 整组读取有序
2. [x] 添加对全局键操作的支持（全局组不支持间接操作，因为全局组是所有内容，所以可以直接操作带有 `All` 的函数）
3. [x] 添加返回 `path` 的 `ConnId`
4. [ ] 添加 `.json` 支持

## 报告问题

[你可以直接点击这里创建一个问题](https://github.com/supine0703/qt-settings/issues/new)

## 与我联系

如果有其他问题可以与我取得联系  
Email: supine0703@outlook.com
