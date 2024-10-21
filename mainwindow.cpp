#include "mainwindow.h"

#include "./ui_mainwindow.h"

#include <QApplication>
#include <QPushButton>
#include <QSettings>
#include <QFile>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plainTextEdit_1->document()->setDefaultTextOption(QTextOption(Qt::AlignRight)); // 右对齐
    ui->plainTextEdit_1->viewport()->setCursor(Qt::ArrowCursor); // 箭头光标
    ui->plainTextEdit_2->document()->setDefaultTextOption(QTextOption(Qt::AlignCenter)); // 居中
    ui->plainTextEdit_2->viewport()->setCursor(Qt::ArrowCursor); // 箭头光标


    auto ec = ui->plainTextEdit_3->textCursor();
    m_label = new QLabel(
        QString("[%1,%2]").arg(ec.blockNumber() + 1).arg(ec.columnNumber() + 1)
    );
    ui->statusbar->addWidget(m_label);

    QSettings settings(CONFIG_INI, QSettings::IniFormat);

    auto font = QApplication::font();
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

    // 将按钮添加到状态栏的最右侧
    QPushButton* button;
#if MODEL == 1
    button = new QPushButton("Come on, try me?"); // 按键1
    ui->statusbar->addPermanentWidget(button);
    connect(button, &QPushButton::clicked, this, [this]() {
        ui->plainTextEdit_3->appendPlainText("看你急了！");
    });
    button->setStyleSheet(";");
    // ***
    connect(this, &MainWindow::fontSizeChange, this, [this, button](double sizeF) {
        auto font = QApplication::font();
        font.setPointSizeF(sizeF);
        button->setFont(font);
    });
    button->setFont(font); // ***
#endif
    button = new QPushButton("点我试试！");
    ui->statusbar->addPermanentWidget(button);
    connect(button, &QPushButton::clicked, this, [this]() {
        ui->plainTextEdit_3->appendPlainText("不能缩放啊啊啊！");
    });
    button->setStyleSheet(";");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_plainTextEdit_3_cursorPositionChanged()
{
    auto ec = ui->plainTextEdit_3->textCursor();
    m_label->setText(
        QString("[%1,%2]").arg(ec.blockNumber() + 1).arg(ec.columnNumber() + 1)
    );
}

void MainWindow::on_pushButton_2_clicked()
{
    auto font = QApplication::font();
    font.setPointSizeF(font.pointSizeF() / 1.2);
    if (font.pointSizeF() > 4)
    {
        emit fontSizeChange(font.pointSizeF());
        QApplication::setFont(font);
        QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/font/size", font.pointSizeF());
    }
}

void MainWindow::on_pushButton_clicked()
{
    auto font = QApplication::font();
    font.setPointSizeF(font.pointSizeF() * 1.2);
    if (font.pointSizeF() < 64)
    {
        emit fontSizeChange(font.pointSizeF());
        QApplication::setFont(font);
        QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/font/size", font.pointSizeF());
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        if (event->key() == Qt::Key_Equal)
        {
            on_pushButton_clicked();
            // update();
        }
        else if (event->key() == Qt::Key_Minus)
        {
            on_pushButton_2_clicked();
            // update();
        }
    }
}

void MainWindow::moveEvent(QMoveEvent* event)
{
    QWidget::moveEvent(event);
    QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/pos", this->pos());
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    QSettings(CONFIG_INI, QSettings::IniFormat).setValue("app/size", this->size());
}

void MainWindow::on_pushButton_3_clicked()
{
    QFile::remove(CONFIG_INI);
}
