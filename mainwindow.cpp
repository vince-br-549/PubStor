#include "mainwindow.h"
#include "ui_mainwindow.h"

bool SillySwitch = false;

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
        QMessageBox::critical(this, "Unable to load database", "This demo needs the QPSQL driver");

    db  = (QSqlDatabase::addDatabase("QPSQL"));
    db.setHostName(MYHost);


    db.setDatabaseName(MYDB);
    db.setUserName(MYUserName);
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

    QString args = "INSERT INTO ";
    args.append(MYTable);
    args.append("(title,author,publisher,isbn,genre) VALUES ('");
    args.append(ui->title->text()); args.append("','");
    args.append(ui->author->text()); args.append("','");
    args.append(ui->publisher->text()); args.append("','");
    args.append(ui->isbn->text()); args.append("','");
    args.append(ui->genre->text()); args.append( " ') ");
    args.append(ui->genre->text()); args.append( "returning bookid");
    args.append(ui->genre->text()); args.append( "; ");
    if(!db.isOpen())
    {
        lck = MainWindow::openDB();
    }

    if( lck )
    {
        ui->result->setText("Pub is being inserted");
        QSqlQuery query;
        query.addBindValue("sth");

        ret = query.exec(args);

        if(!ret)
        {
            QMessageBox::information(this,"Fail","Insert failed with: \"" + query.lastError().text() + "\"");
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
            //while (query.next()) {
            query.first();
            QString what = query.value(0).toString();
                qWarning() << "got " + what ;
                QMessageBox::information(this,"returning book Id Method 0",what);
            //}


            /*
             * this also returns the inserted record number.


            query.first();  // returns bool
            QSqlRecord a = query.record(); // get the returning row of result.


            int  resultInt  = a.field("bookid").value().toInt(); // ask for the field value by name.
            char result[6];
            sprintf(result,"%d",resultInt); // convert it into a string

            QMessageBox::information(this,"returning book Id Method 1",result);
            */
    }
}

void MainWindow::clearpub()
{
    ui->author->clear();
    ui->isbn->clear();
    ui->publisher->clear();
    ui->title->clear();
    ui->genre->clear();
    ui->result->clear();
}

void MainWindow::searchpub()
//void MainWindow::on_searchButton_clicked()
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
    QSqlQuery que;

    bool searchAnswersFound = false;


    /*
     * build the search string
     */
    searchSQLString = "SELECT title,author,isbn,genre,publisher FROM ";
//    searchSQLString = "SELECT * FROM ";

    searchSQLString += MYTable;
    searchSQLString += " WHERE ";
    searchSQLString += ui->srcq->currentText();
    searchSQLString += " LIKE '%";
    searchSQLString += ui->search->text();
    searchSQLString += "%' ;";


    qWarning() <<"this is your  last qWarning :)";

    /*
        if (SillySwitch) {
            SillySwitch = false;
            QMessageBox::information(this,"SillySwitch","True");
        } else {
            SillySwitch = true;
            QMessageBox::information(this,"SillySwitch","False");
        }
*/

    /*
     * now do the search
     */
    if( !que.exec(searchSQLString) )
    {
        QMessageBox mb;
        mb.setText("DB search failed with \"" + que.lastError().text() + "\"");
        //             mb.setWindowIcon(QIcon("icon.png"));
        mb.exec();
        return;     // no sense in continuing.
    }


    /*
     * loop thru the results building an output block, there could be many.
     */
    while (que.next()) {
        searchAnswersFound = true;
        searchOutputLine.append(que.value(0).toString());    searchOutputLine +=", ";
        searchOutputLine.append(que.value(1).toString());    searchOutputLine +=", ";
        searchOutputLine.append(que.value(2).toString());    searchOutputLine +=", ";
        searchOutputLine.append(que.value(3).toString());    searchOutputLine +=", ";
        searchOutputLine.append(que.value(4).toString());    searchOutputLine +=", ";
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
