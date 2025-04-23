#include "mapwidget.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMouseEvent>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QRegExp>
#include <QResizeEvent>
#include <QStandardPaths>
#include <QUrl>
#include <QVector>
#include <QWheelEvent>
#include <cmath>

/**
 * @brief Constructeur de MapWidget.
 *
 * Initialise le widget avec un zoom par défaut de 10 et le centre sur
 * les coordonnées spécifiées.
 *
 * @param parent Widget parent
 */
MapWidget::MapWidget(QWidget* parent)
    : QWidget(parent)
    , _zoom(10)
    , _pendingRequests(0)
    , _isDragging(false)
    , _needFullRefresh(true)
{
    // Définir le centre sur les coordonnées spécifiées (Belfort, France)
    _centerLon = 6.839349;
    _centerLat = 47.64263;

    // Créer le répertoire de cache pour les tuiles
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/osm_tiles";
    QDir().mkpath(cacheDir);

    // Configurer le gestionnaire réseau
    connect(&_networkManager, &QNetworkAccessManager::finished, this,
        &MapWidget::onTileDownloaded);

    loadTiles();
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

/**
 * @brief Définit le centre de la carte.
 *
 * @param lon Longitude du centre
 * @param lat Latitude du centre
 */
void MapWidget::setCenter(double lon, double lat)
{
    _centerLon = lon;
    _centerLat = lat;

    _needFullRefresh = true;
    loadTiles();
    update();
}

void MapWidget::setZoom(int zoom)
{
    // Limiter le zoom entre 5 et 15
    _zoom = qBound(5, zoom, 15);

    _needFullRefresh = true;
    loadTiles();
    update();
}

/**
 * @brief Convertit des coordonnées géographiques en coordonnées de tuile.
 *
 * @param lon Longitude
 * @param lat Latitude
 * @param zoom Niveau de zoom
 * @return Coordonnées de la tuile (x, y)
 */
QPoint MapWidget::lonLatToTile(double lon, double lat, int zoom)
{
    int n = 1 << zoom; // 2^zoom
    int x = static_cast<int>((lon + 180.0) / 360.0 * n);
    double latRad = lat * M_PI / 180.0;
    int y = static_cast<int>((1.0 - log(tan(latRad) + 1.0 / cos(latRad)) / M_PI) / 2.0 * n);
    return QPoint(x, y);
}

QPointF MapWidget::lonLatToTileF(double lon, double lat, int zoom)
{
    int n = 1 << zoom; // 2^zoom
    double x = (lon + 180.0) / 360.0 * n;
    double latRad = lat * M_PI / 180.0;
    double y = (1.0 - log(tan(latRad) + 1.0 / cos(latRad)) / M_PI) / 2.0 * n;
    return QPointF(x, y);
}

/**
 * @brief Convertit des coordonnées de tuile en coordonnées géographiques.
 *
 * @param x Coordonnée X de la tuile
 * @param y Coordonnée Y de la tuile
 * @param zoom Niveau de zoom
 * @return Coordonnées géographiques (longitude, latitude)
 */
QPair<double, double> MapWidget::tileToLonLat(int x, int y, int zoom)
{
    int n = 1 << zoom;
    double lon = x / static_cast<double>(n) * 360.0 - 180.0;
    double latRad = atan(sinh(M_PI * (1 - 2 * y / static_cast<double>(n))));
    double lat = latRad * 180.0 / M_PI;
    return qMakePair(lon, lat);
}

/**
 * @brief Construit le chemin du fichier local pour une tuile.
 *
 * @param x Coordonnée X de la tuile
 * @param y Coordonnée Y de la tuile
 * @param zoom Niveau de zoom
 * @return Chemin du fichier local
 */
QString MapWidget::tileFilePath(int x, int y, int zoom)
{
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/osm_tiles";
    return QString("%1/%2-%3-%4.png").arg(cacheDir).arg(zoom).arg(x).arg(y);
}

/**
 * @brief Télécharge une tuile depuis le serveur OpenStreetMap.
 *
 * @param x Coordonnée X de la tuile
 * @param y Coordonnée Y de la tuile
 * @param zoom Niveau de zoom
 */
void MapWidget::downloadTile(int x, int y, int zoom)
{
    // Vérifier si le fichier existe déjà
    QString filePath = tileFilePath(x, y, zoom);
    QFileInfo fileInfo(filePath);

    if (fileInfo.exists()) {
        // Charger la tuile depuis le fichier local
        QPixmap tile(filePath);
        if (!tile.isNull()) {
            _tiles.append(tile);
            _tilePositions.append(QPoint(x, y));
            update();
            return;
        }
    }

    // Construire l'URL de la tuile
    // Format: https://a.tile.openstreetmap.org/{z}/{x}/{y}.png
    QString urlStr = QString("https://a.tile.openstreetmap.org/%1/%2/%3.png")
                         .arg(zoom)
                         .arg(x)
                         .arg(y);
    QUrl url(urlStr);

    // Créer la requête
    QNetworkRequest request(url);

    // Ajouter un User-Agent pour respecter les conditions d'utilisation
    // d'OpenStreetMap
    request.setHeader(QNetworkRequest::UserAgentHeader, "Qt OSM Map Widget/1.0");

    // Envoyer la requête
    _networkManager.get(request);
    _pendingRequests++;
}

/**
 * @brief Slot appelé lorsqu'une tuile a été téléchargée.
 *
 * @param reply Réponse du serveur contenant la tuile
 */
void MapWidget::onTileDownloaded(QNetworkReply* reply)
{
    // Diminuer le compteur de requêtes en attente
    _pendingRequests--;

    // Vérifier si la requête a réussi
    if (reply->error() == QNetworkReply::NoError) {
        // Extraire les coordonnées de la tuile de l'URL
        QString urlStr = reply->url().toString();
        QRegExp rx("/(\\d+)/(\\d+)/(\\d+)\\.png");
        if (rx.indexIn(urlStr) != -1) {
            int zoom = rx.cap(1).toInt();
            int x = rx.cap(2).toInt();
            int y = rx.cap(3).toInt();

            // Lire les données de l'image
            QByteArray data = reply->readAll();

            // Créer une image à partir des données
            QPixmap tile;
            if (tile.loadFromData(data)) {
                // Sauvegarder la tuile dans le cache
                QString filePath = tileFilePath(x, y, zoom);
                QFile file(filePath);
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(data);
                    file.close();
                }

                // Ajouter la tuile à la liste
                _tiles.append(tile);
                _tilePositions.append(QPoint(x, y));

                // Rafraîchir l'affichage
            }
        }
        update();
    } else {
        qDebug() << "Erreur de téléchargement de tuile:" << reply->errorString();
    }

    // Libérer la mémoire
    reply->deleteLater();
}

