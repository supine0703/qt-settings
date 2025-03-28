/**
 * License: GPLv3 LGPLv3
 * Copyright (c) 2024-2025 李宗霖 (Li Zonglin)
 * Email: supine0703@outlook.com
 * GitHub: https://github.com/supine0703
 * Repository: https://github.com/supine0703/qt-settings
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QLabel>
#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public:
    void resize(const QSize& size)
    {
        QMainWindow::resize(size);
        qDebug() << "size: " << size;
    }
    void move(const QPoint& pos)
    {
        QMainWindow::move(pos);
        qDebug() << "pos: " << pos;
    }

private slots:
    void on_plainTextEdit_3_cursorPositionChanged();

    void on_pushButton_reset_all_clicked();

    void on_pushButton_reset_font_clicked();

    void on_pushButton_reset_window_clicked();

    void on_pushButton_de_all_clicked();

    void on_pushButton_de_font_clicked();

    void on_pushButton_de_window_clicked();

    void on_pushButton_minus_clicked();

    void on_pushButton_plus_clicked();

    void on_pushButton_dc_all_clicked();

    void on_pushButton_dc_font_clicked();

    void on_pushButton_dc_window_clicked();

private:
    Ui::MainWindow* ui;
    QLabel* m_label;

signals:
    void fontSizeChange(double);

    // QWidget interface
protected:
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void moveEvent(QMoveEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
};
#endif // MAINWINDOW_H
