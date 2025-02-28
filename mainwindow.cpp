/**
 * License: LGPL-2.1
 * Copyright (c) 2024-2025 李宗霖 (Li Zonglin)
 * Email: supine0703@outlook.com
 * GitHub: https://github.com/supine0703
 * Repository: qt-settings <https://github.com/supine0703/qt-settings>
 */

#include "mainwindow.h"

#include "lzl/settings"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QPushButton>
#include <QScreen>
#include <QSettings>
#include <QTimer>

// 可以通过搜索这个宏查看如果不封装和封装后的区别
#define USE_LZL_QT_SETTINGS 1

#if USE_LZL_QT_SETTINGS
namespace lzl::utils {
template <>
struct ConvertQVariant<const QSize&>
{
    static auto convert(const QVariant& value) { return value.toSize(); }
};
template <>
struct ConvertQVariant<const QPoint&>
{
    static auto convert(const QVariant& value) { return value.toPoint(); }
};
} // namespace lzl::utils
#endif // 补充需要转换的类型

static lzl::Settings::ConnId id1, id2, id3;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plainTextEdit_1->document()->setDefaultTextOption(QTextOption(Qt::AlignRight));  // 右对齐
    ui->plainTextEdit_1->viewport()->setCursor(Qt::ArrowCursor);                         // 箭头光标
    ui->plainTextEdit_2->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter)); // 居中
    ui->plainTextEdit_2->viewport()->setCursor(Qt::ArrowCursor);                         // 箭头光标

    auto ec = ui->plainTextEdit_3->textCursor();
    m_label = new QLabel(QString("[%1,%2]").arg(ec.blockNumber() + 1).arg(ec.columnNumber() + 1));
    ui->statusbar->addWidget(m_label);

    // 将按钮添加到状态栏的最右侧
    // 可以缩放
    QPushButton* button1;
    button1 = new QPushButton("Come on, try me?"); // 按键1
    ui->statusbar->addPermanentWidget(button1);
    connect(button1, &QPushButton::clicked, this, [this]() { ui->plainTextEdit_3->appendPlainText("看你急了！"); });
    button1->setStyleSheet(";");
    // 不可以缩放
    QPushButton* button2;
    button2 = new QPushButton("点我试试！");
    ui->statusbar->addPermanentWidget(button2);
    connect(button2, &QPushButton::clicked, this, [this]() {
        ui->plainTextEdit_3->appendPlainText("不能缩放啊啊啊！");
    });
    button2->setStyleSheet(";");

    // 获取桌面可活动区域
    auto screen_rect = QApplication::primaryScreen()->availableGeometry();
    auto font = QApplication::font();
#if USE_LZL_QT_SETTINGS
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
    // 绑定设置事件
    id1 = lzl::Settings::connectReadValue("app/font/size", [button1](double size) {
        auto font = QApplication::font();
        font.setPointSizeF(size);
        QApplication::setFont(font);
        button1->setFont(font);
    });
    id2 = lzl::Settings::connectReadValue("app/window/size", this, &MainWindow::resize);
    id3 = lzl::Settings::connectReadValue("app/window/pos", this, &MainWindow::move);
    // 读取设置
    lzl::Settings::emitReadValuesFromGroup("app");
#else
    // ***
    connect(this, &MainWindow::fontSizeChange, this, [button1](double sizeF) {
        auto font = QApplication::font();
        font.setPointSizeF(sizeF);
        button1->setFont(font);
    });
    QSettings settings(CONFIG_INI, QSettings::IniFormat);
    font.setPointSizeF(settings.value("app/font/size", font.pointSizeF() * 1.728).toDouble());
    // 限制值
    if (font.pointSizeF() < 4 || font.pointSizeF() > 48)
    {
        font.setPointSizeF(font.pointSizeF() * 1.728); // 1.2 * 1.2 * 1.2
    }
    QApplication::setFont(font);

    this->move(settings.value("app/window/pos", QPoint(0, 0)).toPoint());
    // 限制窗体坐标
    if (!screen_rect.contains(this->pos()))
    {
        this->move({0, 0});
    }
    this->resize(settings.value("app/window/size", QSize(800, 600)).toSize());
    // 限制窗体大小
    auto size = this->size();
    if (size.width() < 400 || size.height() < 300 || size.width() > screen_rect.width()
        || size.height() > screen_rect.height())
    {
        this->resize(800, 600);
    }
    button1->setFont(font);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
