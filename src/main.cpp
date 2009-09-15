/**
 * \file main.cpp
 * \brief Application entry point
 * \author Diego Iastrubni elcuco@kde.org
 * License GPLv3, 2008,2009
 */

#include <QApplication>
#include "mainwindowimpl.h"

int main(int argc, char ** argv)
{
	QApplication app( argc, argv );
	MainWindowImpl win;
	win.show(); 
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
}
