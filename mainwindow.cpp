/**
 * @file mainwindow.cpp
 * @brief Implémentation de la classe MainWindow.
 */

#include "mainwindow.h"
#include <QApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QPixmap>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>

using namespace std;

/**
 * @brief Constructeur de MainWindow.
 *
 * Initialise la fenêtre principale et délègue la configuration de l'interface
 * à la méthode setupUi().
 *
 * @param parent Widget parent (nullptr par défaut).
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi();
  connectSignalsSlots();
}

/**
 * @brief Destructeur de MainWindow.
 *
 * La gestion de la mémoire est automatique grâce aux QScopedPointer
 * et au système de parenté de Qt.
 */
MainWindow::~MainWindow() {}

/**
 * @brief Configure l'interface utilisateur complète.
 *
 * Définit le titre de la fenêtre et appelle les méthodes spécialisées
 * pour créer les différentes parties de l'interface.
 */
void MainWindow::setupUi() {
  setWindowTitle(QString{"Droit_But"});

  createMenus();
  createWidgets();
  setupLayouts();
}

/**
 * @brief Crée et configure les menus de l'application.
 *
 * Initialise les menus Fichier et Aide avec leurs actions respectives.
 */
void MainWindow::createMenus() {
  // Create menus
  _file_menu = menuBar()->addMenu(QString{tr("&File")});
  _help_menu = menuBar()->addMenu(QString{tr("&Help")});

  // Create actions
  _pref_action = new QAction(tr("&Preferences"), this);
  _quit_action = new QAction(tr("&Quit"), this);
  _manual_action = new QAction(tr("&Manual"), this);
  _about_action = new QAction(tr("&About"), this);

  // Add keyboard shortcuts
  _quit_action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));

  // Populate menus (menu items)
  _file_menu->addAction(_pref_action);
  _file_menu->addAction(_quit_action);
  _help_menu->addAction(_manual_action);
  _help_menu->addAction(_about_action);
}

/**
 * @brief Crée et initialise tous les widgets de l'interface.
 *
 * Cette méthode instancie le conteneur principal, le bouton, le champ de texte,
 * la liste des villes et le widget de carte.
 */
void MainWindow::createWidgets() {
  // Container for window
  _main_widget.reset(new QGroupBox{this});
  setCentralWidget(_main_widget.get());

  // Button
  _button.reset(new QPushButton{QString{"Search"}, _main_widget.get()});

  // Text area
  _text_edit.reset(new QLineEdit{_main_widget.get()});

  // List
  QStringList places = {"New York", "Paris", "Beijing"};
  _list.reset(new QListWidget{_main_widget.get()});
  _list->addItems(places);

  // Widget pour la carte
  _map_widget.reset(new QLabel{_main_widget.get()});
  _map_widget->setMinimumSize(300, 300);
  _map_widget->setAlignment(Qt::AlignCenter);
  _map_widget->setText("Carte du monde");
  _map_widget->setStyleSheet(
      "border: 1px solid black; background-color: #f0f0f0;");
}

/**
 * @brief Configure les layouts pour organiser les widgets.
 *
 * Met en place un layout horizontal principal divisé en deux parties :
 * - Partie gauche : contrôles (bouton, champ de texte, liste)
 * - Partie droite : carte du monde
 *
 * Le ratio entre les deux parties est de 1:2.
 */
void MainWindow::setupLayouts() {
  // Layout principal horizontal
  QHBoxLayout *mainLayout = new QHBoxLayout{};

  // Layout pour la partie gauche (contrôles existants)
  QVBoxLayout *leftLayout = new QVBoxLayout{};

  leftLayout->addWidget(_button.get());
  leftLayout->addWidget(_text_edit.get());
  leftLayout->addWidget(_list.get());

  // Ajout des layouts au layout principal
  mainLayout->addLayout(leftLayout);
  mainLayout->addWidget(_map_widget.get());

  // Définir le ratio entre la partie gauche et la carte (1:2)
  mainLayout->setStretchFactor(leftLayout, 1);
  mainLayout->setStretchFactor(_map_widget.get(), 2);

  // Put layout into central widget
  _main_widget->setLayout(mainLayout);
}

/**
 * @brief Connecte les signaux aux slots appropriés.
 * 
 * Cette méthode établit les connexions entre les signaux émis par
 * les widgets (comme les actions de menu) et les slots qui les traitent.
 */
void MainWindow::connectSignalsSlots()
{
    // Connexion des actions du menu File
    connect(_quit_action, &QAction::triggered, this, &MainWindow::onQuitTriggered);
    connect(_pref_action, &QAction::triggered, this, &MainWindow::onPreferencesTriggered);
    
    // Connexion des actions du menu Help
    connect(_manual_action, &QAction::triggered, this, &MainWindow::onManualTriggered);
    connect(_about_action, &QAction::triggered, this, &MainWindow::onAboutTriggered);
}

/**
 * @brief Slot appelé lorsque l'utilisateur clique sur "Quitter".
 * 
 * Ferme l'application proprement.
 */
void MainWindow::onQuitTriggered()
{
    // Ferme l'application
    QApplication::quit();
}

/**
 * @brief Slot appelé lorsque l'utilisateur clique sur "Préférences".
 * 
 * Affiche la boîte de dialogue des préférences.
 */
void MainWindow::onPreferencesTriggered()
{
    //TODO: afficher la boîte de dialogue des préférences
    QMessageBox::information(this, tr("Préférences"), tr("Boîte de dialogue des préférences"));
}

/**
 * @brief Slot appelé lorsque l'utilisateur clique sur "Manuel".
 * 
 * Affiche le manuel d'utilisation.
 */
void MainWindow::onManualTriggered()
{
    //TODO: afficher le manuel d'utilisation
    QMessageBox::information(this, tr("Manuel"), tr("Manuel d'utilisation"));
}

/**
 * @brief Slot appelé lorsque l'utilisateur clique sur "À propos".
 * 
 * Affiche la boîte de dialogue "À propos".
 */
void MainWindow::onAboutTriggered()
{
    QMessageBox::about(this, tr("À propos de Droit_But"),
                       tr("Droit_But v1.0\n\n"
                          "Une application pour apprendre QT !\n"
                          "© 2025 - Olivier - Tous droits réservés."));
}
