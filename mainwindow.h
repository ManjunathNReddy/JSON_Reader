#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QPushButton* m_button_load;
    QTextEdit* m_text_display;
    QCheckBox* m_checkbox_clear;

    std::tuple<int,int,int> parseJSON(QString json_string);
    QString toMessage(std::tuple<int,int,int> parse_results);
    void showText(QString text);

private slots:
    void onLoadClicked();
};
#endif // MAINWINDOW_H
