#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
 * db connect default setttings
 */

QString MYDBType = "Postgresql";
QString MYHost = "localhost";
QString MYDB = "vince";
QString MYUserName = "vince";
QString MYTable = "shelf";
QString MYPassword = "";
bool SSL = false;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
    /*
     * build a list of genre for the  combo box
     */

    QStringList genreList=(QStringList()
                           << "Action and Adventure"
                           << "Anthology"
                           << "Art"
                           << "Autobiographies"
                           << "Biographies"
                           << "Children's"
                           << "Comics"
                           << "Cookbooks"
                           << "Diaries"
                           << "Dictionaries"
                           << "Drama"
                           << "Encyclopedias"
                           << "Fantasy"
                           << "Guide"
                           << "Health"
                           << "History"
                           << "Horror"
                           << "Journals"
                           << "Math"
                           << "Mystery"
                           << "Poetry"
                           << "Prayer books"
                           << "Religion, Spirituality & New Age"
                           << "Romance"
                           << "Satire"
                           << "Science"
                           << "Science fiction"
                           << "Self help"
                           << "Series"
                           << "Travel"
                           << "Trilogy"
                           );
    ui->comboBox->addItems(genreList);
}


MainWindow::~MainWindow()
{
    /*
     * should be closed already, via the close event
     */
    if ( db.isOpen() )
    {
        QMessageBox::warning(this,"Warning","Close event failed");
        std::cout << "Eep: close event didn't work" << std::endl;
        db.close();
    }
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    case QEvent::Close:
        /*
         * this doesn't appear to work
         */

        std::cout << "QEvent close" << std::endl;
        break;
    default:
        break;
    }
}

bool MainWindow::openDB()
{
    if (!QSqlDatabase::drivers().contains("QPSQL"))
        QMessageBox::critical(this, "Unable to load database", "The QPSQL driver is required");


    db  = (QSqlDatabase::addDatabase("QPSQL"));
    db.setHostName(MYHost);
    db.setDatabaseName(MYDB);
    db.setUserName(MYUserName);
    db.setPassword(MYPassword);


    if ( SSL ) {
        db.setConnectOptions("requiressl=yes");
    }

    bool ret = db.open();
    if ( ! ret ) {
        QMessageBox::critical(this,"PubStor","DB open failed " + db.lastError().text()  );
    }
    return ret;
}

void MainWindow::addpub()
{
    /*
     * add a new entry into database
     */

    if( ! db.isOpen())
    {
        MainWindow::openDB();
    }

    QSqlQuery query;
    QString args = "INSERT INTO ";
    args.append(MYTable);
    args.append("(title,author,publisher,isbn,genre) VALUES (?,?,?,?,?) RETURNING bookid;");
    query.prepare(args);

    query.addBindValue(ui->title->text());
    query.addBindValue(ui->author->text());
    query.addBindValue(ui->publisher->text());
    query.addBindValue(ui->isbn->text());
    query.addBindValue(ui->comboBox->currentText());

    /*
     * will likely not be seen unless response time is very bad
     */
    ui->result->setText("Pub is being inserted");


    if( ! query.exec())
    {
        QMessageBox::critical(this,"Fail","Insert failed with: \"" + query.lastError().text() + "\"");
        return;
    }

    ui->result->setText("record inserted OK");

    /*
     * this returns the results of the  'returning' phrase
     * QString lastid = query.lastInsertId().toString();
     * qWarning() << "got another version " << lastid;
     */


    /*
     * returns the column bookid
     */

    query.first();  // query.next() also works.
    QString what = query.value(0).toString();
    qWarning() << "got " + what ;
    QMessageBox::information(this,"returning bookid Method 0",what);

}


void MainWindow::clearpub()
{
    ui->author->clear();
    ui->isbn->clear();
    ui->publisher->clear();
    ui->title->clear();
    ui->result->clear();
}


