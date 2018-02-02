#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"


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
    delete ui;
    if ( db.isOpen() ) db.close();
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
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
    bool ret = false;
    bool lck = true;

    if(!db.isOpen())
    {
        lck = MainWindow::openDB();
    }

    if( lck )
    {
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

        ui->result->setText("Pub is being inserted");  // probably only of use if very poor response time.


        ret = query.exec();

        if(!ret)
        {
            QMessageBox::critical(this,"Fail","Insert failed with: \"" + query.lastError().text() + "\"");

//            QMessageBox::information(this,"Fail","Insert failed with: \"" + query.lastError().text() + "\"");
            return;
        }
        ui->result->setText("record inserted OK");

        /*
             * this returns the results of the  'returning' phrase


            QString lastid = query.lastInsertId().toString();
            qWarning() << "got another version " << lastid;
             */


        /*
         * the following bullshit returns the record number
         */

        query.first();  // query.next() also works.
        QString what = query.value(0).toString();
        qWarning() << "got " + what ;
        QMessageBox::information(this,"returning book Id Method 0",what);
    }
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
    ui->searchResults->clear(); // clear previous output
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
    QSqlQuery query;

    bool searchAnswersFound = false;


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
    searchSQLString += " LIKE '";
    searchSQLString += ui->search->text();
    searchSQLString += "%'";

    //    searchSQLString += " like % ;"; // syntax error.
    //    searchSQLString += " LIKE  %:what% ;";  // gives error
    //    searchSQLString += " LIKE  '%:what%' ;";  // Fails to find anything.
    //searchSQLString += " ILIKE  '%:what%' ;";  // Fails to find anything.
    //    searchSQLString += " ILIKE  '%?%' ;";  // using addBindValue Fails to find anything
    //    searchSQLString += " ILIKE  %?% ;";  // using addBindValue syntax error.


    //searchSQLString += " = :what ;";  //  Works.




    //    qWarning() <<"this is your  last qWarning :)";


    /*
     * now do the search
     */

    QMessageBox mb;
    //    mb.setText(searchSQLString);
    //    mb.exec();

    query.prepare(searchSQLString);
    query.addBindValue(ui->search->text());


    if( !query.exec() )
    {
        mb.setText("DB search failed with \"" + query.lastError().text() + "\" the search string was \"" + searchSQLString + "\"");
        //             mb.setWindowIcon(QIcon("icon.png"));
        mb.exec();
        return;     // no sense in continuing.
    }


    /*
     * loop thru the results building an output block, there could be many.
     */
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

}


void MainWindow::settings(){

}


/*
 * Inialize the settings values
 */
void MainWindow::on_settingsTab_tabBarClicked(int index)
{
    std::cout<<"index is " << index <<std::endl;
    switch (index) {
    case  0:
        break;

    case 1:
        break;

    case 2:
        if (ui->HostNameEntry->text().isEmpty()){
            ui->HostNameEntry->setText(MYHost);
        }

        if (ui->DBNameEntry->text().isEmpty()) {
            ui->DBNameEntry->setText(MYDB);
        }

        if (ui->UserNameEntry->text().isEmpty()) {
            ui->UserNameEntry->setText(MYUserName);
        }

        if  (ui->TableNameEntry->text().isEmpty()) {
            ui->TableNameEntry->setText(MYTable);
        }

        if (ui->PasswordEntry->text().isEmpty()) {
            ui->PasswordEntry->setText(MYPassword);
        }

        if (SSL) {
            ui->SSLCheck->setCheckState(Qt::Checked);
        } else {
            ui->SSLCheck->setCheckState(Qt::Unchecked);
        }
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
    std::cout <<"Clicked load\n"<<std::endl;
    /*
     * here  one would read from the config file and parse it
     */
}

void MainWindow::on_SettingsSaveButton_clicked()
{
    saveSettings();
}

void MainWindow::saveSettings(){
    std::cout << "Clicked Save\n" << std::endl;
    MYHost = ui->HostNameEntry->text();
    MYDB = ui->DBNameEntry->text();
    MYUserName = ui->UserNameEntry->text();
    MYTable = ui->TableNameEntry->text();
    MYPassword = ui->PasswordEntry->text();
    SSL = ui->SSLCheck->checkState();


    /*
     * here one would write  out the config file.
     */
}