#if USE_LZL_QT_SETTINGS
    // 注销设置
    if (lzl::Settings::containsGroup("app"))
    {
        lzl::Settings::deRegisterSettingGroup("app");
    }
#endif
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        if (event->key() == Qt::Key_Equal)
        {
            on_pushButton_plus_clicked();
        }
        else if (event->key() == Qt::Key_Minus)
        {
            on_pushButton_minus_clicked();
        }
    }
}

void MainWindow::moveEvent(QMoveEvent* event)
{
    QWidget::moveEvent(event);
#if USE_LZL_QT_SETTINGS
    // 如果没有注销的话直接写入（正常来说也不应该在结束前注销）
    if (lzl::Settings::containsKey("app/window/pos"))
    {
        lzl::Settings::writeValue("app/window/pos", this->pos());
    }
#else
    QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/window/pos", this->pos());
#endif
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
#if USE_LZL_QT_SETTINGS
    // 如果没有注销的话直接写入（正常来说也不应该在结束前注销）
    if (lzl::Settings::containsKey("app/window/size"))
    {
        lzl::Settings::writeValue("app/window/size", this->size());
    }
#else
    QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/window/size", this->size());
#endif
}

void MainWindow::on_pushButton_minus_clicked()
{
    auto font = QApplication::font();
    font.setPointSizeF(font.pointSizeF() / 1.2);
#if USE_LZL_QT_SETTINGS
    // 没有注销的话直接写入
    if (lzl::Settings::containsKey("app/font/size"))
    {
        lzl::Settings::writeValue("app/font/size", font.pointSizeF(), true);
    }
    // 否则只能通过常规方式（正常来说也不应该在结束前注销）
    else
#endif
    {
        if (font.pointSizeF() < 4)
        {
            font.setPointSizeF(4);
        }
        emit fontSizeChange(font.pointSizeF());
        QApplication::setFont(font);
#if !USE_LZL_QT_SETTINGS
        QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/font/size", font.pointSizeF());
#endif
    }
}

void MainWindow::on_pushButton_plus_clicked()
{
    auto font = QApplication::font();
    font.setPointSizeF(font.pointSizeF() * 1.2);
#if USE_LZL_QT_SETTINGS
    // 没有注销的话直接写入
    if (lzl::Settings::containsKey("app/font/size"))
    {
        lzl::Settings::writeValue("app/font/size", font.pointSizeF(), true);
    }
    // 否则只能通过常规方式（正常来说也不应该在结束前注销）
    else
#endif
    {
        if (font.pointSizeF() > 48)
        {
            font.setPointSizeF(48);
        }
        emit fontSizeChange(font.pointSizeF());
        QApplication::setFont(font);
#if !USE_LZL_QT_SETTINGS
        QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/font/size", font.pointSizeF());
#endif
    }
}

void MainWindow::on_plainTextEdit_3_cursorPositionChanged()
{
    auto ec = ui->plainTextEdit_3->textCursor();
    m_label->setText(QString("[%1,%2]").arg(ec.blockNumber() + 1).arg(ec.columnNumber() + 1));
}

void MainWindow::on_pushButton_reset_all_clicked()
{
#if USE_LZL_QT_SETTINGS
    if (lzl::Settings::containsGroup("app"))
    {
        lzl::Settings::reset("app");
        lzl::Settings::emitReadValuesFromKey("app/font/size");
        QCoreApplication::processEvents();
        QTimer::singleShot(0, this, []() { lzl::Settings::emitReadValuesFromGroup("app/window"); });
    }
    else
    {
        ui->plainTextEdit_3->appendPlainText("app 已被注销！");
    }
#else
    QSettings(CONFIG_INI, QSettings::IniFormat).clear();
    // 重置所有设置
    auto font = QApplication::font();
    font.setPointSizeF(12 * 1.728);
    QApplication::setFont(font);
    emit fontSizeChange(font.pointSizeF());
    QCoreApplication::processEvents();
    QTimer::singleShot(0, this, [this]() {
        this->resize(800, 600);
        this->move(0, 0);
    });
#endif
}

