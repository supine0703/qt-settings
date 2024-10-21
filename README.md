
正常情况下，我们对软件进行的修改是不会保存的，这时便需要配置文件。
常用的配置文件有两种：

- `.ini`: 
  - 优点：简单、轻量，适合存储简单的键值对配置。读取和写入速度快，易于实现。
  - 缺点：不适合存储复杂的数据结构，如嵌套数据 (支持一层分级) 或数组。
  - 可以写入注册表
- `.json`: 
  - 优点：灵活、易读，适合存储复杂数据结构，如嵌套对象和数组。广泛支持，便于与其他系统和应用程序集成。
  - 缺点：比 INI 文件稍微复杂，解析和写入速度可能略慢。


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

- 缺点：
  - 

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

###
默认值
检查函数 std::function<bool(T)>
无效处理类型
自定义无效处理 std::function<void(void)>