/**
 * @brief Charge les tuiles nécessaires pour afficher la carte.
 *
 * Cette méthode calcule quelles tuiles sont nécessaires pour afficher
 * la région autour du centre à un niveau de zoom donné.
 */
void MapWidget::loadTiles()
{
    // Vider les tuiles précédentes
    _tiles.clear();
    _tilePositions.clear();

    // Calculer la tuile centrale avec des coordonnées fractionnaires
    QPointF centralTileF = lonLatToTileF(_centerLon, _centerLat, _zoom);

    // Obtenir la partie entière de la position centrale
    int centralTileX = floor(centralTileF.x());
    int centralTileY = floor(centralTileF.y());

    // Déterminer combien de tuiles sont nécessaires en fonction de la taille du widget
    int tileSize = 256;

    // Facteur pour l'image mise en cache plus grande
    int factor = 4; // Même facteur que dans renderFullView

    // Calculer le nombre de tuiles nécessaires pour couvrir l'image mise en cache
    int tilesX = (width() * factor / tileSize) + 3; // +3 pour couvrir les bords et le décalage fractionnaire
    int tilesY = (height() * factor / tileSize) + 3;

    // S'assurer qu'on a au moins un minimum de tuiles
    tilesX = qMax(5, tilesX);
    tilesY = qMax(5, tilesY);

    // Calculer les tuiles à charger
    int startX = centralTileX - tilesX / 2;
    int startY = centralTileY - tilesY / 2;
    int endX = startX + tilesX;
    int endY = startY + tilesY;

    // Limiter aux tuiles valides pour ce niveau de zoom
    int maxTile = (1 << _zoom) - 1;
    startX = qBound(0, startX, maxTile);
    startY = qBound(0, startY, maxTile);
    endX = qBound(0, endX, maxTile);
    endY = qBound(0, endY, maxTile);

    // Télécharger ou charger les tuiles
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            downloadTile(x, y, _zoom);
        }
    }
}
void MapWidget::renderFullView()
{
    // Créer une image plus grande que la taille du widget
    int factor = 4; // Facteur 2 dans chaque dimension = 4 fois la surface totale
    QSize cacheSize(width() * factor, height() * factor);

    _cachedView = QPixmap(cacheSize);
    _cachedView.fill(QColor(240, 240, 240));

    QPainter painter(&_cachedView);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Calculer la tuile centrale avec des coordonnées fractionnaires
    QPointF centralTileF = lonLatToTileF(_centerLon, _centerLat, _zoom);

    // Taille standard d'une tuile
    const int tileSize = 256;

    // Calculer le décalage pour centrer la tuile centrale
    // Centrer sur l'image élargie
    int centerX = cacheSize.width() / 2;
    int centerY = cacheSize.height() / 2;

    // Calculer le décalage fractionnaire (en pixels) à l'intérieur de la tuile centrale
    double fracX = centralTileF.x() - floor(centralTileF.x());
    double fracY = centralTileF.y() - floor(centralTileF.y());
    int pixelOffsetX = fracX * tileSize;
    int pixelOffsetY = fracY * tileSize;

    // Dessiner toutes les tuiles
    for (int i = 0; i < _tiles.size(); i++) {
        QPoint tilePos = _tilePositions[i];
        QPixmap tile = _tiles[i];

        int x = centerX + (tilePos.x() - centralTileF.x()) * tileSize;
        int y = centerY + (tilePos.y() - centralTileF.y()) * tileSize;

        QRect tileRect(x, y, tileSize, tileSize);
        painter.drawPixmap(tileRect, tile);
    }

    _needFullRefresh = false;
}