void MainWindow::on_pushButton_reset_font_clicked()
{
#if USE_LZL_QT_SETTINGS
    if (lzl::Settings::containsKey("app/font/size"))
    {
        lzl::Settings::reset("app/font/size");
        lzl::Settings::emitReadValuesFromKey("app/font/size");
    }
    else
    {
        ui->plainTextEdit_3->appendPlainText("app/font/size 已被注销！");
    }
#else
    QSettings(CONFIG_INI, QSettings::IniFormat).remove("app/font/size");
    auto font = QApplication::font();
    font.setPointSizeF(12 * 1.728);
    QApplication::setFont(font);
    emit fontSizeChange(font.pointSizeF());
    QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/font/size", font.pointSizeF());
#endif
}

void MainWindow::on_pushButton_reset_window_clicked()
{
#if USE_LZL_QT_SETTINGS
    if (lzl::Settings::containsGroup("app/window"))
    {
        lzl::Settings::reset("app/window");
        lzl::Settings::emitReadValuesFromGroup("app/window");
    }
    else
    {
        ui->plainTextEdit_3->appendPlainText("app/window 已被注销！");
    }
#else
    QSettings(CONFIG_INI, QSettings::IniFormat).remove("app/window/size");
    QSettings(CONFIG_INI, QSettings::IniFormat).remove("app/window/pos");
    QTimer::singleShot(0, this, [this]() {
        this->resize(800, 600);
        this->move(0, 0);
    });
#endif
}

void MainWindow::on_pushButton_de_all_clicked()
{
#if USE_LZL_QT_SETTINGS
    if (lzl::Settings::containsGroup("app"))
    {
        lzl::Settings::deRegisterSettingGroup("app");
        ui->plainTextEdit_3->appendPlainText("app 注销成功！所有设置将不会被记录！");
    }
    else
    {
        ui->plainTextEdit_3->appendPlainText("app 已经注销过！");
    }
#else
    ui->plainTextEdit_3->appendPlainText("常规方法不支持注销设置！");
#endif
}

void MainWindow::on_pushButton_de_font_clicked()
{
#if USE_LZL_QT_SETTINGS
    if (lzl::Settings::containsKey("app/font/size"))
    {
        lzl::Settings::deRegisterSettingKey("app/font/size");
        ui->plainTextEdit_3->appendPlainText("app/font/size 注销成功！改变字体将不会被记录！");
    }
    else
    {
        ui->plainTextEdit_3->appendPlainText("app/font/size 已经注销过！");
    }
#else
    ui->plainTextEdit_3->appendPlainText("常规方法不支持注销设置！");
#endif
}

void MainWindow::on_pushButton_de_window_clicked()
{
#if USE_LZL_QT_SETTINGS
    if (lzl::Settings::containsGroup("app/window"))
    {
        lzl::Settings::deRegisterSettingGroup("app/window");
        ui->plainTextEdit_3->appendPlainText("app/window 注销成功！改变窗体大小和位置将不会被记录！");
    }
    else
    {
        ui->plainTextEdit_3->appendPlainText("app/window 已经注销过！");
    }
#else
    ui->plainTextEdit_3->appendPlainText("常规方法不支持注销设置！");
#endif
}

void MainWindow::on_pushButton_dc_all_clicked()
{
#if USE_LZL_QT_SETTINGS
    lzl::Settings::disconnectAllSettingsReadValues();
    // lzl::Settings::disconnectReadValue(id1);
    // lzl::Settings::disconnectReadValue(id2);
    // lzl::Settings::disconnectReadValue(id3);
    ui->plainTextEdit_3->appendPlainText("所有读取设置事件断开连接！");
#else
    ui->plainTextEdit_3->appendPlainText("常规方法不支持断开连接！");
#endif
}

void MainWindow::on_pushButton_dc_font_clicked()
{
#if USE_LZL_QT_SETTINGS
    lzl::Settings::disconnectReadValuesFromKey("app/font/size");
    ui->plainTextEdit_3->appendPlainText("键 app/font/size 读取设置事件断开连接！");
#else
    ui->plainTextEdit_3->appendPlainText("常规方法不支持断开连接！");
#endif
}

void MainWindow::on_pushButton_dc_window_clicked()
{
#if USE_LZL_QT_SETTINGS
    lzl::Settings::disconnectReadValuesFromGroup("app/window");
    ui->plainTextEdit_3->appendPlainText("组 app/window 读取设置事件断开连接！");
#else
    ui->plainTextEdit_3->appendPlainText("常规方法不支持断开连接！");
#endif
}
