#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>
#include <QRectF>

namespace nav {

class MapView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MapView(QWidget* parent = nullptr);

    // Zoom to fit all content
    void zoomToFit();

    // Update LOD visibility based on current zoom
    void updateLOD();

    // Get current zoom level
    double getCurrentZoom() const { return currentZoom_; }

    // Focus on a specific point with a reasonable zoom level
    void focusOnPoint(double x, double y, double zoomLevel = 2.0);

    // Focus on a bounding rectangle with padding
    void focusOnBounds(const QRectF& bounds, double padding = 50.0);

protected:
    // Override wheel event for zoom centered on cursor
    void wheelEvent(QWheelEvent* event) override;

private:
    static constexpr double ZOOM_FACTOR = 1.15;
    static constexpr double MIN_ZOOM = 0.01;
    static constexpr double MAX_ZOOM = 100.0;
    static constexpr double LOD_THRESHOLD = 0.5;  // Hide nodes below this zoom level

    double currentZoom_ = 1.0;
};

} // namespace nav

#endif // MAPVIEW_H