/**
 * @brief Gère l'événement de dessin du widget.
 *
 * @param event Événement de dessin
 */
void MapWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    if (_needFullRefresh) {
        renderFullView();
    }

    if (_isDragging) {
        // Pendant le glissement, dessiner l'image mise en cache avec un décalage
        // Calculer l'offset pour centrer l'image
        int factor = 4; // Même facteur que dans renderFullView
        int offsetX = (width() * factor - width()) / 2;
        int offsetY = (height() * factor - height()) / 2;

        // Dessiner la partie appropriée de l'image mise en cache
        painter.drawPixmap(-_dragOffset.x() - offsetX, -_dragOffset.y() - offsetY, _cachedView);
    } else {
        // En mode normal, dessiner la partie centrale de l'image mise en cache
        int factor = 4; // Même facteur que dans renderFullView
        int offsetX = (width() * factor - width()) / 2;
        int offsetY = (height() * factor - height()) / 2;

        painter.drawPixmap(0, 0, width(), height(), _cachedView,
            offsetX, offsetY, width(), height());
    }
}

/**
 * @brief Gère l'événement de redimensionnement du widget.
 *
 * @param event Événement de redimensionnement
 */
void MapWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    _needFullRefresh = true;
    loadTiles();
}

/**
 * @brief Gère l'événement de clic de souris pour permettre le déplacement de la
 * carte.
 *
 * @param event Événement de clic de souris
 */
void MapWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        _isDragging = true;
        _lastMousePos = event->pos();
        _dragOffset = QPoint(0, 0);
        setCursor(Qt::ClosedHandCursor);

        // S'assurer que la vue est à jour avant de commencer le glissement
        if (_needFullRefresh) {
            renderFullView();
            update();
        }
    }
}

/**
 * @brief Gère l'événement de déplacement de souris pour permettre le
 * déplacement de la carte.
 *
 * @param event Événement de déplacement de souris
 */
void MapWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (_isDragging) {
        // Calculer le déplacement en pixels
        QPoint delta = event->pos() - _lastMousePos;

        // Mettre à jour le décalage total
        _dragOffset -= delta;

        // Mettre à jour la dernière position
        _lastMousePos = event->pos();

        // Forcer un rafraîchissement immédiat
        update();
    }
}

/**
 * @brief Gère l'événement de relâchement de souris pour terminer le déplacement
 * de la carte.
 *
 * @param event Événement de relâchement de souris
 */
void MapWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && _isDragging) {
        _isDragging = false;
        setCursor(Qt::ArrowCursor);

        // Calcul précis pour la conversion pixels -> degrés
        int n = 1 << _zoom;
        double worldSize = 256.0 * n;

        // Pour la longitude, c'est une relation linéaire
        double deltaLon = 360.0 * _dragOffset.x() / worldSize;

        // Pour la latitude, c'est plus complexe à cause de la projection Mercator
        double latRad = _centerLat * M_PI / 180.0;
        double factor = 1.0 / cos(latRad); // Facteur de correction pour la latitude
        double deltaLat = -360.0 * _dragOffset.y() / (worldSize * factor);

        _centerLon += deltaLon;
        _centerLat += deltaLat;

        // Limiter la longitude à [-180, 180] et la latitude à [-85, 85]
        _centerLon = qBound(-180.0, _centerLon, 180.0);
        _centerLat = qBound(-85.0, _centerLat, 85.0);

        // Réinitialiser le décalage
        _dragOffset = QPoint(0, 0);

        _needFullRefresh = true;
        loadTiles();
        update();
    }
}

/**
 * @brief Gère l'événement de la molette de souris pour permettre le zoom.
 *
 * @param event Événement de la molette de souris
 */
void MapWidget::wheelEvent(QWheelEvent* event)
{
    // Déterminer la direction du zoom
    int delta = event->angleDelta().y();

    // Changer le niveau de zoom
    if (delta > 0) {
        // Zoom in
        setZoom(_zoom + 1);
    } else if (delta < 0) {
        // Zoom out
        setZoom(_zoom - 1);
    }
}
