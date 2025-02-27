#include "mainwindow.h"

#include "lzl/settings"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QPushButton>
#include <QSettings>

// 可以通过搜索这个宏查看如果不封装和封装后的区别
#define USE_LZL_QT_SETTINGS 1

#if USE_LZL_QT_SETTINGS
namespace lzl::utils {
template <>
struct ConvertQVariant<QSize>
{
    static auto convert(const QVariant& value) { return value.toSize(); }
};

template <>
struct ConvertQVariant<QPoint>
{
    static auto convert(const QVariant& value) { return value.toPoint(); }
};
} // namespace lzl::utils
#endif // 补充需要转换的类型

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

    auto font = QApplication::font();
#if USE_LZL_QT_SETTINGS
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
#else
    // ***
    connect(this, &MainWindow::fontSizeChange, this, [button1](double sizeF) {
        auto font = QApplication::font();
        font.setPointSizeF(sizeF);
        button1->setFont(font);
    });
    QSettings settings(CONFIG_INI, QSettings::IniFormat);
    if (settings.contains("app/font/size"))
    {
        font.setPointSizeF(settings.value("app/font/size").toDouble());
    }
    else
    {
        font.setPointSizeF(font.pointSizeF() * 1.728); // 1.2 * 1.2 * 1.2
    }
    QApplication::setFont(font);

    if (settings.contains("app/pos"))
    {
        this->move(settings.value("app/pos").toPoint());
        QRect();
    }
    if (settings.contains("app/size"))
    {
        this->resize(settings.value("app/size").toSize());
    }
    button1->setFont(font);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
#if USE_LZL_QT_SETTINGS
    // 注销设置
    lzl::Settings::deRegisterSettings(1);
#endif
}

void MainWindow::on_plainTextEdit_3_cursorPositionChanged()
{
    auto ec = ui->plainTextEdit_3->textCursor();
    m_label->setText(QString("[%1,%2]").arg(ec.blockNumber() + 1).arg(ec.columnNumber() + 1));
}

void MainWindow::on_pushButton_2_clicked()
{
    auto font = QApplication::font();
    font.setPointSizeF(font.pointSizeF() / 1.2);
#if USE_LZL_QT_SETTINGS
    lzl::Settings::writeValue("app/font/size", font.pointSizeF(), true);
#else
    if (font.pointSizeF() < 4)
    {
        font.setPointSizeF(4);
    }
    emit fontSizeChange(font.pointSizeF());
    QApplication::setFont(font);
    QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/font/size", font.pointSizeF());
#endif
}

void MainWindow::on_pushButton_clicked()
{
    auto font = QApplication::font();
    font.setPointSizeF(font.pointSizeF() * 1.2);
#if USE_LZL_QT_SETTINGS
    lzl::Settings::writeValue("app/font/size", font.pointSizeF(), true);
#else
    if (font.pointSizeF() > 64)
    {
        font.setPointSizeF(64);
    }
    emit fontSizeChange(font.pointSizeF());
    QApplication::setFont(font);
    QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/font/size", font.pointSizeF());
#endif
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        if (event->key() == Qt::Key_Equal)
        {
            on_pushButton_clicked();
        }
        else if (event->key() == Qt::Key_Minus)
        {
            on_pushButton_2_clicked();
        }
    }
}

void MainWindow::moveEvent(QMoveEvent* event)
{
    QWidget::moveEvent(event);
#if USE_LZL_QT_SETTINGS
    lzl::Settings::writeValue("app/pos", this->pos());
#else
    QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/pos", this->pos());
#endif
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
#if USE_LZL_QT_SETTINGS
    lzl::Settings::writeValue("app/size", this->size());
#else
    QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/size", this->size());
#endif
}

void MainWindow::on_pushButton_3_clicked()
{
#if USE_LZL_QT_SETTINGS
    lzl::Settings::reset(1);
#else
    QSettings(CONFIG_INI, QSettings::IniFormat).clear();
#endif
}
