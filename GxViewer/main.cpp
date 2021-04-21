//This is the main
//More explaination about the code, can you find in GxViewer.cpp

//--------------------------------------------------------------------------------
/**
\file     main.cpp
\brief    Application entry, create the MainWindow

\version  v1.0.1807.9271
\date     2018-07-27

<p>Copyright (c) 2017-2018</p>
*/
//----------------------------------------------------------------------------------
#include <QApplication>
#include "GxViewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Set Qt window style
    //QApplication::setStyle("windows");   // or windows etc.

	//Create a QApplication
    CGxViewer w;

    QFont font = w.font();
    font.setPointSize(10);
    w.setFont(font);

	//Show object
    w.show();

    return a.exec();
}
