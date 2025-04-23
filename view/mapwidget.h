// mapwidget.h (adapt�)
#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include "model/mapmodel.h"
#include "controller/mapcontroller.h"
#include <QNetworkAccessManager>
#include <QWidget>

class QNetworkReply;
class QPaintEvent;
class QResizeEvent;
class QMouseEvent;
class QWheelEvent;

/**
 * @class MapWidget
 * @brief Widget personnalis� pour l'affichage de tuiles cartographiques.
 *
 * Cette classe g�re le t�l�chargement et l'affichage d'une carte compos�e de
 * tuiles cartographiques OpenStreetMap.
 */
class MapWidget : public QWidget {
    Q_OBJECT

private:
    MapModel* _mapModel; ///< Mod�le de donn�es pour la carte
    MapController* _mapController; ///< Contr�leur pour les interactions avec la carte
    
    QVector<QPixmap> _tiles; ///< Tuiles � afficher
    QVector<QPoint> _tilePositions; ///< Positions des tuiles sur la carte
    QNetworkAccessManager _networkManager; ///< Gestionnaire de r�seau pour t�l�charger les tuiles
    int _pendingRequests; ///< Nombre de requ�tes en attente
    QPoint _lastMousePos; ///< Derni�re position de la souris pour le d�placement
    bool _isDragging; ///< Indique si la carte est en train d'�tre d�plac�e
    QPixmap _cachedView; ///< Vue mise en cache pour le glissement rapide
    QPoint _dragOffset; ///< D�calage actuel pendant le glissement
    bool _needFullRefresh; ///< Indique si un fullRefresh est n�cessaire

public:
    /**
     * @brief Constructeur du widget d'affichage de carte.
     * @param mapModel Mod�le de donn�es pour la carte
     * @param mapController Contr�leur pour les interactions avec la carte
     * @param parent Widget parent
     */
    MapWidget(MapModel* mapModel, MapController* mapController, QWidget* parent = nullptr);

public slots:
    /**
     * @brief Slot appel� lorsque le centre de la carte change.
     */
    void onCenterChanged();

    /**
     * @brief Slot appel� lorsque le niveau de zoom change.
     */
    void onZoomChanged();

private:
    /**
     * @brief Charge les tuiles n�cessaires pour afficher la carte.
     */
    void loadTiles();

    /**
     * @brief Pr�pare l'affichage complet de la carte.
     */
    void renderFullView();

    /**
     * @brief Convertit des coordonn�es g�ographiques en coordonn�es de tuile.
     * @param lon Longitude
     * @param lat Latitude
     * @param zoom Niveau de zoom
     * @return Coordonn�es de la tuile (x, y)
     */
    QPoint lonLatToTile(double lon, double lat, int zoom);

    /**
     * @brief Convertit des coordonn�es g�ographiques en coordonn�es de tuile (version flottante).
     * @param lon Longitude
     * @param lat Latitude
     * @param zoom Niveau de zoom
     * @return Coordonn�es de la tuile (x, y) en flottant
     */
    QPointF lonLatToTileF(double lon, double lat, int zoom);

    /**
     * @brief Convertit des coordonn�es de tuile en coordonn�es g�ographiques.
     * @param x Coordonn�e X de la tuile
     * @param y Coordonn�e Y de la tuile
     * @param zoom Niveau de zoom
     * @return Coordonn�es g�ographiques (longitude, latitude)
     */
    QPair<double, double> tileToLonLat(int x, int y, int zoom);

    /**
     * @brief T�l�charge une tuile depuis le serveur OpenStreetMap.
     * @param x Coordonn�e X de la tuile
     * @param y Coordonn�e Y de la tuile
     * @param zoom Niveau de zoom
     */
    void downloadTile(int x, int y, int zoom);

    /**
     * @brief Construit le chemin du fichier local pour une tuile.
     * @param x Coordonn�e X de la tuile
     * @param y Coordonn�e Y de la tuile
     * @param zoom Niveau de zoom
     * @return Chemin du fichier local
     */
    QString tileFilePath(int x, int y, int zoom);

protected:
    /**
     * @brief G�re l'�v�nement de dessin du widget.
     * @param event �v�nement de dessin
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief G�re l'�v�nement de redimensionnement du widget.
     * @param event �v�nement de redimensionnement
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * @brief G�re l'�v�nement de clic de souris pour permettre le d�placement de la carte.
     * @param event �v�nement de clic de souris
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief G�re l'�v�nement de d�placement de souris pour permettre le d�placement de la carte.
     * @param event �v�nement de d�placement de souris
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief G�re l'�v�nement de rel�chement de souris pour terminer le d�placement de la carte.
     * @param event �v�nement de rel�chement de souris
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /**
     * @brief G�re l'�v�nement de la molette de souris pour permettre le zoom.
     * @param event �v�nement de la molette de souris
     */
    void wheelEvent(QWheelEvent* event) override;

private slots:
    /**
     * @brief Slot appel� lorsqu'une tuile a �t� t�l�charg�e.
     * @param reply R�ponse du serveur contenant la tuile
     */
    void onTileDownloaded(QNetworkReply* reply);
};

#endif // MAPWIDGET_H

