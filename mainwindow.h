/**
 * @file mainwindow.h
 * @brief Définition de la classe MainWindow qui gère la fenêtre principale de
 * l'application.
 */

#include "qobject.h"
#include <QMainWindow>
#include <QScopedPointer>

class QApplication;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QListWidget;
class QMenu;
class QMenuBar;
class QPixmap;
class QPushButton;
class QStringList;
class QVBoxLayout;
class QWidget;
class QMessageBox;

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
  QMenu *_file_menu; ///< Menu Fichier
  QMenu *_help_menu; ///< Menu Aide
  
  // Actions
  QAction *_pref_action; ///< Action pour l'item de menu Préférences
  QAction *_quit_action; ///< Action pour l'item de menu Quit
  QAction *_manual_action; ///< Action pour l'item de menu Manual
  QAction *_about_action; ///< Action pour l'item de menu About

  // Widgets
  QScopedPointer<QGroupBox> _main_widget; ///< Widget principal contenant tout
  QScopedPointer<QPushButton> _button;    ///< Bouton "Search"
  QScopedPointer<QLineEdit> _text_edit;   ///< Champ de texte éditable
  QScopedPointer<QListWidget> _list;      ///< Liste des villes
  QScopedPointer<QLabel> _map_widget; ///< Widget affichant la carte du monde

private:
  /**
   * @brief Configure l'interface utilisateur complète.
   *
   * Cette méthode initialise tous les composants de l'interface utilisateur
   * en appelant les méthodes spécialisées.
   */
  void setupUi();

  /**
   * @brief Crée et configure les menus de l'application.
   *
   * Initialise les menus Fichier et Aide avec leurs actions respectives.
   */
  void createMenus();

  /**
   * @brief Crée et initialise tous les widgets de l'interface.
   *
   * Cette méthode instancie le bouton, le champ de texte, la liste des villes
   * et le widget de carte.
   */
  void createWidgets();

  /**
   * @brief Configure les layouts pour organiser les widgets.
   *
   * Met en place un layout horizontal principal avec une partie gauche
   * (contrôles) et une partie droite (carte du monde).
   */
  void setupLayouts();

  /**
   * @brief Connecte les signaux aux slot appropriés.
   *
   * Cette méthode établit les connexions entre les signaux émis par les widgets
   * et les slots qui traitent ces signaux.
   */
  void connectSignalsSlots();

private slots:
  /**
     * @brief Slot appelé lorsque l'utilisateur clique sur "Quitter".
     * 
     * Ferme l'application proprement.
     */
    void onQuitTriggered();
    
    /**
     * @brief Slot appelé lorsque l'utilisateur clique sur "Préférences".
     * 
     * Affiche la boîte de dialogue des préférences.
     */
    void onPreferencesTriggered();
    
    /**
     * @brief Slot appelé lorsque l'utilisateur clique sur "Manuel".
     * 
     * Affiche le manuel d'utilisation.
     */
    void onManualTriggered();
    
    /**
     * @brief Slot appelé lorsque l'utilisateur clique sur "À propos".
     * 
     * Affiche la boîte de dialogue "À propos".
     */
    void onAboutTriggered();

public:
  /**
   * @brief Constructeur de la classe MainWindow.
   * @param parent Widget parent (nullptr par défaut).
   *
   * Initialise la fenêtre principale et configure l'interface utilisateur.
   */
  MainWindow(QWidget *parent = nullptr);

  /**
   * @brief Destructeur de la classe MainWindow.
   *
   * Libère les ressources allouées. La plupart des ressources sont gérées
   * automatiquement par les QScopedPointer et le système de parenté de Qt.
   */
  ~MainWindow();
};

