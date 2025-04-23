// mapwidget.cpp
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

MapWidget::MapWidget(MapModel* mapModel, MapController* mapController, QWidget* parent)
    : QWidget(parent)
    , _mapModel(mapModel)
    , _mapController(mapController)
    , _pendingRequests(0)
    , _isDragging(false)
    , _needFullRefresh(true)
{
    // Créer le répertoire de cache pour les tuiles
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/osm_tiles";
    QDir().mkpath(cacheDir);

    // Configurer le gestionnaire réseau
    connect(&_networkManager, &QNetworkAccessManager::finished, this,
        &MapWidget::onTileDownloaded);

    // Connecter les signaux du modèle aux slots de la vue
    connect(_mapModel, &MapModel::centerChanged, this, &MapWidget::onCenterChanged);
    connect(_mapModel, &MapModel::zoomChanged, this, &MapWidget::onZoomChanged);

    loadTiles();
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void MapWidget::onCenterChanged()
{
    _needFullRefresh = true;
    loadTiles();
    update();
}

void MapWidget::onZoomChanged()
{
    _needFullRefresh = true;
    loadTiles();
    update();
}

QPair<double, double> MapWidget::screenToLonLat(const QPoint& screenPos)
{
    // Obtenir les données du modèle
    QPointF center = _mapModel->getCenter();
    int zoom = _mapModel->getZoom();

    // Calculer la tuile centrale avec des coordonnées fractionnaires
    QPointF centralTileF = lonLatToTileF(center.x(), center.y(), zoom);

    // Taille standard d'une tuile
    const int tileSize = 256;

    // Position du centre de l'écran
    int centerX = width() / 2;
    int centerY = height() / 2;

    // Calculer le décalage en pixels par rapport au centre
    int pixelDeltaX = screenPos.x() - centerX;
    int pixelDeltaY = screenPos.y() - centerY;

    // Calculer la position en tuiles (fractionnaire)
    double tileDeltaX = pixelDeltaX / static_cast<double>(tileSize);
    double tileDeltaY = pixelDeltaY / static_cast<double>(tileSize);

    // Calculer la tuile sous le curseur
    double tileX = centralTileF.x() + tileDeltaX;
    double tileY = centralTileF.y() + tileDeltaY;

    // Convertir en coordonnées géographiques
    int n = 1 << zoom;
    double lon = tileX / n * 360.0 - 180.0;
    double latRad = atan(sinh(M_PI * (1 - 2 * tileY / n)));
    double lat = latRad * 180.0 / M_PI;

    return qMakePair(lon, lat);
}

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

QPair<double, double> MapWidget::tileToLonLat(int x, int y, int zoom)
{
    int n = 1 << zoom;
    double lon = x / static_cast<double>(n) * 360.0 - 180.0;
    double latRad = atan(sinh(M_PI * (1 - 2 * y / static_cast<double>(n))));
    double lat = latRad * 180.0 / M_PI;
    return qMakePair(lon, lat);
}

QString MapWidget::tileFilePath(int x, int y, int zoom)
{
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/osm_tiles";
    return QString("%1/%2-%3-%4.png").arg(cacheDir).arg(zoom).arg(x).arg(y);
}

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

    // Ajouter un User-Agent pour respecter les conditions d'utilisation d'OpenStreetMap
    request.setHeader(QNetworkRequest::UserAgentHeader, "Qt OSM Map Widget/1.0");

    // Envoyer la requête
    _networkManager.get(request);
    _pendingRequests++;
}

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

void MapWidget::loadTiles()
{
    // Vider les tuiles précédentes
    _tiles.clear();
    _tilePositions.clear();

    // Obtenir les données du modèle
    QPointF center = _mapModel->getCenter();
    int zoom = _mapModel->getZoom();

    // Calculer la tuile centrale avec des coordonnées fractionnaires
    QPointF centralTileF = lonLatToTileF(center.x(), center.y(), zoom);

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
    int maxTile = (1 << zoom) - 1;
    startX = qBound(0, startX, maxTile);
    startY = qBound(0, startY, maxTile);
    endX = qBound(0, endX, maxTile);
    endY = qBound(0, endY, maxTile);

    // Télécharger ou charger les tuiles
    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            downloadTile(x, y, zoom);
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

    // Obtenir les données du modèle
    QPointF center = _mapModel->getCenter();
    int zoom = _mapModel->getZoom();

    // Calculer la tuile centrale avec des coordonnées fractionnaires
    QPointF centralTileF = lonLatToTileF(center.x(), center.y(), zoom);

    // Taille standard d'une tuile
    const int tileSize = 256;

    // Calculer le décalage pour centrer la tuile centrale
    // Centrer sur l'image élargie
    int centerX = cacheSize.width() / 2;
    int centerY = cacheSize.height() / 2;

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

void MapWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

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

void MapWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    _needFullRefresh = true;
    loadTiles();
}

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
    // Émettre le signal avec les coordonnées géographiques sous le curseur
    QPair<double, double> coords = screenToLonLat(event->pos());
    emit mousePositionChanged(coords.first, coords.second);
}

void MapWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && _isDragging) {
        _isDragging = false;
        setCursor(Qt::ArrowCursor);

        // Utiliser le contrôleur pour mettre à jour le modèle
        _mapController->panMap(_dragOffset.x(), _dragOffset.y(), _mapModel->getZoom());

        // Réinitialiser le décalage
        _dragOffset = QPoint(0, 0);
    }
}

void MapWidget::wheelEvent(QWheelEvent* event)
{
    // Utiliser le contrôleur pour gérer le zoom
    _mapController->zoomMap(event->angleDelta().y());
}

void MapWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // Récupérer les coordonnées géographiques du point cliqué
        QPair<double, double> coords = screenToLonLat(event->pos());
        double lon = coords.first;
        double lat = coords.second;

        // Centrer la carte sur ce point
        _mapController->setCenter(lon, lat);

        // Zoomer d'un niveau
        int currentZoom = _mapModel->getZoom();
        _mapController->setZoom(currentZoom + 1);
    }
}
