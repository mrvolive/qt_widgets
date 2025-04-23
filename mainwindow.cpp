// mainwindow.cpp
#include "mainwindow.h"
#include "controller/mapcontroller.h"
#include "controller/searchcontroller.h"
#include "model/mapmodel.h"
#include "model/placemodel.h"
#include "view/mapwidget.h"

#include <QApplication>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStatusBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Créer les modèles
    _placeModel.reset(new PlaceModel(this));
    _mapModel.reset(new MapModel(this));

    // Créer les contrôleurs
    _searchController.reset(new SearchController(_placeModel.get(), _mapModel.get(), this));
    _mapController.reset(new MapController(_mapModel.get(), this));

    setupUi();
    connectSignalsSlots();
}

MainWindow::~MainWindow() { }

void MainWindow::setupUi()
{
    setWindowTitle(QString { "Droit_But" });

    createMenus();
    createWidgets();
    setupLayouts();

    statusBar()->showMessage("");
    _coordsLabel = new QLabel(this);
    statusBar()->addPermanentWidget(_coordsLabel);
}

void MainWindow::createMenus()
{
    // Create menus
    _file_menu = menuBar()->addMenu(QString { tr("&File") });
    _help_menu = menuBar()->addMenu(QString { tr("&Help") });

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

void MainWindow::createWidgets()
{
    // Container for window
    _main_widget.reset(new QGroupBox { this });
    setCentralWidget(_main_widget.get());

    // Button
    _button.reset(new QPushButton { QString { "Search" }, _main_widget.get() });

    // Text area
    _text_edit.reset(new QLineEdit { _main_widget.get() });

    // List
    QStringList places = { "Les résultats de votre recherche s'afficheront ici" };
    _list.reset(new QListWidget { _main_widget.get() });
    _list->addItems(places);

    // Widget pour la carte (utilisant les modèles et contrôleurs)
    _map_widget.reset(new MapWidget(_mapModel.get(), _mapController.get(), _main_widget.get()));
    _map_widget->setMinimumSize(300, 300);
}

void MainWindow::setupLayouts()
{
    // Layout principal horizontal
    QHBoxLayout* mainLayout = new QHBoxLayout {};

    // Layout pour la partie gauche (contrôles existants)
    QVBoxLayout* leftLayout = new QVBoxLayout {};

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

void MainWindow::connectSignalsSlots()
{
    // Connexion des actions du menu File
    connect(_quit_action, &QAction::triggered, this, &MainWindow::onQuitTriggered);
    connect(_pref_action, &QAction::triggered, this, &MainWindow::onPreferencesTriggered);

    // Connexion des actions du menu Help
    connect(_manual_action, &QAction::triggered, this, &MainWindow::onManualTriggered);
    connect(_about_action, &QAction::triggered, this, &MainWindow::onAboutTriggered);

    // Connexion du bouton Search et du champ de texte
    connect(_button.get(), &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);
    connect(_text_edit.get(), &QLineEdit::returnPressed, this, &MainWindow::onSearchButtonClicked);

    // Connexion de la liste
    connect(_list.get(), &QListWidget::itemClicked, this, &MainWindow::onListItemSelected);

    // Connexion du modèle de lieux
    connect(_placeModel.get(), &PlaceModel::placesUpdated, this, &MainWindow::onPlacesUpdated);
    connect(_placeModel.get(), &PlaceModel::searchError, this, &MainWindow::onSearchError);

    // Connexion pour les coordonnées de la souris
    connect(_map_widget.get(), &MapWidget::mousePositionChanged, this, &MainWindow::onMousePositionChanged);
}

void MainWindow::onQuitTriggered()
{
    // Ferme l'application
    QApplication::quit();
}

void MainWindow::onPreferencesTriggered()
{
    // TODO: afficher la boite de dialogue des préférences
    QMessageBox::information(this, tr("Préférences"), tr("BoÃ®te de dialogue des préférences"));
}

void MainWindow::onManualTriggered()
{
    // TODO: afficher le manuel d'utilisation
    QMessageBox::information(this, tr("Manuel"), tr("Manuel d'utilisation"));
}

void MainWindow::onAboutTriggered()
{
    QMessageBox::about(this, tr("Ã propos de Droit_But"),
        tr("Droit_But v1.0\n\n"
           "Une application pour apprendre QT !\n"
           "Â© 2025 - Olivier - Tous droits réservés."));
}

void MainWindow::onSearchButtonClicked()
{
    QString text = _text_edit->text().trimmed();
    if (text.isEmpty())
        return;

    // Utiliser le contrôleur pour effectuer la recherche
    _searchController->search(text);
}

void MainWindow::onPlacesUpdated(const QStringList& placeNames)
{
    // Mettre à jour la liste avec les nouveaux lieux
    _list->clear();
    _list->addItems(placeNames);
}

void MainWindow::onSearchError(const QString& errorMessage)
{
    QMessageBox::warning(this, tr("Erreur de recherche"), errorMessage);
}

void MainWindow::onListItemSelected(QListWidgetItem* item)
{
    // Utiliser le contrôleur pour sélectionner le lieu
    _searchController->selectPlace(item->text());
}

void MainWindow::onMousePositionChanged(double lon, double lat)
{
    // Formater les coordonnées avec 6 décimales
    QString coordsText = QString("Lon: %1°, Lat: %2°")
                             .arg(lon, 0, 'f', 6)
                             .arg(lat, 0, 'f', 6);
    
    // Mettre à jour le label dans la barre de statut
    _coordsLabel->setText(coordsText);
}
