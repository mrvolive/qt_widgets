// mainwindow.h (adapté)
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**
 * @file mainwindow.h
 * @brief Définition de la classe MainWindow qui gère la fenêtre principale de l'application.
 */
#include <QMainWindow>
#include <QScopedPointer>

// Déclarations anticipées
class QGroupBox;
class QPushButton;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QMenu;
class QAction;

class MapWidget;
class PlaceModel;
class MapModel;
class SearchController;
class MapController;

/**
 * @class MainWindow
 * @brief Classe principale de l'interface utilisateur.
 *
 * Cette classe gère la fenêtre principale de l'application, comprenant
 * les menus, les widgets et les layouts. Elle affiche une liste de villes
 * et une carte du monde.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    // Menus
    QMenu* _file_menu; ///< Menu Fichier
    QMenu* _help_menu; ///< Menu Aide

    // Actions
    QAction* _pref_action; ///< Action pour l'item de menu Préférences
    QAction* _quit_action; ///< Action pour l'item de menu Quit
    QAction* _manual_action; ///< Action pour l'item de menu Manual
    QAction* _about_action; ///< Action pour l'item de menu About

    // Widgets
    QScopedPointer<QGroupBox> _main_widget; ///< Widget principal contenant tout
    QScopedPointer<QPushButton> _button; ///< Bouton "Search"
    QScopedPointer<QLineEdit> _text_edit; ///< Champ de texte éditable
    QScopedPointer<QListWidget> _list; ///< Liste des lieux
    QScopedPointer<MapWidget> _map_widget; ///< Widget affichant la carte

    // Modèles et contrôleurs
    QScopedPointer<PlaceModel> _placeModel; ///< Modèle de données pour les lieux
    QScopedPointer<MapModel> _mapModel; ///< Modèle de données pour la carte
    QScopedPointer<SearchController> _searchController; ///< Contrôleur pour la recherche
    QScopedPointer<MapController> _mapController; ///< Contrôleur pour la carte

private:
    /**
     * @brief Configure l'interface utilisateur complète.
     */
    void setupUi();

    /**
     * @brief Crée et configure les menus de l'application.
     */
    void createMenus();

    /**
     * @brief Crée et initialise tous les widgets de l'interface.
     */
    void createWidgets();

    /**
     * @brief Configure les layouts pour organiser les widgets.
     */
    void setupLayouts();

    /**
     * @brief Connecte les signaux aux slots appropriés.
     */
    void connectSignalsSlots();

private slots:
    /**
     * @brief Slot appelé lorsque l'utilisateur clique sur "Quitter".
     */
    void onQuitTriggered();

    /**
     * @brief Slot appelé lorsque l'utilisateur clique sur "Préférences".
     */
    void onPreferencesTriggered();

    /**
     * @brief Slot appelé lorsque l'utilisateur clique sur "Manuel".
     */
    void onManualTriggered();

    /**
     * @brief Slot appelé lorsque l'utilisateur clique sur "À propos".
     */
    void onAboutTriggered();

    /**
     * @brief Slot appelé lorsque l'utilisateur clique sur le bouton Search.
     */
    void onSearchButtonClicked();

    /**
     * @brief Slot appelé lorsque la liste des lieux est mise à jour.
     * @param placeNames Liste des noms de lieux
     */
    void onPlacesUpdated(const QStringList& placeNames);

    /**
     * @brief Slot appelé lorsqu'une erreur survient lors de la recherche.
     * @param errorMessage Message d'erreur
     */
    void onSearchError(const QString& errorMessage);

    /**
     * @brief Slot appelé lorsque l'utilisateur sélectionne un lieu dans la liste.
     * @param item Élément sélectionné
     */
    void onListItemSelected(QListWidgetItem* item);

public:
    /**
     * @brief Constructeur de la classe MainWindow.
     * @param parent Widget parent (nullptr par défaut).
     */
    MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Destructeur de la classe MainWindow.
     */
    ~MainWindow();
};
#endif // MAINWINDOW_H
