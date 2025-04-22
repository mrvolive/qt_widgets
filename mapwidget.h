#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QNetworkAccessManager>
#include <QWidget>

class QDebug;
class QDir;
class QFile;
class QFileInfo;
class QMouseEvent;
class QNetworkReply;
class QNetworkRequest;
class QPaintEvent;
class QPainter;
class QPixmap;
class QRegExp;
class QResizeEvent;
class QStandardPaths;
class QUrl;
class QWheelEvent;
class cmath;

/**
 * @class MapWidget
 * @brief Widget personnalisé pour l'affichage de tuiles cartographiques.
 *
 * Cette classe gère le téléchargement et l'affichage d'une carte composée de
 * tuiles cartographiques OpenStreetMap centrée sur des coordonnées
 * géographiques spécifiques.
 */
class MapWidget : public QWidget {
  Q_OBJECT

private:
  int _zoom;               ///< Niveau de zoom de la carte (entre 5 et 15)
  double _centerLon;       ///< Longitude du centre de la carte
  double _centerLat;       ///< Latitude du centre de la carte
  QVector<QPixmap> _tiles; ///< Tuiles à afficher
  QVector<QPoint> _tilePositions; ///< Positions des tuiles sur la carte
  QNetworkAccessManager
      _networkManager;   ///< Gestionnaire de réseau pour télécharger les tuiles
  int _pendingRequests;  ///< Nombre de requêtes en attente
  QPoint _lastMousePos;  ///< Dernière position de la souris pour le déplacement
  bool _isDragging;      ///< Indique si la carte est en train d'être déplacée
  QPixmap _cachedView;   ///< Vue mise en cache pour le glissement rapide
  QPoint _dragOffset;    ///< Décalage actuel pendant le glissement
  bool _needFullRefresh; ///< Indique si un fullRefresh est nécessaire

public:
  /**
   * @brief Constructeur du widget d'affichage de carte.
   * @param parent Widget parent
   */
  MapWidget(QWidget *parent = nullptr);

  /**
   * @brief Définit le centre de la carte.
   * @param lon Longitude du centre
   * @param lat Latitude du centre
   */
  void setCenter(double lon, double lat);

  /**
   * @brief Définit le niveau de zoom de la carte.
   * @param zoom Niveau de zoom (entre 5 et 15)
   */
  void setZoom(int zoom);

  /**
   * @brief Charge les tuiles nécessaires pour afficher la carte.
   */
  void loadTiles();

  void renderFullView();

protected:
  /**
   * @brief Gère l'événement de dessin du widget.
   * @param event Événement de dessin
   */
  void paintEvent(QPaintEvent *event) override;

  /**
   * @brief Gère l'événement de redimensionnement du widget.
   * @param event Événement de redimensionnement
   */
  void resizeEvent(QResizeEvent *event) override;

  /**
   * @brief Gère l'événement de clic de souris pour permettre le déplacement de
   * la carte.
   * @param event Événement de clic de souris
   */
  void mousePressEvent(QMouseEvent *event) override;

  /**
   * @brief Gère l'événement de déplacement de souris pour permettre le
   * déplacement de la carte.
   * @param event Événement de déplacement de souris
   */
  void mouseMoveEvent(QMouseEvent *event) override;

  /**
   * @brief Gère l'événement de relâchement de souris pour terminer le
   * déplacement de la carte.
   * @param event Événement de relâchement de souris
   */
  void mouseReleaseEvent(QMouseEvent *event) override;

  /**
   * @brief Gère l'événement de la molette de souris pour permettre le zoom.
   * @param event Événement de la molette de souris
   */
  void wheelEvent(QWheelEvent *event) override;

private:

  /**
   * @brief Convertit des coordonnées géographiques en coordonnées de tuile.
   * @param lon Longitude
   * @param lat Latitude
   * @param zoom Niveau de zoom
   * @return Coordonnées de la tuile (x, y)
   */
  QPoint lonLatToTile(double lon, double lat, int zoom);

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

private slots:
  /**
   * @brief Slot appelé lorsqu'une tuile a été téléchargée.
   * @param reply Réponse du serveur contenant la tuile
   */
  void onTileDownloaded(QNetworkReply *reply);
};

#endif // MAPWIDGET_H
