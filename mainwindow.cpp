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
//    db.setHostName("localhost");
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
    QString args = "INSERT INTO ";
    args.append(MYTable);
    args.append("(title,author,publisher,isbn,genre) VALUES ('");
    bool ret = false;
    bool lck = true;

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

    ui->result->setText("Pub insertion starting");
    if( lck )
    {
        ui->result->setText("Pub is being inserted");
        QSqlQuery query;
        ret = query.exec(args);

        if(ret)
        {
            ui->result->setText("record inserted OK");
/*
 * the following bullshit returns the record number
 */

        query.first();  // returns bool
        QSqlRecord a = query.record(); // get the returning row of result.


        int  resultInt  = a.field("bookid").value().toInt(); // ask for the field value by name.
        char result[6];
        sprintf(result,"%d",resultInt); // convert it into a string

        QMessageBox::information(this,"returning book Id ",result);
        }
        else {
            QMessageBox::information(this,"Fail","Insert failed with: \"" + query.lastError().text() + "\"");
        }
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

    QString out;
    QString findr;
    QSqlQuery que;

    bool myresults = false;

    findr = "SELECT title,author,isbn,genre,publisher FROM ";
    findr += MYTable;
    findr += " WHERE ";
    findr += ui->srcq->currentText();
    findr += " LIKE '%";
    findr += ui->search->text();
    findr += "%' ;";

    if (ret)
    {

/*
        if (SillySwitch) {
            SillySwitch = false;
            QMessageBox::information(this,"SillySwitch","True");
        } else {
            SillySwitch = true;
            QMessageBox::information(this,"SillySwitch","False");
        }
*/



        if( !que.exec(findr) )
        {
            QMessageBox mb;
            mb.setText(que.lastError().text());
            //             mb.setWindowIcon(QIcon("icon.png"));
            mb.exec();
        }

        while (que.next()) {
            myresults = true;
            out.append(que.value(0).toString());    out +=", ";
            out.append(que.value(1).toString());    out +=", ";
            out.append(que.value(2).toString());    out +=", ";
            out.append(que.value(3).toString());    out +=", ";
            out.append(que.value(4).toString());    out +=", ";
            out += "\n";
        }
        if ( myresults ) {
            ui->searchResults->setText(out);
        }

        else
        {
            ui->searchResults->setText("No result found " +findr );
        }
        ui->search->clear();  // clear input

    }
}
