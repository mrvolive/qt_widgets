// mapwidget.h
#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include "controller/mapcontroller.h"
#include "model/mapmodel.h"
#include <QNetworkAccessManager>
#include <QWidget>

class QNetworkReply;
class QPaintEvent;
class QResizeEvent;
class QMouseEvent;
class QWheelEvent;

/**
 * @class MapWidget
 * @brief Widget personnalisé pour l'affichage de tuiles cartographiques.
 *
 * Cette classe gère le téléchargement et l'affichage d'une carte composée de
 * tuiles cartographiques OpenStreetMap.
 */
class MapWidget : public QWidget {
    Q_OBJECT

private:
    MapModel* _mapModel; ///< Modèle de données pour la carte
    MapController* _mapController; ///< Contrôleur pour les interactions avec la carte

    QVector<QPixmap> _tiles; ///< Tuiles à afficher
    QVector<QPoint> _tilePositions; ///< Positions des tuiles sur la carte
    QNetworkAccessManager _networkManager; ///< Gestionnaire de réseau pour télécharger les tuiles
    int _pendingRequests; ///< Nombre de requêtes en attente
    QPoint _lastMousePos; ///< Dernière position de la souris pour le déplacement
    bool _isDragging; ///< Indique si la carte est en train d'être déplacée
    QPixmap _cachedView; ///< Vue mise en cache pour le glissement rapide
    QPoint _dragOffset; ///< Décalage actuel pendant le glissement
    bool _needFullRefresh; ///< Indique si un fullRefresh est nécessaire

protected:
    /**
     * @brief Gère l'événement de double-clic pour zoomer sur un point.
     * @param event Événement de double-clic
     */
    void mouseDoubleClickEvent(QMouseEvent* event) override;

public:
    /**
     * @brief Constructeur du widget d'affichage de carte.
     * @param mapModel Modèle de données pour la carte
     * @param mapController Contrôleur pour les interactions avec la carte
     * @param parent Widget parent
     */
    MapWidget(MapModel* mapModel, MapController* mapController, QWidget* parent = nullptr);

    /**
     * @brief Convertit des coordonnées écran en coordonnées géographiques.
     * @param screenPos Position sur l'écran
     * @return Coordonnées géographiques (longitude, latitude)
     */
    QPair<double, double> screenToLonLat(const QPoint& screenPos);

signals:
    /**
     * @brief Signal émis lorsque la position de la souris change sur la carte.
     * @param lon Longitude
     * @param lat Latitude
     */
    void mousePositionChanged(double lon, double lat);

public slots:
    /**
     * @brief Slot appelé lorsque le centre de la carte change.
     */
    void onCenterChanged();

    /**
     * @brief Slot appelé lorsque le niveau de zoom change.
     */
    void onZoomChanged();

private:
    /**
     * @brief Charge les tuiles nécessaires pour afficher la carte.
     */
    void loadTiles();

    /**
     * @brief Prépare l'affichage complet de la carte.
     */
    void renderFullView();

    /**
     * @brief Convertit des coordonnées géographiques en coordonnées de tuile.
     * @param lon Longitude
     * @param lat Latitude
     * @param zoom Niveau de zoom
     * @return Coordonnées de la tuile (x, y)
     */
    QPoint lonLatToTile(double lon, double lat, int zoom);

    /**
     * @brief Convertit des coordonnées géographiques en coordonnées de tuile (version flottante).
     * @param lon Longitude
     * @param lat Latitude
     * @param zoom Niveau de zoom
     * @return Coordonnées de la tuile (x, y) en flottant
     */
    QPointF lonLatToTileF(double lon, double lat, int zoom);

    /**
     * @brief Convertit des coordonnées de tuile en coordonnées géographiques.
     * @param x Coordonnée X de la tuile
     * @param y Coordonnée Y de la tuile
     * @param zoom Niveau de zoom
     * @return Coordonnées géographiques (longitude, latitude)
     */
    QPair<double, double> tileToLonLat(int x, int y, int zoom);

    /**
     * @brief Télécharge une tuile depuis le serveur OpenStreetMap.
     * @param x Coordonnée X de la tuile
     * @param y Coordonnée Y de la tuile
     * @param zoom Niveau de zoom
     */
    void downloadTile(int x, int y, int zoom);

    /**
     * @brief Construit le chemin du fichier local pour une tuile.
     * @param x Coordonnée X de la tuile
     * @param y Coordonnée Y de la tuile
     * @param zoom Niveau de zoom
     * @return Chemin du fichier local
     */
    QString tileFilePath(int x, int y, int zoom);

protected:
    /**
     * @brief Gère l'événement de dessin du widget.
     * @param event Événement de dessin
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief Gère l'événement de redimensionnement du widget.
     * @param event Événement de redimensionnement
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * @brief Gère l'événement de clic de souris pour permettre le déplacement de la carte.
     * @param event Événement de clic de souris
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief Gère l'événement de déplacement de souris pour permettre le déplacement de la carte.
     * @param event Événement de déplacement de souris
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief Gère l'événement de relâchement de souris pour terminer le déplacement de la carte.
     * @param event Événement de relâchement de souris
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /**
     * @brief Gère l'événement de la molette de souris pour permettre le zoom.
     * @param event Événement de la molette de souris
     */
    void wheelEvent(QWheelEvent* event) override;

private slots:
    /**
     * @brief Slot appelé lorsqu'une tuile a été téléchargée.
     * @param reply Réponse du serveur contenant la tuile
     */
    void onTileDownloaded(QNetworkReply* reply);
};

#endif // MAPWIDGET_H
