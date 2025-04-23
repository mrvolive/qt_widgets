// mainwindow.h (adapt�)
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**
 * @file mainwindow.h
 * @brief D�finition de la classe MainWindow qui g�re la fen�tre principale de l'application.
 */
#include <QMainWindow>
#include <QScopedPointer>

// D�clarations anticip�es
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
 * Cette classe g�re la fen�tre principale de l'application, comprenant
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
    QAction* _pref_action; ///< Action pour l'item de menu Pr�f�rences
    QAction* _quit_action; ///< Action pour l'item de menu Quit
    QAction* _manual_action; ///< Action pour l'item de menu Manual
    QAction* _about_action; ///< Action pour l'item de menu About

    // Widgets
    QScopedPointer<QGroupBox> _main_widget; ///< Widget principal contenant tout
    QScopedPointer<QPushButton> _button; ///< Bouton "Search"
    QScopedPointer<QLineEdit> _text_edit; ///< Champ de texte �ditable
    QScopedPointer<QListWidget> _list; ///< Liste des lieux
    QScopedPointer<MapWidget> _map_widget; ///< Widget affichant la carte

    // Mod�les et contr�leurs
    QScopedPointer<PlaceModel> _placeModel; ///< Mod�le de donn�es pour les lieux
    QScopedPointer<MapModel> _mapModel; ///< Mod�le de donn�es pour la carte
    QScopedPointer<SearchController> _searchController; ///< Contr�leur pour la recherche
    QScopedPointer<MapController> _mapController; ///< Contr�leur pour la carte

private:
    /**
     * @brief Configure l'interface utilisateur compl�te.
     */
    void setupUi();

    /**
     * @brief Cr�e et configure les menus de l'application.
     */
    void createMenus();

    /**
     * @brief Cr�e et initialise tous les widgets de l'interface.
     */
    void createWidgets();

    /**
     * @brief Configure les layouts pour organiser les widgets.
     */
    void setupLayouts();

    /**
     * @brief Connecte les signaux aux slots appropri�s.
     */
    void connectSignalsSlots();

private slots:
    /**
     * @brief Slot appel� lorsque l'utilisateur clique sur "Quitter".
     */
    void onQuitTriggered();

    /**
     * @brief Slot appel� lorsque l'utilisateur clique sur "Pr�f�rences".
     */
    void onPreferencesTriggered();

    /**
     * @brief Slot appel� lorsque l'utilisateur clique sur "Manuel".
     */
    void onManualTriggered();

    /**
     * @brief Slot appel� lorsque l'utilisateur clique sur "� propos".
     */
    void onAboutTriggered();

    /**
     * @brief Slot appel� lorsque l'utilisateur clique sur le bouton Search.
     */
    void onSearchButtonClicked();

    /**
     * @brief Slot appel� lorsque la liste des lieux est mise � jour.
     * @param placeNames Liste des noms de lieux
     */
    void onPlacesUpdated(const QStringList& placeNames);

    /**
     * @brief Slot appel� lorsqu'une erreur survient lors de la recherche.
     * @param errorMessage Message d'erreur
     */
    void onSearchError(const QString& errorMessage);

    /**
     * @brief Slot appel� lorsque l'utilisateur s�lectionne un lieu dans la liste.
     * @param item �l�ment s�lectionn�
     */
    void onListItemSelected(QListWidgetItem* item);

public:
    /**
     * @brief Constructeur de la classe MainWindow.
     * @param parent Widget parent (nullptr par d�faut).
     */
    MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Destructeur de la classe MainWindow.
     */
    ~MainWindow();
};
#endif // MAINWINDOW_H

