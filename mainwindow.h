#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QtSql>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlResult>
#include <QSettings>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void addpub();
    void clearpub();
    void searchpub();
    void settings();

    void saveSettings();
    void loadSettings();

    void on_settingsTab_tabBarClicked(int index);

    void on_SettingsLoadButton_clicked();

    void on_SettingsSaveButton_clicked();

protected:
    void changeEvent(QEvent *e);
    bool openDB();


private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
};

#endif // MAINWINDOW_H
