#include "mainwindow.h"
#include <QGroupBox>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QString{"Coin²"});
    // Create menus
    _file_menu = menuBar()->addMenu(QString{tr("&File")});
    _help_menu = menuBar()->addMenu(QString{tr("&Help")});
    // Populate menus (menu items)
    _file_menu->addAction(QString{"Preferences"});
    _file_menu->addAction(QString{"Quit"});
    _help_menu->addAction(QString{"Manual"});
    _help_menu->addAction(QString{"About"});
    // Container for window
    _main_widget.reset(new QGroupBox{this});
    setCentralWidget(_main_widget.get());
    // Layout in container
    QVBoxLayout *layout = new QVBoxLayout{};
    // Button
    _button.reset(new QPushButton{QString{"Press"}, _main_widget.get()});
    layout->addWidget(_button.get());
    // Text area
    _text_edit.reset(new QLineEdit{_main_widget.get()});
    layout->addWidget(_text_edit.get());
    // List
    QStringList places = {"New York", "Paris", "Beijing"};
    _list.reset(new QListWidget{_main_widget.get()});
    _list->addItems(places);
    layout->addWidget(_list.get());
    // Put layout into central widget
    _main_widget->setLayout(layout);
}
MainWindow::~MainWindow() {}
