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


    db.setDatabaseName("vince");
    db.setUserName("vince");
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
/*
 * try query.fetch() to get the inserted record number
 * then record to get the full inserrted record.
 *
       QSqlResult a=query.result();
        a.fetchFirst();
        QSqlRecord b = a.record();


        QMessageBox::information(this,"record results",recordNumber);

        QString returningID = query.record.field("bookid");
        QMessageBox::information(this,"the returned Record ID",returningID);

        */

           query.first();  // returns bool
        QSqlRecord a = query.record();


        QString  result = a.field("bookid").value().toString();
        QMessageBox::information(this,"results",result);


        QMessageBox::information(this,"insert results","Success");
        if(ret)
        {
            ui->result->setText("Pub has been inserted");
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
//            out.append(que.value(5).toString());  //  out +=", ";
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
