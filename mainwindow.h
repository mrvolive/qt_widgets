#include <QMainWindow>
#include <QScopedPointer>
class QMenu;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QListWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    // All pointers managed by the main window
    QMenu *_file_menu;
    QMenu *_help_menu;
    QScopedPointer<QGroupBox> _main_widget;
    QScopedPointer<QPushButton> _button;
    QScopedPointer<QLineEdit> _text_edit;
    QScopedPointer<QListWidget> _list;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
