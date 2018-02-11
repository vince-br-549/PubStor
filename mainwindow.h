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
#include <QCloseEvent>
#include <iostream>

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
    void saveSettings();
    void loadSettings();

    void on_settingsTab_tabBarClicked(int index);

    void on_SettingsLoadButton_clicked();

    void on_SettingsSaveButton_clicked();

    /*
     *  called if user doesnt click the search button, but just hits return in the search query input area.
     */
    void on_search_returnPressed();

    /*
     * this gets called when the user clicks the x button (in top right corner) to close the application
     * you must include the parameters as shown else it doessn't seem to  work
     */
    void closeEvent(QCloseEvent *e);

protected:
    void changeEvent(QEvent *e);
    bool openDB();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQueryModel *model;

};

#endif // MAINWINDOW_H
