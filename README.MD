A QT project with the postgresql database to manipulate a book shelf.

I got this initially from:

https://apparatusd.wordpress.com/2010/09/08/simple-qt4-and-postgresql-book-catalog-application/
I sent a comment about some of the problems I found.


Then spent time fixing what seemed to be not working.
Probably because my versions are much newer than the one used on the web page.


2018-2-11 update:

	Use QSqlSearchModel, gives a nice presentation of the output.

	remove db close from window descructor and added a CloseEvent
	to handle it.

	Start adding code for Mysql, and SqlLite

Older updates:

	Added ability to load and save the users own data base connection particulars.


Existing Issues not limited to:

?

Todo:

	Add support for MySql and SQLite


Fixed:



Help and Comments welcome