void MainWindow::searchpub()
{
    bool ret = true;
    //    ui->searchResults->clear(); // clear previous output
    if(!db.isOpen())
    {
        ret = MainWindow::openDB();
    }

    if ( ! ret ) {
        // it did not open successfully
        return;
    }

    QString searchOutputLine;
    QString searchSQLString;

    /*
     * build the search string
     */
    if ("localhost" != MYHost) {
        searchSQLString = "SELECT * FROM ";
    }
    else {
        searchSQLString = "SELECT title,author,isbn,genre,publisher FROM ";
    }

    searchSQLString += MYTable;
    searchSQLString += " WHERE ";
    searchSQLString += ui->srcq->currentText();
    searchSQLString += " ILIKE '%";
    searchSQLString += ui->search->text();
    searchSQLString += "%' ";
    searchSQLString += "ORDER BY ";
    searchSQLString += ui->srcq->currentText();

    //    searchSQLString += " like % ;"; // syntax error.
    //    searchSQLString += " LIKE  %:what% ;";  // gives error
    //    searchSQLString += " LIKE  '%:what%' ;";  // Fails to find anything.
    //    searchSQLString += " ILIKE  '%:what%' ;";  // Fails to find anything.
    //    searchSQLString += " ILIKE  '%?%' ;";  // using addBindValue Fails to find anything
    //    searchSQLString += " ILIKE  %?% ;";  // using addBindValue syntax error.
    //    searchSQLString += " = :what ;";  //  Works.
    //    qWarning() <<"this is your  last qWarning :)";


    /*
     * now do the search


    QMessageBox mb;
       mb.setText(searchSQLString);
       mb.exec();
    */

    this->model = new QSqlQueryModel();

    this->model->setQuery(searchSQLString,db);   // do the search


    if (this->model->lastError().isValid())
    {
        QMessageBox::critical(this,"PubStor","Query Failed " + this->model->lastError().text()  );
    }

    ui->searchResults->setModel(model);  // display the results

    //    query.addBindValue(ui->search->text());

    /*

    if( !query.exec() )
    {
        mb.setText("DB search failed with \"" + query.lastError().text() + "\" the search string was \"" + searchSQLString + "\"");
        //             mb.setWindowIcon(QIcon("icon.png"));
        mb.exec();
        return;     // no sense in continuing.
    }


     * loop thru the results building an output block, there could be many.

    while (query.next()) {
        searchAnswersFound = true;
        searchOutputLine.append(query.value(0).toString());    searchOutputLine +=", ";
        searchOutputLine.append(query.value(1).toString());    searchOutputLine +=", ";
        searchOutputLine.append(query.value(2).toString());    searchOutputLine +=", ";
        searchOutputLine.append(query.value(3).toString());    searchOutputLine +=", ";
        searchOutputLine.append(query.value(4).toString());    searchOutputLine +=", ";
        searchOutputLine += "\n";
    }

    if ( searchAnswersFound ) {
        ui->searchResults->setText(searchOutputLine);
        //        ui->search->clear();  // clear input

    }
    else
    {
        ui->searchResults->setText("Nothing found." );
    }
    */

}


void MainWindow::on_search_returnPressed()
{
    searchpub();
}

void MainWindow::on_settingsTab_tabBarClicked(int index)
{


    /*
     * Inialize the settings values
     * not convinced this is needed
     */

    // std::cout<<"index is " << index <<std::endl;

    /*
     * which tab has been clicked
     */

    switch (index) {
    case  0:
        break;

    case 1:
        break;

    case 2:
        loadSettings();  // settings tab was clicked
        break;
    default:
        break;
    }
}


void MainWindow::on_SettingsLoadButton_clicked()
{
    loadSettings();
}

void MainWindow::loadSettings(){
    /*
     * setup the config file
     */

    QSettings settings("PubStor","dbsetings");
    settings.beginGroup("network");
    MYDBType = settings.value("PostGresql",MYDBType).toString();
    MYHost = settings.value("hostName",MYHost).toString();
    MYDB = settings.value("dbName",MYDB).toString();
    MYTable = settings.value("tableName",MYTable).toString();
    MYUserName = settings.value("user",MYUserName).toString();
    MYPassword = settings.value("password",MYPassword).toString();
    SSL = settings.value("SSL",SSL).toBool();
    settings.endGroup();

    /*
     * display the results of the settings, either the defaultvalues or those read
     */

    ui->HostNameEntry->setText(MYHost);
    ui->DBNameEntry->setText(MYDB);
    ui->UserNameEntry->setText(MYUserName);
    ui->TableNameEntry->setText(MYTable);
    ui->PasswordEntry->setText(MYPassword);
    ui->SSLCheck->setChecked(SSL);

}

void MainWindow::on_SettingsSaveButton_clicked()
{
    saveSettings();
}

void MainWindow::saveSettings(){

    /*
     * get the changes wrought
     */

    // MYDBType =
    MYHost = ui->HostNameEntry->text();
    MYDB = ui->DBNameEntry->text();
    MYUserName = ui->UserNameEntry->text();
    MYTable = ui->TableNameEntry->text();
    MYPassword = ui->PasswordEntry->text();
    SSL = ui->SSLCheck->checkState();

    /*
     * write out the config file
     */

    QSettings settings("PubStor","dbsetings");
    settings.beginGroup("network");
    settings.setValue("DBType",MYDBType);
    settings.setValue("hostName",MYHost);
    settings.setValue("dbName",MYDB);
    settings.setValue("tableName",MYTable);
    settings.setValue("user",MYUserName);
    settings.setValue("password",MYPassword);
    settings.setValue("SSL",SSL);
    settings.endGroup();

    if ( db.isOpen() ) db.close();  // else the new settings will not be used.
    /*
     * hopefully this will see if any of the open params are bad
     */

    openDB();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    db.close();
    if (event ) {
        // just suppress unused warning
    }
    //std::cout << "OK bye" << std::endl;

    /*
     * fun with intercepting the close button
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "PubStor",
                                                                    tr("Quit: Are you sure?\n"),
                                                                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            event->accept();
        }
        */
}
