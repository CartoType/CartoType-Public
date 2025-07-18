#pragma once

#include <QOpenGLWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QScreen>
#include <QWindow>

#include "mainwindow.h"
#include "finddialog.h"
#include "cartotype_qt_map_renderer.h"

namespace Ui
    {
    class MapForm;
    }

class MapForm: public QOpenGLWidget
    {
    Q_OBJECT

    public:
    explicit MapForm(QWidget* aParent,MainWindow& aMainWindow,const std::vector<QString>& aMapFileNameArray);
    ~MapForm();

    const QString& FileName(size_t aIndex) const { return m_filename_array[aIndex]; }
    size_t FileNameCount() const { return m_filename_array.size(); }
    QString ViewState() const;
    void SetView(const CartoType::ViewState& aViewState);
    bool Valid() const { return m_framework != nullptr; }
    void LoadMap(const QString& aFileName);
    void Print(bool aPreview);
    void EnableDrawLegend(bool aEnable);
    void EnableDrawScale(bool aEnable);
    bool DrawLegendEnabled() const { return m_draw_legend; }
    bool DrawScaleEnabled() const { return m_draw_scale; }
    void EnableDrawRotator(bool aEnable);
    bool DrawRotatorEnabled() const { return m_draw_rotator; }
    void SetRotation(double aAngle);
    double Rotation() const { return m_framework->Rotation(); }
    void EnableDrawRange(bool aEnable);
    bool DrawRangeEnabled() const { return m_draw_range; }
    void Find();
    void FindAddress();
    size_t FoundItemCount() const { return m_found_object.size(); }
    void FindNext();
    void ReverseRoute();
    void DeleteRoute();
    void DeletePushpins();
    bool HasRoute() const { return m_framework->Route() != nullptr; }
    void SetSimulateRouting(bool aSet);
    bool RoutingIsSimulated() const { return m_simulate_routing; }
    bool HasPushpins() const;
    size_t BuiltInProfileCount();
    const CartoType::RouteProfile* BuiltInProfile(size_t aIndex);
    void SetRouteProfileIndex(size_t aIndex);
    size_t RouteProfileIndex() const { return m_route_profile_index; }
    void SetPreferredRouterType(CartoType::RouterType aRouterType);
    CartoType::RouterType PreferredRouterType() const { return m_framework->PreferredRouterType(); }
    bool EditCustomRouteProfile(CartoType::RouteProfile& aRouteProfile);
    void OnCustomRouteProfileLoaded();
    void ViewRouteInstructions();
    void SaveRouteInstructions();
    void SaveRouteAsXml();
    void SaveRouteAsGpx();
    void LoadRouteFromXml();
    void SaveImageAsPng();
    bool HasWritableData() const;
    bool WritableDataChanged() const { return m_writable_map_changed; }
    void SaveWritableDataAsCtms();
    void LoadWritableDataFromCtms();
    void LoadWritableDataFromGpx();
    void SetScale();
    void GoToLocation();
    void GoToOrdnanceSurveyGridRef();
    void ChooseStyleSheet();
    void ReloadStyleSheet();
    void ChooseLayers();
    void SetMetricUnits(bool aEnable);
    bool MetricUnits() const { return m_framework->MetricUnits(); }
    void SetNightMode(bool aSet);
    bool NightMode() const;
    void SetGraphicsAcceleration(bool aEnable);
    bool GraphicsAcceleration() const { return m_graphics_acceleration; }
    void SetPerspective(bool aEnable);
    bool Perspective() const;
    void EnableDebugLayers(bool aEnable);
    bool DebugLayers() const;
    void EnableFixedLabels(bool aEnable);
    bool FixedLabels() const;
    void SetDraw3DBuildings(bool aEnable);
    bool Draw3DBuildings() const;
    bool MapIncludesGreatBritain() const;
    void SetUseCustomStyleSheet(bool aEnable);
    bool UsingCustomStyleSheet() const { return m_use_custom_style_sheet; }
    void OnCustomStyleSheetLoaded();
    void FinishEditingCustomStyleSheet(int aResult);
    CartoType::Framework& Framework() { return *m_framework; }

    public slots:
    void ApplyStyleSheet(const std::string& aStyleSheet);

    private slots:
    void on_dial_valueChanged(int aValue);
    void on_perspective_slider_valueChanged(int aValue);

    private:
    // from QWidget
    void resizeEvent(QResizeEvent* aEvent) override;
    void paintEvent(QPaintEvent* aEvent) override;
    void mousePressEvent(QMouseEvent* aEvent) override;
    void mouseDoubleClickEvent(QMouseEvent* aEvent) override;
    void mouseReleaseEvent(QMouseEvent* aEvent) override;
    void mouseMoveEvent(QMouseEvent* aEvent) override;
    void wheelEvent(QWheelEvent* aEvent) override;
    void keyPressEvent(QKeyEvent* aEvent) override;
    void closeEvent(QCloseEvent* aEvent) override;
    bool event(QEvent* aEvent) override;

    // from QOpenGLWidget
    void initializeGL() override;
    void paintGL() override;

    void LoadFont(const char* aFileName);
    void StopDragging();
    void PanToDraggedPosition();
    void DrawRange();
    const CartoType::BitmapView* MapBitmap(CartoType::Result& aError);
    void CreateLegend();
    void CreateTurnInstructions();
    void LeftButtonDown(int32_t aX,int32_t aY);
    void LeftButtonUp(int32_t aX,int32_t aY,bool aShift);
    void RightButtonDown(int32_t aX,int32_t aY);
    void RightButtonUp(int32_t aX,int32_t aY);
    void ShowRouteDialog(int32_t aX,int32_t aY);
    void HandleEditRightButtonUp(int32_t aX,int32_t aY);
    void CalculateAndDisplayRoute();
    void ShowNextFoundObject();
    void GoToLocation(double aLong,double aLat);
    void SetEditMode(bool aSet);
    void ShowEditedObjectSize();
    void ScreenChanged();
    void TestCode();

    Ui::MapForm* m_ui;
    MainWindow& m_main_window;
    std::vector<QString> m_filename_array;
    std::unique_ptr<CartoType::Framework> m_framework;
    std::unique_ptr<CartoType::CQtMapRenderer> m_map_renderer;
    std::unique_ptr<CartoType::GraphicsContext> m_extra_gc;
    uint32_t m_writable_map_handle = 0;
    bool m_writable_map_changed = false;
    class RoutePoint
        {
        public:
        CartoType::RoutePoint Point;
        uint64_t Id = 0;
        };
    std::vector<RoutePoint> m_route_point_array;
    size_t m_route_profile_index = 0;
    std::unique_ptr<QImage> m_map_image;
    std::unique_ptr<QImage> m_legend_image;
    CartoType::MapObjectArray m_found_object;
    FindDialog m_find_dialog;
    CartoType::String m_find_text;
    CartoType::Address m_address;
    int m_found_object_index = 0;
    uint64_t m_found_object_id = 0;                 // map object ID of found object
    bool m_draw_legend = false;                     // if true draw the legend
    bool m_draw_scale = false;                      // if true draw the scale
    bool m_draw_rotator = true;                     // if true draw the rotator dial
    bool m_draw_range = false;                      // if true draw the range from the last point right-clicked
    uint64_t m_range_id0 = 0;
    uint64_t m_range_id1 = 0;
    bool m_map_drag_enabled = false;
    CartoType::Point m_map_drag_anchor;
    CartoType::PointFP m_map_drag_anchor_in_map_coords;
    CartoType::Point m_map_drag_offset;
    double m_on_route_time = 0;                     // simulated time in seconds on the route
    bool m_best_route = false;                      // calculate the best route through the waypoints; don't just take them in order

    std::mutex m_new_route_mutex;
    std::atomic<bool> m_new_route_may_exist;
    std::shared_ptr<CartoType::Route> m_new_route;
    CartoType::Result m_new_route_error;

    QString m_pushpin_name;
    QString m_pushpin_desc;
    CartoType::String m_pushpin_color = "ncsred";
    uint16_t m_pushpin_glyph = 0;
    bool m_use_custom_style_sheet = false;
    CartoType::StyleSheetData m_saved_style_sheet; // a copy of the ordinary style sheet kept while the custom style sheet is in use
    bool m_style_sheet_applied = false;
    bool m_graphics_acceleration = false;           // true if rendering is to be done using OpenGL ES
    QTimer m_graphics_acceleration_update_timer;
    bool m_simulate_routing = false;
    bool m_edit_mode = false;
    bool m_debug_layers_enabled = false;
    bool m_fixed_labels = false;
    double m_device_pixel_ratio = 1;                // factor by which to multiply display coords to get physical pixels
    };
