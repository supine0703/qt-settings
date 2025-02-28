> License: MIT LGPL-2.1  
> Copyright (c) 2024 李宗霖 (Li Zonglin)  
> Email: supine0703@outlook.com  
> GitHub: https://github.com/supine0703  


# lzl-qt-settings <!-- omit in toc -->

注意：0.3.x 版本完全重构，不兼容之前的版本

## 目录 <!-- omit in toc -->
- [简单介绍](#简单介绍)
- [使用方法](#使用方法)
  - [使用示例](#使用示例)
    - [注册设置](#注册设置)
    - [注销设置](#注销设置)
    - [绑定读取事件](#绑定读取事件)
    - [解除绑定读取事件](#解除绑定读取事件)
    - [读取或触发读取事件](#读取或触发读取事件)
    - [写入（可选：并触发读取）](#写入可选并触发读取)
- [报告问题](#报告问题)
- [与我联系](#与我联系)

## 简单介绍

`QSettings` 是 `Qt` 对 `.ini` 和 `RegEdit` 的简单封装，操作较为简单底层，也意味着对于很多操作是**不方便的**、**难以维护的**，因此在我大量使用的过程中封装了这一套基于 `QSettings` ，利用类似信号槽机制（观察者模式）封装了一套库

## 使用方法

详细可见 [./lzl_settings.h](./lzl_settings.h)

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
     * @param key 要注册的键
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
    static void deRegisterSettingKey(const QString& key);

    /**
     * @brief deRegisterSettingGroup 注销设置
     * @param dir 存在的组
     */
    static void deRegisterSettingGroup(const QString& dir);

    /**
     * @brief deRegisterAllSettings 注销所有设置
     */
    static void deRegisterAllSettings();

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
     * @param read_func 对象成员函数读取事件的回调函数
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
    static QList<ConnId> getConnIds();
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
lzl::Settings::connectReadValue("app/window/pos", [this](QPoint pos) { this->move(pos); });
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

## 报告问题

[你可以直接点击这里创建一个问题](https://github.com/supine0703/qt-settings/issues/new)

## 与我联系

如果有其他问题可以与我取得联系  
Email: supine0703@outlook.com
