#include "mapform.h"
#include "ui_mapform.h"
#include "routedialog.h"
#include "ui_routedialog.h"
#include "finddialog.h"
#include "ui_finddialog.h"
#include "findaddressdialog.h"
#include "ui_findaddressdialog.h"
#include "scaledialog.h"
#include "ui_scaledialog.h"
#include "locationdialog.h"
#include "ui_locationdialog.h"
#include "layerdialog.h"
#include "ui_layerdialog.h"
#include "routeinstructionsdialog.h"
#include "ui_routeinstructionsdialog.h"
#include "routeprofiledialog.h"
#include "util.h"

#include <fstream>

#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

constexpr QEvent::Type NEW_ROUTE_EVENT = QEvent::Type::User;

MapForm::MapForm(QWidget* aParent,MainWindow& aMainWindow,const std::vector<QString>& aMapFileNameArray):
    QOpenGLWidget(aParent),
    m_ui(new Ui::MapForm),
    m_main_window(aMainWindow),
    m_find_dialog(this),
    m_new_route_may_exist(false)
    {
    m_device_pixel_ratio = devicePixelRatioF();

    assert(!aMapFileNameArray.empty());
    m_filename_array.push_back(aMapFileNameArray[0]);
    m_ui->setupUi(this);

    m_route_point_array.resize(2);
    m_route_point_array[0] = RoutePoint();
    m_route_point_array[1] = RoutePoint();

    CartoType::String filename;
    std::string api_key;
    // Split a comma-separated concatenation of path and API key.
    auto split_path = [&](const QString& aPath)
        {
        CartoType::String s = aPath.utf16();
        const uint16_t* p = s.Data();
        const uint16_t* end = p + s.Length();
        while (p < end)
            {
            if (*p == ',')
               break;
            p++;
            }
        filename.Set(s.Data(),p - s.Data());
        api_key.clear();
        if (p < end)
            {
            CartoType::String a(p + 1,end - p - 1);
            api_key = a;
            }
        };

    // Create the CartoType framework object.
    CartoType::Framework::Param param;
    split_path(aMapFileNameArray[0]);
    param.MapFileName = filename;
    param.Key = api_key;
    param.StyleSheetFileName = m_main_window.DefaultStyleSheetPath();
    param.FontFileName = m_main_window.FontPath("DejaVuSans.ttf");

    QRect rect(geometry());
    param.ViewWidth = rect.width();
    param.ViewHeight = rect.height();

    CartoType::Result error;
    m_framework = CartoType::Framework::New(error,param);
    if (!error)
        {
        error = m_framework->CreateWritableMap(CartoType::WritableMapType::Memory);
        m_writable_map_handle = m_framework->LastMapHandle();
        if (!error)
            m_framework->EditSetWritableMap(m_writable_map_handle);
        }
    if (error)
        {
        std::string s("cannot open map ");
        s += std::string(param.MapFileName);
        m_main_window.ShowError(s.c_str(),error);
        return;
        }

    // Load any other maps.
    for (size_t i = 1; i < aMapFileNameArray.size(); i++)
        {
        split_path(aMapFileNameArray[i]);
        error = m_framework->LoadMap(filename,&api_key);
        if (error)
            {
            std::string s("cannot open map ");
            s += aMapFileNameArray[i].toStdString();
            m_main_window.ShowError(s.c_str(),error);
            }
        }

    // Start an async task to load navigation data.
    m_framework->LoadNavigationData();

    // Prevent panning beyond the map extent, and zooming in further than 1:1000.
    m_framework->SetViewLimits(1000,0);

    // Load extra fonts
    LoadFont("DejaVuSans-Bold.ttf");
    LoadFont("DejaVuSans-Oblique.ttf");
    LoadFont("DejaVuSerif.ttf");
    LoadFont("DejaVuSerif-Bold.ttf");
    LoadFont("DejaVuSerif-Italic.ttf");
    LoadFont("DejaVuSerif-BoldItalic.ttf");
    LoadFont("DroidSansFallback.ttf");
    LoadFont("MapkeyIcons.ttf");

    // Set the window title to that of the data set.
    QString qname;
    SetString(qname,m_framework->DataSetName());
    setWindowTitle(qname);

    // Add a style for UK historic counties, but disable it so that it can be enabled if needed.
    static const char* historic_counties_style =
        "<CartoTypeStyleSheet><layer name='county/historic'>"
        "<shape border='grey+red' opacity='0.5' width='0.6pt'/>"
        "<scale max='1000000'><label case='upper' color='grey+red' font-size='8pt' font-weight='bold' letter-spacing='0.2em' opacity='0.5' position='horizontal' priority='-4' wrapWidth='20em'/></scale>"
        "</layer></CartoTypeStyleSheet>";
    m_framework->AppendStyleSheet(historic_counties_style,strlen(historic_counties_style));
    m_framework->EnableLayer("county/historic",false);

    m_framework->SetFollowMode(CartoType::FollowMode::None);
    m_framework->EnableTurnInstructions(m_simulate_routing);
    CreateTurnInstructions();

    CreateLegend();
    m_framework->EnableLegend(m_draw_legend || m_draw_scale);
    m_framework->SetAnimateTransitions(true);

    // Create the initial map image.
    m_map_image = std::make_unique<QImage>(rect.width(),rect.height(),QImage::Format_ARGB32_Premultiplied);

    m_ui->perspective_slider->hide();
    }

void MapForm::ScreenChanged()
    {
    double dpr = devicePixelRatioF();
    if (dpr != m_device_pixel_ratio)
        {
        m_device_pixel_ratio = dpr;
        m_framework->Resize(CartoType::Round(size().width() * m_device_pixel_ratio),
                            CartoType::Round(size().height() * m_device_pixel_ratio));
        }
    }

MapForm::~MapForm()
    {
    delete m_ui;
    }

QString MapForm::ViewState() const
    {
    auto v = m_framework->ViewState();
    return QString::fromStdString(v.ToXml());
    }

void MapForm::SetView(const CartoType::ViewState& aViewState)
    {
    m_framework->SetView(aViewState);
    m_ui->dial->setValue(int(m_framework->Rotation() * 100));
    m_main_window.UpdateNorthUp();
    m_ui->perspective_slider->setValue((int)m_framework->PerspectiveParam().DeclinationDegrees);
    if (m_framework->Perspective())
        {
#ifndef Q_OS_MAC // vertical sliders don't work properly on Mac: see QTBUG-57194: QSlider in orientation vertical fill display inverted on Mac X.12
        m_ui->perspective_slider->show();
#endif
        }
    else
        m_ui->perspective_slider->hide();
    m_main_window.UpdatePerspective();
    }

void MapForm::resizeEvent(QResizeEvent* aEvent)
    {
    static QWindow* w;
    if (w == nullptr)
        {
        w = window()->windowHandle();
        connect(w,&QWindow::screenChanged,this,&MapForm::ScreenChanged);
        }

    QOpenGLWidget::resizeEvent(aEvent);
    if (m_framework &&
        aEvent->size().width() > 0 &&
        aEvent->size().height() > 0)
        {
        auto w = CartoType::Round(aEvent->size().width() * m_device_pixel_ratio);
        auto h = CartoType::Round(aEvent->size().height() * m_device_pixel_ratio);

        m_framework->Resize(w,h);
        m_map_image = std::make_unique<QImage>(w,h,QImage::Format_ARGB32_Premultiplied);
        }

    static bool first = true;
    if (first)
        {
        // m_framework->SetViewToWholeMap();
        first = false;
        }
    }

void MapForm::paintEvent(QPaintEvent* aEvent)
    {
    if (m_graphics_acceleration)
        {
        QOpenGLWidget::paintEvent(aEvent);
        return;
        }

    CartoType::Result error = 0;
    auto map_bitmap = MapBitmap(error);
    if (map_bitmap && !error)
        {
        CopyBitmapToImage(*map_bitmap,*m_map_image);
        QPainter painter(this);
        painter.drawImage(0,0,*m_map_image);
        }
    }

void MapForm::mousePressEvent(QMouseEvent* aEvent)
    {
    auto x = CartoType::Round(aEvent->x() * m_device_pixel_ratio);
    auto y = CartoType::Round(aEvent->y() * m_device_pixel_ratio);
    if (aEvent->button() == Qt::MouseButton::LeftButton)
        LeftButtonDown(x,y);
    else if (aEvent->button() == Qt::MouseButton::RightButton)
        RightButtonDown(x,y);
    }

void MapForm::SetEditMode(bool aSet)
    {
    if (m_edit_mode != aSet)
        {
        m_edit_mode = aSet;
        setMouseTracking(m_edit_mode);
        ShowEditedObjectSize();
        }
    }

/*
Double-clicking the left mouse button enters map object editing mode.
If there is a point of an existing editable object within 2mm, it is selected,
otherwise a new object is inserted; if the shift key is down the new object is
a polygon, else it is a line.
*/
void MapForm::mouseDoubleClickEvent(QMouseEvent* aEvent)
    {
    if (aEvent->button() != Qt::MouseButton::LeftButton)
        return;

    CartoType::PointFP p(aEvent->x() * m_device_pixel_ratio,aEvent->y() * m_device_pixel_ratio);
    if (!m_edit_mode)
        {
        if (m_framework->EditSelectNearestPoint(p,2) == CartoType::KErrorNone)
            {
            SetEditMode(true);
            return;
            }
        }

    if (m_edit_mode)
        SetEditMode(false);
    else
        {
        if (aEvent->modifiers() == Qt::ShiftModifier)
            {
            if (m_framework->EditNewPolygonObject(p) == CartoType::KErrorNone)
                {
                SetEditMode(true);
                m_writable_map_changed = true;
                }
            }
        else
            {
            if (m_framework->EditNewLineObject(p) == CartoType::KErrorNone)
                {
                SetEditMode(true);
                m_writable_map_changed = true;
                }
            }
        }
    }

void MapForm::mouseReleaseEvent(QMouseEvent* aEvent)
    {
    auto x = CartoType::Round(aEvent->x() * m_device_pixel_ratio);
    auto y = CartoType::Round(aEvent->y() * m_device_pixel_ratio);

    if (aEvent->button() == Qt::MouseButton::LeftButton)
        LeftButtonUp(x,y,aEvent->modifiers() == Qt::ShiftModifier);
    else if (aEvent->button() == Qt::MouseButton::RightButton)
        RightButtonUp(x,y);
    }

void MapForm::mouseMoveEvent(QMouseEvent* aEvent)
    {
    auto x = CartoType::Round(aEvent->x() * m_device_pixel_ratio);
    auto y = CartoType::Round(aEvent->y() * m_device_pixel_ratio);

    if (m_edit_mode)
        {
        m_framework->EditMoveCurrentPoint(CartoType::PointFP(x,y));
        ShowEditedObjectSize();
        m_writable_map_changed = true;
        update();
        return;
        }

    if (m_map_drag_enabled)
        {
        m_map_drag_offset.X = x - m_map_drag_anchor.X;
        m_map_drag_offset.Y = y - m_map_drag_anchor.Y;

        if (m_graphics_acceleration)
            {
            m_framework->Pan(m_map_drag_anchor_in_map_coords.X,m_map_drag_anchor_in_map_coords.Y,CartoType::CoordType::Map,
                             x,y,CartoType::CoordType::Display);
            m_map_drag_offset = CartoType::Point(0,0);
            m_map_drag_anchor.X = x;
            m_map_drag_anchor.Y = y;
            m_map_drag_anchor_in_map_coords = m_map_drag_anchor;
            m_framework->ConvertPoint(m_map_drag_anchor_in_map_coords.X,m_map_drag_anchor_in_map_coords.Y,CartoType::CoordType::Display,CartoType::CoordType::Map);
            }

        update();
        }
    }

void MapForm::wheelEvent(QWheelEvent* aEvent)
    {
    auto delta = aEvent->angleDelta().y();
    if (delta == 0)
        return;

    // If the mouse pointer is in the window, pan the map so that the point under the mouse pointer stays where it is.
    CartoType::RectFP r(0,0,width() * m_device_pixel_ratio,height() * m_device_pixel_ratio);
    QPointF point = aEvent->position();
    bool set_centre = false;
    CartoType::PointFP p(point.x() * m_device_pixel_ratio,point.y() * m_device_pixel_ratio);
    if (r.Contains(p))
        set_centre = true;

    double zoom = sqrt(2);
    int zoom_count = delta / QWheelEvent::DefaultDeltasPerStep;
    if (zoom_count == 0)
        zoom_count = delta >= 0 ? 1 : -1;
    zoom = pow(zoom,zoom_count);

    if (set_centre)
        m_framework->ZoomAt(zoom,p.X,p.Y,CartoType::CoordType::Display);
    else
        m_framework->Zoom(zoom);

    update();
    }

void MapForm::keyPressEvent(QKeyEvent* aEvent)
    {
    if (aEvent->type() != QEvent::KeyPress)
        return;
    double pan_x = 0;
    double pan_y = 0;
    switch (aEvent->key())
        {
        case Qt::Key_Escape:
            SetEditMode(false);
            break;

        case Qt::Key_Backspace:
            if (m_edit_mode)
                {
                m_framework->EditDeleteCurrentPoint();
                ShowEditedObjectSize();
                m_writable_map_changed = true;
                SetEditMode(false);
                }
            break;

        case Qt::Key_Delete:
            if (m_edit_mode)
                {
                QMessageBox message_box(QMessageBox::Question,"Delete editable object","Really delete this editable object?",
                                        QMessageBox::Yes | QMessageBox::Cancel);
                QMessageBox::StandardButton button = (QMessageBox::StandardButton)message_box.exec();
                if (button == QMessageBox::Yes)
                    {
                    m_framework->EditDeleteCurrentObject();
                    m_writable_map_changed = true;
                    SetEditMode(false);
                    }
                }
            break;

        case Qt::Key_Up:
            pan_x = 0;
            pan_y = -32 * m_device_pixel_ratio;
            break;

        case Qt::Key_Down:
            pan_x = 0;
            pan_y = 32 * m_device_pixel_ratio;
            break;

        case Qt::Key_Left:
            pan_x = -32 * m_device_pixel_ratio;
            pan_y = 0;
            break;

        case Qt::Key_Right:
            pan_x = 32 * m_device_pixel_ratio;
            pan_y = 0;
            break;

        case Qt::Key_F1:
            TestCode();
            break;
        }

    if (m_edit_mode && (pan_x || pan_y))
        {
        m_framework->Pan(CartoType::Round(pan_x),CartoType::Round(pan_y));
        auto pos = mapFromGlobal(cursor().pos());
        m_framework->EditMoveCurrentPoint(CartoType::PointFP(pos.x() * m_device_pixel_ratio,pos.y() * m_device_pixel_ratio));
        ShowEditedObjectSize();
        m_writable_map_changed = true;
        }

    update();
    }

void MapForm::TestCode()
    {
    auto start_time = std::chrono::high_resolution_clock::now();

    // INSERT TEST CODE HERE.
    CartoType::TileParam param;
    CartoType::TileSet tile_set;

    tile_set.FileNameTemplate = "C:\\Users\\graham\\OneDrive\\CartoType\\cases\\365\\Base Map Test Tiles\\tile-z{}-x{}-y{}.ctm1";
    tile_set.FileZoom = 1;
    tile_set.MinDisplayZoom = 0;
    tile_set.MaxDisplayZoom = 3;
    tile_set.TileRange = { 0, 0, 1, 1 };
    param.TileSetArray.push_back(tile_set);

    tile_set.FileZoom = 4;
    tile_set.MinDisplayZoom = 4;
    tile_set.MaxDisplayZoom = 21;
    tile_set.TileRange = { 0, 0, 15, 15 };
    param.TileSetArray.push_back(tile_set);

    m_framework->LoadMapsAutomatically(param);

    auto end_time = std::chrono::high_resolution_clock::now();
    double total_time = std::chrono::duration<double>(end_time - start_time).count();
    char message[100];
    snprintf(message,sizeof(message),"test code finished in %g seconds",total_time);
    //QMessageBox::about(this,"TestCode",message);
    }

void MapForm::closeEvent(QCloseEvent* aEvent)
    {
    if (m_writable_map_changed)
        {
        QMessageBox message_box(QMessageBox::Question,"Quit without saving","You have unsaved added data. Close without saving it?",
                                QMessageBox::Yes | QMessageBox::Cancel);
        QMessageBox::StandardButton button = (QMessageBox::StandardButton)message_box.exec();
        if (button == QMessageBox::Yes)
            {
            m_main_window.OnMapFormDestroyed(this);
            aEvent->accept();
            }
        else
            aEvent->ignore();
        }
    else
        {
        m_main_window.OnMapFormDestroyed(this);
        aEvent->accept();
        }
    }

bool MapForm::event(QEvent* aEvent)
    {
    if (aEvent->type() != NEW_ROUTE_EVENT)
        return QWidget::event(aEvent);

    // Handle a new route.
    if (m_new_route_may_exist)
        {
        std::lock_guard<std::mutex> lock(m_new_route_mutex);
        if (m_new_route)
            m_framework->UseRoute(m_new_route);

        if (m_new_route_error)
            {
            m_main_window.ShowError("Routing error",m_new_route_error);
            m_main_window.statusBar()->clearMessage();
            }
        else
            {
            CartoType::String text;
            m_framework->AppendDistance(text,m_new_route->Distance());
            text += " (";
            m_framework->AppendTime(text,m_new_route->Time());
            text += ")";
            QString text_qs;
            text_qs.setUtf16(text.Data(),(int)text.Length());
            m_main_window.statusBar()->showMessage(text_qs);

            // Recreate the turn instructions to update the diagram colour.
            CreateTurnInstructions();
            }

        m_main_window.UpdateManageRoute();

        m_new_route_may_exist = false;
        m_new_route = nullptr;
        m_new_route_error = { };
        }

    return true;
    }

void MapForm::initializeGL()
    {
    // Create the OpenGL map renderer.
    CartoType::Result error;
    m_map_renderer = CartoType::CQtMapRenderer::New(error,*m_framework);
    if (error)
        m_main_window.ShowError("cannot create map renderer: graphics acceleration not available",error);

    // Protect against calls by the QOpenGLWidget constructor in cases where the map could not be opened.
    if (!m_map_renderer)
        return;

    SetGraphicsAcceleration(true);
    }

void MapForm::paintGL()
    {
    // Protect against calls by the QOpenGLWidget constructor in cases where the map could not be opened.
    if (!m_map_renderer)
        return;

    m_map_renderer->Draw();
    }

void MapForm::LoadFont(const char* aFileName)
    {
    auto result = m_framework->LoadFont(m_main_window.FontPath(aFileName));
    if (result != CartoType::KErrorNone)
        {
        auto message = std::string("unable to load font ") + aFileName;
        m_main_window.ShowError(message.c_str(),result);
        }
    }

void MapForm::StopDragging()
    {
    setMouseTracking(false);
    if (m_map_drag_enabled)
        {
        m_map_drag_enabled = false;
        PanToDraggedPosition();
        }
    m_map_drag_offset = CartoType::Point(0,0);
    update();
    }

void MapForm::PanToDraggedPosition()
    {
    CartoType::Point to(m_map_drag_anchor);
    to += m_map_drag_offset;
    m_framework->Pan(m_map_drag_anchor,to);
    update();
    }

const CartoType::BitmapView* MapForm::MapBitmap(CartoType::Result& aError)
    {
    if (!m_framework)
        return nullptr;

    // Get the map bitmap.
    bool redraw_needed = false;
    auto bitmap = m_framework->MapBitmap(aError,&redraw_needed);

    /*
    If there is a legend, or there's a drag offset, copy the map bitmap into the extra GC,
    offset if necessary, and draw them on top.
    */
    if (!aError && (m_draw_legend || m_draw_scale || m_map_drag_offset.X || m_map_drag_offset.Y))
        {
        // Create the extra GC if necessary.
        if (m_extra_gc == nullptr || m_extra_gc->Bounds() != CartoType::Rect(0,0,bitmap->Width(),bitmap->Height()))
            m_extra_gc = m_framework->CreateGraphicsContext(bitmap->Width(),bitmap->Height());

        if (!aError)
            {
            assert(bitmap->DataBytes() == m_extra_gc->Bitmap()->DataBytes());
            assert(bitmap->RowBytes() == m_extra_gc->Bitmap()->RowBytes());

            // Copy the bitmap into the extra GC's bitmap. */
            if (m_map_drag_offset.X || m_map_drag_offset.Y)
                {
                // Set the destination bitmap to white so that the edges don't have relics of the previous map.
                memset(const_cast<uint8_t*>(m_extra_gc->Bitmap()->Data()),0xFF,m_extra_gc->Bitmap()->DataBytes());

                // Draw the source bitmap at the required offset.
                m_extra_gc->DrawBitmap(*bitmap,m_map_drag_offset);
                }
            else
                memcpy(const_cast<uint8_t*>(m_extra_gc->Bitmap()->Data()),bitmap->Data(),bitmap->DataBytes());

            // Draw the legend.
            if (m_draw_legend)
                m_framework->DrawNotices(*m_extra_gc);

            bitmap = m_extra_gc->Bitmap();
            }
        }

    return bitmap;
    }

void MapForm::CreateLegend()
    {
    uint32_t style = CartoType::Legend::KStyleFlagTitle | CartoType::Legend::KStyleFlagScaleInTitle;
    if (m_draw_legend)
        style |= CartoType::Legend::KStyleFlagMapObjects;
    if (m_draw_scale)
        style |= CartoType::Legend::KStyleFlagScaleBar;
    CartoType::Result error;
    auto legend = CartoType::Legend::New(error,*m_framework,style);
    if (error)
        {
        m_main_window.ShowError("could not create Legend object",error);
        return;
        }
    m_framework->SetLegend({CartoType::NoticeAnchor::TopRight,2.5,"in"},std::move(legend));
    }

void MapForm::EnableDrawLegend(bool aEnable)
    {
    if (aEnable != m_draw_legend)
        {
        m_draw_legend = aEnable;
        CreateLegend();
        m_framework->EnableLegend(m_draw_legend || m_draw_scale);
        update();
        }
    }

void MapForm::EnableDrawScale(bool aEnable)
    {
    if (aEnable != m_draw_scale)
        {
        m_draw_scale = aEnable;
        CreateLegend();
        m_framework->EnableLegend(m_draw_legend || m_draw_scale);
        update();
        }
    }

void MapForm::EnableDrawRotator(bool aEnable)
    {
    if (aEnable != m_draw_rotator)
        {
        m_draw_rotator = aEnable;
        if (m_draw_rotator)
            m_ui->dial->show();
        else
            m_ui->dial->hide();
        update();
        }
    }

void MapForm::DrawRange()
    {
    if (m_draw_range)
        {
        if (m_route_point_array[0].Point.Point != CartoType::PointFP())
            {
            CartoType::Result error = 0;
            CartoType::Geometry g1 { m_framework->Range(error,nullptr,m_route_point_array[0].Point.Point.X,m_route_point_array[0].Point.Point.Y,CartoType::CoordType::Map,600,true) };
            CartoType::Geometry g2 { m_framework->Range(error,nullptr,m_route_point_array[0].Point.Point.X,m_route_point_array[0].Point.Point.Y,CartoType::CoordType::Map,1200,true) };
            m_framework->InsertMapObject(0,"range",g1,"",CartoType::FeatureInfo(),m_range_id0,true);
            m_framework->InsertMapObject(0,"range",g2,"",CartoType::FeatureInfo(),m_range_id1,true);
            }
        }
    else
        {
        uint64_t count = 0;
        m_framework->DeleteMapObjects(0,m_range_id0,m_range_id0,count,"");
        m_framework->DeleteMapObjects(0,m_range_id1,m_range_id1,count,"");
        }
    update();
    }

void MapForm::EnableDrawRange(bool aEnable)
    {
    if (aEnable != m_draw_range)
        {
        m_draw_range = aEnable;
        DrawRange();
        }
    }

static CartoType::String AreaOrLengthString(double aArea,double aLength,bool aMetricUnits)
    {
    CartoType::String s;
    char buffer[64];
    if (aArea != 0)
        {
        if (aMetricUnits)
            {
            if (aArea > 1000000)
                snprintf(buffer,sizeof(buffer),"area = %.2f sq km",aArea / 1000000);
            else
                snprintf(buffer,sizeof(buffer),"area = %.2f ha",aArea / 10000);
            }
        else
            {
            // Convert to acres.
            double acres = aArea / 4046.856422;
            if (acres > 640)
                snprintf(buffer,sizeof(buffer),"area = %.2f sq mi",acres / 640);
            else
                snprintf(buffer,sizeof(buffer),"area = %.2f acres",acres);
            }
        s.Append(buffer);
        }
    else if (aLength != 0)
        {
        if (aMetricUnits)
            {
            if (aLength > 1000)
                snprintf(buffer,sizeof(buffer),"length = %.2f km",aLength / 1000);
            else
                snprintf(buffer,sizeof(buffer),"length = %.2f m",aLength);
            }
        else
            {
            // Convert to yards.
            double yards = aLength / 0.9144;
            if (yards > 1760)
                snprintf(buffer,sizeof(buffer),"length = %.2f mi",yards / 1760);
            else
                snprintf(buffer,sizeof(buffer),"length = %.2f yards",yards);
            }
        s.Append(buffer);
        }
    return s;
    }

void MapForm::ShowEditedObjectSize()
    {
    double area, length;
    auto error = m_framework->EditGetCurrentObjectAreaAndLength(area,length);
    if (error)
        return;

    CartoType::String s = AreaOrLengthString(area,length,m_framework->MetricUnits());
    QString qs;
    qs.setUtf16(s.Data(),int(s.Length()));
    m_main_window.statusBar()->showMessage(qs);
    }

void MapForm::LeftButtonDown(int32_t aX,int32_t aY)
    {
    if (m_edit_mode)
        {
        m_framework->EditAddCurrentPoint();
        m_writable_map_changed = true;
        }
    else
        {
        m_map_drag_enabled = true;
        m_map_drag_anchor.X = aX;
        m_map_drag_anchor.Y = aY;
        m_map_drag_anchor_in_map_coords = m_map_drag_anchor;
        m_framework->ConvertPoint(m_map_drag_anchor_in_map_coords.X,m_map_drag_anchor_in_map_coords.Y,CartoType::CoordType::Display,CartoType::CoordType::Map);
        setMouseTracking(true);
        }
    }

void MapForm::LeftButtonUp(int32_t aX,int32_t aY,bool aShift)
    {
    if (m_edit_mode)
        return;

    m_map_drag_offset.X = aX - m_map_drag_anchor.X;
    m_map_drag_offset.Y = aY - m_map_drag_anchor.Y;

    // Supply a simulated navigation fix.
    if (m_simulate_routing && aShift && m_map_drag_offset.X == 0 && m_map_drag_offset.Y == 0)
        {
        CartoType::NavigationData nav;
        nav.Validity = CartoType::NavigationData::KPositionValid | CartoType::NavigationData::KTimeValid;
        nav.Position.X = aX;
        nav.Position.Y = aY;
        m_on_route_time += 20;
        nav.Time = m_on_route_time;
        m_framework->ConvertPoint(nav.Position.X,nav.Position.Y,CartoType::CoordType::Display,CartoType::CoordType::Degree);
        CartoType::Result error = m_framework->Navigate(nav);
        if (error)
            m_main_window.ShowError("Routing error",error);

        // Show the new navigation state in the status bar.
        CartoType::String state = "navigation state: ";
        switch (m_framework->NavigationState())
            {
            case CartoType::NavigationState::None: state += "None (no route has been created, or navigation is disabled)"; break;
            case CartoType::NavigationState::NoPosition: state += "NoPosition (there is a route, and navigation is enabled, but no position has been supplied)"; break;
            case CartoType::NavigationState::Turn: state += "Turn (the position is on the route and turn information is available)"; break;
            case CartoType::NavigationState::OffRoute: state += "OffRoute (the position is off the route)"; break;
            case CartoType::NavigationState::Routing: state += "Routing (a route is being calculated)"; break;
            case CartoType::NavigationState::NewRoute: state += "NewRoute (a new route has been calculated)"; break;
            case CartoType::NavigationState::TurnRound: state += "TurnRound (the position is on the route but a U-turn is needed)"; break;
            case CartoType::NavigationState::Arrival: state += "Arrival (the position is on the route and very close to the destination)"; break;
            case CartoType::NavigationState::Departure: state += "Departure (navigation on a route has started)"; break;
            }
        QString state_qs;
        state_qs.setUtf16(state.Data(),(int)state.Length());
        m_main_window.statusBar()->showMessage(state_qs);
        }

    StopDragging();
    }

void MapForm::RightButtonDown(int32_t /*aX*/,int32_t /*aY*/)
    {

    }

void MapForm::RightButtonUp(int32_t aX,int32_t aY)
    {
    ShowRouteDialog(aX,aY);
    }

void MapForm::ShowRouteDialog(int32_t aX,int32_t aY)
    {
    CartoType::PointFP p(aX,aY);

    // Get the address of the point clicked on.
    CartoType::Address address;
    m_framework->GetAddress(address,aX,aY,CartoType::CoordType::Display);

    CartoType::String address_string;
    m_framework->ConvertPoint(p.X,p.Y,CartoType::CoordType::Display,CartoType::CoordType::Degree);

    address_string = address.ToString(false,&p);

    address_string.Append("\r\n\r\n[");
    address_string += address.ToStringWithLabels();
    address_string.Append("]");

    // Add the altitude if available.
    double x[] = { p.X, p.Y };
    CartoType::CoordSet cs(x,2);
    CartoType::Result error;
    auto alt = m_framework->Heights(error,cs,CartoType::CoordType::Degree);
    if (!error)
        {
        char buffer[32];
        snprintf(buffer,sizeof(buffer)," (altitude = %dm)",alt[0]);
        address_string.Append(buffer);
        }

    // If possible, get the Ordnance Survey National Grid reference.
    CartoType::String os_grid_ref = CartoType::UKGridReferenceFromDegrees(p,3);
    if (os_grid_ref.Length())
        {
        address_string.Append("\r\n\r\nOrdnance Survey of Great Britain grid reference: ");
        address_string.Append(os_grid_ref);
        }

    // Get information about the objects clicked on - within 2 millimetres.
    CartoType::MapObjectArray object;
    double pixel_mm = m_framework->ResolutionDpi() / 25.4;
    m_framework->FindInDisplay(object,100,aX,aY,int(ceil(2 * pixel_mm)));
    CartoType::MapObject* pushpin = nullptr;

    // See if we have a pushpin, and geocode the objects so that they can be sorted.
    for (auto& p : object)
        {
        if (p->LayerName() == "pushpin" && p->Type() == CartoType::MapObjectType::Point)
            pushpin = p.get();
        p->SetUserData((int)p->GeoCodeType());
        }

    // Sort objects on geocode and add information about them to the 'Address and nearby objects' field.
    if (object.size())
        {
        bool first_object = true;
        std::sort(object.begin(),object.end(),CartoType::MapObjectUserDataLessThan);
        for (auto& p : object)
            {
            // Skip objects with a geocode type greater than City; avoids reporting entire nation, etc.
            if (p->GeoCodeType() > CartoType::GeoCodeType::City)
                continue;
            // Skip unnamed map_extent and outline objects.
            if (!p->Label().Length())
                {
                if (p->LayerName() == "outline" || p->LayerName() == "map-extent")
                    continue;
                }
            if (first_object)
                {
                address_string.Append("\r\n\r\nNearby objects:\r\n");
                first_object = false;
                }
            address_string.Append(p->VerboseDescription().c_str());
            address_string.Append("\r\n");
            }
        }

    // Set up the route dialog.
    CartoType::String pushpin_color = m_pushpin_color;
    uint16_t pushpin_glyph = m_pushpin_glyph;
    if (pushpin)
        {
        CartoType::Text c { pushpin->StringAttribute("_color") };
        if (c.Length())
            pushpin_color = c;
        CartoType::Text g { pushpin->StringAttribute("_iconText") };
        if (g.Length() == 1)
            pushpin_glyph = g[0];
        }
    RouteDialog route_dialog(*this,*m_framework,pushpin_color,pushpin_glyph);
    if (pushpin)
        {
        route_dialog.m_ui->addPushPin->setText("Edit pushpin");
        route_dialog.m_ui->addPushPin->setChecked(true);
        route_dialog.EnablePushPinEditing();
        }
    else
        {
        route_dialog.m_ui->cutPushPin->hide();
        if (m_route_point_array[0].Point.Point == CartoType::PointFP())
            route_dialog.m_ui->setStartOfRoute->setChecked(true);
        else
            route_dialog.m_ui->setEndOfRoute->setChecked(true);
        }
    route_dialog.m_ui->visitWayPointsInAnyOrder->setChecked(m_best_route);

    QString qs;
    SetString(qs,address_string);
    route_dialog.m_ui->address->setPlainText(qs);

    uint64_t pushpin_id = 0;
    if (pushpin)
        {
        CartoType::Text value = pushpin->StringAttribute("");
        SetString(m_pushpin_name,value);
        value = pushpin->StringAttribute("desc");
        SetString(m_pushpin_desc,value);
        pushpin_id = pushpin->Id();
        }
    else
        {
        SetString(m_pushpin_name,address.ToString(false));
        SetString(m_pushpin_desc,address.ToString(true));
        }
    route_dialog.m_ui->pushPinName->setText(m_pushpin_name);
    route_dialog.m_ui->pushPinDesc->setPlainText(m_pushpin_desc);

    // Display the route dialog.
    if (route_dialog.exec() != QDialog::DialogCode::Accepted)
        return;

    bool create_route = false;
    m_best_route = route_dialog.m_ui->visitWayPointsInAnyOrder->isChecked();

    // Convert the point clicked on to map coords.
    CartoType::RoutePoint rp;
    rp.Point.X = aX;
    rp.Point.Y = aY;
    m_framework->ConvertPoint(rp.Point.X,rp.Point.Y,CartoType::CoordType::Display,CartoType::CoordType::Map);

    if (route_dialog.m_ui->setStartOfRoute->isChecked()) // set start of route
        {
        m_route_point_array[0].Point = rp;
        DrawRange();
        create_route = true;
        }
    else if (route_dialog.m_ui->setEndOfRoute->isChecked()) // set end of route
        {
        m_route_point_array[m_route_point_array.size() - 1].Point = rp;
        create_route = true;
        }
    else if (route_dialog.m_ui->addPushPin->isChecked()) // add or change a pushpin
        {
        m_pushpin_name = route_dialog.m_ui->pushPinName->text();
        m_pushpin_desc = route_dialog.m_ui->pushPinDesc->toPlainText();
        m_pushpin_color = route_dialog.PushPinColor();
        m_pushpin_glyph = route_dialog.PushPinGlyph();

        CartoType::String string_attrib = m_pushpin_name.utf16();
        if (m_pushpin_desc.length())
            string_attrib.SetAttribute("desc",m_pushpin_desc.utf16());
        string_attrib.SetAttribute("_color",m_pushpin_color);
        double x = 0,y = 0;
        if (pushpin)
            {
            CartoType::Rect r { pushpin->CBox() };
            x = r.Min.X;
            y = r.Min.Y;
            }
        else
            {
            x = aX;
            y = aY;
            m_framework->ConvertPoint(x,y,CartoType::CoordType::Display,CartoType::CoordType::Map);
            }
        if (m_pushpin_glyph)
            {
            CartoType::Text t(&m_pushpin_glyph,1);
            string_attrib.SetAttribute("_iconText",t);
            }
        m_framework->InsertPointMapObject(m_writable_map_handle,"pushpin",x,y,CartoType::CoordType::Map,string_attrib,CartoType::FeatureInfo(),pushpin_id,true);
        m_main_window.UpdateSaveAddedData();
        m_main_window.UpdateDeletePushpins();

        if (route_dialog.m_ui->useAsWayPoint->isChecked())
            {
            RoutePoint* waypoint = nullptr;

            for (int i = 1; waypoint == nullptr && i < (int)m_route_point_array.size() - 1; i++)
                if (m_route_point_array[i].Id == pushpin_id)
                    waypoint = &m_route_point_array[i];
            if (waypoint == nullptr)
                {
                RoutePoint rp;
                m_route_point_array.insert(m_route_point_array.begin() + m_route_point_array.size() - 1,rp);
                waypoint = &m_route_point_array[m_route_point_array.size() - 2];
                }
            if (waypoint)
                {
                waypoint->Point.Point.X = x;
                waypoint->Point.Point.Y = y;
                waypoint->Id = pushpin_id;
                create_route = true;
                }
            }

        m_writable_map_changed = true;
        }
    else if (route_dialog.m_ui->cutPushPin->isChecked()) // cut a pushpin
        {
        uint64_t count = 0;
        m_framework->DeleteMapObjects(m_writable_map_handle,pushpin_id,pushpin_id,count);
        for (int i = 1; i < (int)m_route_point_array.size() - 1; i++)
            if (m_route_point_array[i].Id == pushpin_id)
                {
                m_route_point_array.erase(m_route_point_array.begin() + i);
                create_route = true;
                break;
                }
        m_writable_map_changed = true;
        m_main_window.UpdateSaveAddedData();
        m_main_window.UpdateDeletePushpins();
        }

    if (create_route)
        CalculateAndDisplayRoute();

    update();
    }

void MapForm::CalculateAndDisplayRoute()
    {
    // Do nothing if either of the route points are (0,0), which by convention is the illegal position.
    if (m_route_point_array[0].Point.Point == CartoType::PointFP() ||
        m_route_point_array[m_route_point_array.size() - 1].Point.Point == CartoType::PointFP())
        return;

    CartoType::RouteCoordSet cs(CartoType::CoordType::Map);
    for (const auto& p : m_route_point_array)
        cs.RoutePointArray.push_back(p.Point);

    auto create_route_async_callback = [this](CartoType::Result aError,std::shared_ptr<CartoType::Route> aRoute)->void
        {
        std::lock_guard<std::mutex> lock(m_new_route_mutex);
        m_new_route_may_exist = true;
        m_new_route_error = aError;
        m_new_route = aRoute;
        auto event = new QEvent(NEW_ROUTE_EVENT);
        QCoreApplication::postEvent(this,event);
        };

    CartoType::RouteProfile profile = m_framework->RouteProfile();
    if (m_best_route && m_route_point_array.size() > 3)
        {
        size_t iterations = m_route_point_array.size() * m_route_point_array.size();
        if (iterations < 16)
            iterations = 16;
        else if (iterations > 256)
            iterations = 256;
        m_framework->CreateBestRouteAsync(create_route_async_callback,profile,cs,true,true,uint32_t(iterations),true);
        }
    else
        m_framework->CreateRouteAsync(create_route_async_callback,profile,cs,true);

    m_main_window.statusBar()->showMessage("calculating route...");
    }

void MapForm::CreateTurnInstructions()
    {
    m_framework->SetTurnInstructions({CartoType::NoticeAnchor::TopLeft,40,"mm",15,"mm",4,"mm"});
    }

void MapForm::Find()
    {
    m_find_dialog.Set(*m_framework);
    QString qs;
    qs.setUtf16(m_find_text.Data(),(int)m_find_text.Length());
    m_find_dialog.m_ui->findText->setText(qs);
    m_find_dialog.m_ui->findText->setFocus();

    m_found_object_index = 0;
    while (m_find_dialog.exec() == QDialog::DialogCode::Accepted)
        {
        m_find_text = m_find_dialog.m_ui->findText->text().utf16();
        if (m_find_text.Length() == 0)
            continue;

        m_found_object = m_find_dialog.FoundObjectArray();
        if (m_found_object.size() == 0)
            {
            std::string s("Text ");
            s += std::string(m_find_text) + " not found";
            m_main_window.ShowError(s.c_str(),0);
            continue;
            }

        uint64_t count = 0;
        m_framework->DeleteMapObjects(0,m_found_object_id,m_found_object_id,count);
        m_found_object_id = 0;
        ShowNextFoundObject();
        break;
        }

    m_main_window.UpdateFindNext();
    }

void MapForm::FindAddress()
    {
    FindAddressDialog find_dialog;

    SetString(*find_dialog.m_ui->building,m_address.Building);
    SetString(*find_dialog.m_ui->feature,m_address.Feature);
    SetString(*find_dialog.m_ui->street,m_address.Street);
    SetString(*find_dialog.m_ui->subLocality,m_address.SubLocality);
    SetString(*find_dialog.m_ui->locality,m_address.Locality);
    SetString(*find_dialog.m_ui->island,m_address.Island);
    SetString(*find_dialog.m_ui->subAdminArea,m_address.SubAdminArea);
    SetString(*find_dialog.m_ui->adminArea,m_address.AdminArea);
    SetString(*find_dialog.m_ui->country,m_address.Country);
    SetString(*find_dialog.m_ui->postCode,m_address.PostCode);

    bool stop = false;

    m_found_object_index = 0;
    while (!stop)
        {
        if (find_dialog.exec() == QDialog::DialogCode::Accepted)
            {
            m_found_object.clear();
            SetString(m_address.Building,*find_dialog.m_ui->building);
            SetString(m_address.Feature,*find_dialog.m_ui->feature);
            SetString(m_address.Street,*find_dialog.m_ui->street);
            SetString(m_address.SubLocality,*find_dialog.m_ui->subLocality);
            SetString(m_address.Locality,*find_dialog.m_ui->locality);
            SetString(m_address.Island,*find_dialog.m_ui->island);
            SetString(m_address.SubAdminArea,*find_dialog.m_ui->subAdminArea);
            SetString(m_address.AdminArea,*find_dialog.m_ui->adminArea);
            SetString(m_address.Country,*find_dialog.m_ui->country);
            SetString(m_address.PostCode,*find_dialog.m_ui->postCode);

            CartoType::Result error = m_framework->FindAddress(m_found_object,100,m_address,true);
            if (error || m_found_object.size() == 0)
                {
                m_main_window.ShowError("address not found",error);
                if (error)
                    stop = true;
                }
            else
                {
                ShowNextFoundObject();
                stop = true;
                }
            }
        else
            stop = true;
        }

    m_main_window.UpdateFindNext();
    }

void MapForm::FindNext()
    {
    ShowNextFoundObject();
    }

void MapForm::ShowNextFoundObject()
    {
    auto& object = m_found_object[m_found_object_index];

    CartoType::Result error;
    CartoType::TextLiteral(found_layer,u"found");
    if (object->Type() == CartoType::MapObjectType::Line)
        {
        auto envelope = object->Envelope(5.0 * CartoType::KMapUnitsPerMeter);
        error = m_framework->InsertMapObject(0,found_layer,CartoType::Geometry(envelope,CartoType::CoordType::Map,true),"",CartoType::FeatureInfo(),m_found_object_id,true);
        }
    else
        {
        CartoType::String s;
        CartoType::FeatureInfo f;
        error = m_framework->InsertCopyOfMapObject(0,found_layer,*object,0,CartoType::CoordType::MapMeter,m_found_object_id,true,&s,&f);
        }

    if (!error)
        {
        bool animate = m_framework->SetAnimateTransitions(true);
        m_framework->SetView(*object,64,1000);
        m_framework->SetAnimateTransitions(animate);
        }
    if (error)
        return;

    CartoType::String summary;
    m_framework->GetGeoCodeSummary(summary,*object);
    double length = object->LengthOrPerimeter(error);
    double area = 0;
    if (!error)
        area = object->Area(error);
    if (!error && (area || length))
        {
        summary += "; ";
        summary += AreaOrLengthString(area,length,m_framework->MetricUnits());
        }
    QString summary_qs;
    summary_qs.setUtf16(summary.Data(),(int)summary.Length());
    m_main_window.statusBar()->showMessage(summary_qs);

    m_found_object_index = (m_found_object_index + 1) % m_found_object.size();
    update();
    }

void MapForm::on_dial_valueChanged(int aValue)
    {
    m_framework->SetRotation(double(aValue) / 100.0);
    m_main_window.UpdateNorthUp();
    update();
    }

void MapForm::SetRotation(double aAngle)
    {
    m_framework->SetRotation(aAngle);
    m_ui->dial->setValue(int(aAngle * 100));
    update();
    }

void MapForm::ReverseRoute()
    {
    CartoType::Result error = m_framework->ReverseRoute();
    if (!error)
        {
        std::reverse(m_route_point_array.begin(),m_route_point_array.end());
        update();
        }
    }

void MapForm::DeleteRoute()
    {
    m_route_point_array.resize(2);
    m_route_point_array[0] = m_route_point_array[1] = RoutePoint();
    m_framework->DeleteRoute();
    m_main_window.UpdateManageRoute();
    update();
    }

void MapForm::DeletePushpins()
    {
    uint64_t deleted_count = 0;
    m_framework->DeleteMapObjects(m_writable_map_handle,0,UINT64_MAX,deleted_count,"@layer=='pushpin'");
    m_main_window.UpdateDeletePushpins();
    update();
    }

void MapForm::SetSimulateRouting(bool aSet)
    {
    m_simulate_routing = aSet;
    m_framework->EnableTurnInstructions(aSet);
    update();
    }

bool MapForm::HasPushpins() const
    {
    CartoType::MapObjectArray object_array;
    CartoType::FindParam param;
    param.MaxObjectCount = 1;
    param.Layers = "pushpin";
    param.Merge = false;
    m_framework->Find(object_array,param);
    return object_array.size() > 0;
    }

size_t MapForm::BuiltInProfileCount()
    {
    return m_framework->BuiltInRouteProfileCount();
    }

const CartoType::RouteProfile* MapForm::BuiltInProfile(size_t aIndex)
    {
    return m_framework->BuiltInRouteProfile(uint32_t(aIndex));
    }

void MapForm::SetRouteProfileIndex(size_t aIndex)
    {
    if (aIndex == m_route_profile_index)
        return;

    m_route_profile_index = aIndex;
    if (m_framework->BuiltInRouteProfileCount())
        m_framework->SetBuiltInRouteProfile(uint32_t(aIndex));
    else if (aIndex == 5)
        m_framework->SetRouteProfile(m_main_window.CustomRouteProfile());
    else
        {
        auto type = CartoType::RouteProfileType::Car;
        switch (aIndex)
            {
            case 1: type = CartoType::RouteProfileType::Cycle; break;
            case 2: type = CartoType::RouteProfileType::Walk; break;
            case 3: type = CartoType::RouteProfileType::Hike; break;
            case 4: type = CartoType::RouteProfileType::Ski; break;
            default: break;
            }
        m_framework->SetRouteProfile(type);
        }

    CalculateAndDisplayRoute();
    if (m_draw_range)
        DrawRange();
    m_main_window.UpdateRouteProfile();
    update();
    }

void MapForm::SetPreferredRouterType(CartoType::RouterType aRouterType)
    {
    if (aRouterType != m_framework->PreferredRouterType())
        {
        m_framework->SetPreferredRouterType(aRouterType);
        m_framework->LoadNavigationData();
        CalculateAndDisplayRoute();
        update();
        }
    }

void MapForm::ViewRouteInstructions()
    {
    auto route = m_framework->Route();
    if (!route)
        return;

    RouteInstructionsDialog dialog(this);
    CartoType::String s(m_framework->RouteInstructions(*route));
    QString instructions;
    instructions.setUtf16(s.Data(),(int)s.Length());
    dialog.m_ui->text->setPlainText(instructions);
    dialog.exec();
    }

bool MapForm::EditCustomRouteProfile(CartoType::RouteProfile& aRouteProfile)
    {
    RouteProfileDialog dialog(this,aRouteProfile,m_framework->MetricUnits());
    if  (dialog.exec() == QDialog::DialogCode::Accepted)
        {
        aRouteProfile = dialog.RouteProfile();
        if (m_framework->BuiltInRouteProfileCount() == 0 && m_route_profile_index == 4) // if using the custom route profile
            {
            m_framework->SetRouteProfile(aRouteProfile);
            if (m_route_point_array[0].Point.Point != CartoType::PointFP() &&
                m_route_point_array[m_route_point_array.size() - 1].Point.Point != CartoType::PointFP())
                CalculateAndDisplayRoute();
            }
        return true;
        }
    return false;
    }

void MapForm::SaveRouteInstructions()
    {
    if (!m_framework->Route())
        return;

    std::string filename { GetSaveFile(*this,"Save route instructions","text files","txt") };
    if (filename.empty())
        return;

    std::ofstream file(filename.c_str());
    if (!file.fail())
        {
        std::string text = m_framework->RouteInstructions(*m_framework->Route());
        file.write(text.c_str(),text.length());
        }
    if (file.fail())
        m_main_window.ShowError("failed to save the route instructions to a text file",CartoType::KErrorIo);
    }

void MapForm::SaveRouteAsXml()
    {
    if (!m_framework->Route())
        return;

    CartoType::String path { GetSaveFile(*this,"Save Route","CartoType route files","ctroute") };
    if (!path.Length())
        return;
    CartoType::Result error = m_framework->WriteRouteAsXml(*m_framework->Route(),path);
    if (error)
        m_main_window.ShowError("failed to save the route to a CTROUTE file",error);
    }

void MapForm::SaveRouteAsGpx()
    {
    if (!m_framework->Route())
        return;

    CartoType::String path { GetSaveFile(*this,"Save route as GPX","GPX files","gpx") };
    if (!path.Length())
        return;

    CartoType::Result error = m_framework->WriteRouteAsXml(*m_framework->Route(),path,CartoType::FileType::GPX);
    if (error)
        m_main_window.ShowError("failed to save the route to a GPX file",error);
    }

void MapForm::LoadRouteFromXml()
    {
    QString path = QFileDialog::getOpenFileName(this,"Load route","","GPX files or CartoType route files (*.gpx *.ctroute)");
    if (!path.length())
        return;
    CartoType::Result error = m_framework->ReadRouteFromXml(path.utf16());
    if (error)
        m_main_window.ShowError("failed to load a route from a CTROUTE file",error);
    else
        {
        auto point_array = m_framework->Route()->RoutePoints();
        m_route_point_array.clear();
        for (const auto& p : point_array)
            {
            RoutePoint rp;
            rp.Point.Point = p;
            m_route_point_array.push_back(rp);
            }
        if (m_route_point_array.size() < 2)
            {
            m_route_point_array.resize(2);
            m_route_point_array[0] = m_route_point_array[1] = RoutePoint();
            }
        m_main_window.UpdateManageRoute();
        update();
        }
    }

void MapForm::SaveImageAsPng()
    {
    CartoType::String path = GetSaveFile(*this,"Save image as PNG","PNG files","png");
    if (!path.Length())
        return;
    auto error = m_framework->WriteMapImage(path,CartoType::FileType::PNG,true);
    if (error)
        m_main_window.ShowError("failed to save the image as a PNG file",error);
    }

bool MapForm::HasWritableData() const
    {
    return !m_framework->MapIsEmpty(m_writable_map_handle);
    }

void MapForm::SaveWritableDataAsCtms()
    {
    CartoType::String path { GetSaveFile(*this,"Save added data as CTMS","CartoType serialized map files","ctms") };
    if (!path.Length())
        return;
    CartoType::Result error = m_framework->SaveMap(m_writable_map_handle,path,CartoType::FileType::CTMS);
    if (error)
        m_main_window.ShowError("failed to save data to a CTMS file",error);
    else
        m_writable_map_changed = false;
    }

void MapForm::LoadWritableDataFromCtms()
    {
    QString qs = QFileDialog::getOpenFileName(this,"Import data from a CTMS file","","CartoType serialized map files (*.ctms)");
    CartoType::String filename = qs.utf16();
    CartoType::Result error = m_framework->ReadMap(m_writable_map_handle,filename,CartoType::FileType::CTMS);
    if (error)
        m_main_window.ShowError("failed to read data from a CTMS file",error);
    m_main_window.UpdateSaveAddedData();
    }

void MapForm::LoadWritableDataFromGpx()
    {
    QString filename = QFileDialog::getOpenFileName(this,"Import data from a GPX file","","GPX files (*.gpx)");
    CartoType::Result error = m_framework->ReadGpx(m_writable_map_handle,filename.utf16());
    if (error)
        m_main_window.ShowError("failed to read data from a GPX file",error);
    m_main_window.UpdateSaveAddedData();
    }

void MapForm::SetScale()
    {
    ScaleDialog scale_dialog(this);
    QString scale_qs;
    scale_qs.setNum((int)m_framework->ScaleDenominatorInView());
    scale_dialog.m_ui->scale->setText(scale_qs);
    if (scale_dialog.exec() == QDialog::DialogCode::Accepted)
        {
        scale_qs = scale_dialog.m_ui->scale->text();
        int new_scale = scale_qs.toInt();
        m_framework->SetScaleDenominatorInView(new_scale);
        update();
        }
    }

void MapForm::GoToLocation(double aLong,double aLat)
    {
    double minx,miny,maxx,maxy;
    m_framework->GetMapExtent(minx,miny,maxx,maxy,CartoType::CoordType::Degree);
    if (aLong < minx)
        aLong = minx;
    else if (aLong > maxx)
        aLong = maxx;
    if (aLat < miny)
        aLat = miny;
    else if (aLat > maxy)
        aLat = maxy;

    m_found_object.clear();
    m_found_object_index = 0;
    CartoType::TextLiteral(found_layer,u"found");
    m_framework->InsertCircleMapObject(0,found_layer,
                                       aLong,aLat,CartoType::CoordType::Degree,50,CartoType::CoordType::MapMeter,
                                       nullptr,CartoType::FeatureInfo(),m_found_object_id,true);
    m_framework->SetScaleDenominator(25000);
    m_framework->SetViewCenter(aLong,aLat,CartoType::CoordType::Degree);
    update();
    }

static double Degrees(const CartoType::MString& aText)
    {
    double degrees = 0;
    double scale = 1;
    double sign = 1;
    auto p = aText.Data();
    auto end = p + aText.Length();
    while (p < end)
        {
        while (p < end && CartoType::Char(*p).IsWhitespace())
            p++;
        if (p == end)
            break;

        auto start = p;
        while (p < end && !CartoType::Char(*p).IsWhitespace())
            p++;
        CartoType::Text section(start,p - start);
        if (section == "s" || section == "S" || section == "w" || section == "W")
            {
            sign = -sign;
            continue;
            }
        if (section == "n" || section == "N" || section == "e" || section == "E")
            continue;

        size_t length = 0;
        degrees += section.ToDouble(&length) * scale;
        if (degrees < 0 && scale == 1)
            scale = -scale;

        if (length < section.Length() - 1) // allow a single trailing punctuation character like ' or "
            return 0;
        scale /= 60;
        }

    return degrees * sign;
    }

void MapForm::GoToLocation()
    {
    LocationDialog location_dialog(this);
    double minx,miny,maxx,maxy;
    m_framework->GetView(minx,miny,maxx,maxy,CartoType::CoordType::Degree);
    double cur_long = (minx + maxx) / 2;
    double cur_lat = (miny + maxy) / 2;
    QString long_qs;
    long_qs.setNum(cur_long);
    location_dialog.m_ui->longitude->setText(long_qs);
    QString lat_qs;
    lat_qs.setNum(cur_lat);
    location_dialog.m_ui->latitude->setText(lat_qs);
    if (location_dialog.exec() == QDialog::DialogCode::Accepted)
        {
        CartoType::String long_text(location_dialog.m_ui->longitude->text().toUtf8());
        double new_long = Degrees(long_text);
        CartoType::String lat_text(location_dialog.m_ui->latitude->text().toUtf8());
        double new_lat = Degrees(lat_text);
        GoToLocation(new_long,new_lat);
        }
    }

void MapForm::GoToOrdnanceSurveyGridRef()
    {
    LocationDialog location_dialog(this);
    location_dialog.m_ui->lat_label->hide();
    location_dialog.m_ui->latitude->hide();
    location_dialog.m_ui->long_label->setText("grid reference");
    location_dialog.setWindowTitle("Go to Grid Reference");

    double minx,miny,maxx,maxy;
    m_framework->GetView(minx,miny,maxx,maxy,CartoType::CoordType::Degree);
    double cur_long = (minx + maxx) / 2;
    double cur_lat = (miny + maxy) / 2;
    CartoType::PointFP p(cur_long,cur_lat);
    CartoType::String ref = CartoType::UKGridReferenceFromDegrees(p,3);
    QString ref_qs;
    ref_qs.setUtf16(ref.Data(),(int)ref.Length());
    location_dialog.m_ui->longitude->setText(ref_qs);
    if (location_dialog.exec() == QDialog::DialogCode::Accepted)
        {
        ref_qs = location_dialog.m_ui->longitude->text();
        ref = ref_qs.utf16();
        p = CartoType::PointInDegreesFromUKGridReference(ref);
        if (p.X != 0 && p.Y != 0)
            GoToLocation(p.X,p.Y);
        }
    }

void MapForm::ChooseStyleSheet()
    {
    QString qs = QFileDialog::getOpenFileName(this,"Choose Style Sheet","","CartoType style sheets (*.ctstyle *.xml)");
    CartoType::String filename = qs.utf16();
    if (filename.Length())
        {
        CartoType::Result error = m_framework->SetStyleSheet(filename);
        if (error)
            m_main_window.ShowError("error loading stylesheet",error);
        else
            {
            CreateLegend();
            update();
            }
        }
    }

void MapForm::ReloadStyleSheet()
    {
    m_framework->ReloadStyleSheet();
    CreateLegend();
    update();
    }

void MapForm::ApplyStyleSheet(const std::string& aStyleSheet)
    {
    if (aStyleSheet.length())
        {
        m_main_window.SetCustomStyleSheet(aStyleSheet);

        if (!m_use_custom_style_sheet)
            SetUseCustomStyleSheet(true);
        else
            {
            m_framework->SetStyleSheet(aStyleSheet.data(),aStyleSheet.length());
            CreateLegend();
            update();
            }
        m_style_sheet_applied = true;
        }
    }

void MapForm::FinishEditingCustomStyleSheet(int /*aResult*/)
    {
    m_style_sheet_applied = false;
    }

void MapForm::ChooseLayers()
    {
    LayerDialog layer_dialog(this);
    std::vector<CartoType::String> layers { m_framework->LayerNames() };
    for (const auto& layer_name : layers)
        {
        QString qs_name; qs_name.setUtf16(layer_name.Data(),(int)layer_name.Length());
        QListWidgetItem* item = new QListWidgetItem(qs_name);
        if (m_framework->LayerIsEnabled(layer_name))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        layer_dialog.m_ui->list->addItem(item);
        }
    if (layer_dialog.exec() == QDialog::DialogCode::Accepted)
        {
        for (int i = 0; i < layer_dialog.m_ui->list->count(); i++)
            {
            QString qs_name = layer_dialog.m_ui->list->item(i)->text();
            CartoType::String layer_name = qs_name.utf16();
            bool checked = layer_dialog.m_ui->list->item(i)->checkState() == Qt::Checked;
            m_framework->EnableLayer(layer_name,checked);
            }
        update();
        }
    }

void MapForm::SetMetricUnits(bool aEnable)
    {
    if (aEnable != m_framework->MetricUnits())
        {
        m_framework->SetMetricUnits(aEnable);
        CreateLegend();
        if (DrawScaleEnabled())
            update();
        }
    }

void MapForm::SetNightMode(bool aSet)
    {
    m_framework->SetNightMode(aSet);
    CreateLegend();
    update();
    }

bool MapForm::NightMode() const
    {
    return m_framework->NightMode();
    }

void MapForm::SetGraphicsAcceleration(bool aEnable)
    {
    if (aEnable != m_graphics_acceleration)
        {
        if (aEnable && !m_map_renderer)
            return;

        m_graphics_acceleration = aEnable;
        m_main_window.UpdateGraphicsAcceleration();
        if (m_graphics_acceleration)
            {
            connect(&m_graphics_acceleration_update_timer,SIGNAL(timeout()),this,SLOT(update()));
            m_graphics_acceleration_update_timer.start(33);
            }
        else
            m_graphics_acceleration_update_timer.stop();
        update();
        }
    }

void MapForm::on_perspective_slider_valueChanged(int aValue)
    {
    CartoType::PerspectiveParam p;
    p.DeclinationDegrees = aValue;
    m_framework->SetPerspectiveParam(p);
    update();
    }

void MapForm::SetPerspective(bool aEnable)
    {
    if (aEnable != m_framework->Perspective())
        {
        if (aEnable)
            {
            m_framework->SetPerspective(true);
#ifndef Q_OS_MAC // vertical sliders don't work properly on Mac: see QTBUG-57194: QSlider in orientation vertical fill display inverted on Mac X.12
            m_ui->perspective_slider->show();
#endif
            }
        else
            {
            m_framework->SetPerspective(false);
            m_ui->perspective_slider->hide();
            }
        update();
        }
    }

bool MapForm::Perspective() const
    {
    return m_framework->Perspective();
    }

void MapForm::EnableDebugLayers(bool aEnable)
    {
    m_debug_layers_enabled = aEnable;
    m_framework->SetStyleSheetVariable("DEBUG",aEnable ? "1" : "");
    }

bool MapForm::DebugLayers() const
    {
    return m_debug_layers_enabled;
    }

void MapForm::EnableFixedLabels(bool aEnable)
    {
    m_fixed_labels = aEnable;
    if (m_fixed_labels)
        {
        m_framework->SetFixedLabels(true);
        m_framework->SetLabelFPS(10);
        }
    else
        {
        m_framework->SetFixedLabels(false);
        m_framework->SetLabelFPS(2);
        }
    }

bool MapForm::FixedLabels() const
    {
    return m_fixed_labels;
    }

void MapForm::SetDraw3DBuildings(bool aEnable)
    {
    if (aEnable != m_framework->Draw3DBuildings())
        {
        m_framework->SetDraw3DBuildings(aEnable);
        update();
        }
    }

bool MapForm::Draw3DBuildings() const
    {
    return m_framework->Draw3DBuildings();
    }

bool MapForm::MapIncludesGreatBritain() const
    {
    CartoType::RectFP bounds;
    m_framework->GetMapExtent(bounds,CartoType::CoordType::Degree);
    static CartoType::RectFP gb(-10,49,4,62);
    bounds.Intersection(gb);
    return !bounds.IsEmpty();
    }

void MapForm::SetUseCustomStyleSheet(bool aEnable)
    {
    if (m_use_custom_style_sheet == aEnable)
        return;
    m_use_custom_style_sheet = aEnable;
    m_main_window.UpdateStyleSheet();
    if (aEnable)
        {
        m_saved_style_sheet = m_framework->StyleSheetData(0);

        // If there's no custom style sheet, copy the current style sheet into it.
        if (m_main_window.CustomStyleSheet().empty())
            m_main_window.SetCustomStyleSheet(m_framework->StyleSheetText(0));

        const std::string& style = m_main_window.CustomStyleSheet();
        m_framework->SetStyleSheet(style.data(),style.length());
        }
    else
        m_framework->SetStyleSheet(m_saved_style_sheet);
    CreateLegend();
    update();
    }

void MapForm::OnCustomRouteProfileLoaded()
    {
    m_framework->SetRouteProfile(m_main_window.CustomRouteProfile());
    CalculateAndDisplayRoute();
    }

void MapForm::OnCustomStyleSheetLoaded()
    {
    if (m_use_custom_style_sheet)
        {
        const std::string& style = m_main_window.CustomStyleSheet();
        m_framework->SetStyleSheet(style.data(),style.length());
        }
    }

void MapForm::Print(bool aPreview)
    {
    QPrinter printer(QPrinter::HighResolution);

    if (!aPreview)
        {
        QPrintDialog dialog(&printer,this);
        dialog.setWindowTitle(tr("Print Map"));
        if (dialog.exec() != QDialog::Accepted)
            return;
        }

    CartoType::Result error = 0;
    std::unique_ptr<CartoType::Framework> print_framework { m_framework->Copy(error) };
    if (error)
        {
        m_main_window.ShowError("failed to make a CartoType::Framework copy for printing",error);
        return;
        }
    print_framework->SetResolutionDpi(printer.resolution());
    QRectF page_rect { printer.pageRect(QPrinter::DevicePixel) };

    auto CreateImage = [&](QRectF aPageRect)->QImage
        {
        print_framework->SetViewLimits(1000,0);
        print_framework->SetView(m_framework->ViewState());
        print_framework->Resize(int(aPageRect.width()),int(aPageRect.height()));

        auto map_bitmap = print_framework->MapBitmap(error);
        if (error)
            {
            m_main_window.ShowError("failed to draw the map for printing",error);
            return QImage();
            }

        auto legend = CartoType::Legend::New(error,*print_framework,
                                             CartoType::Legend::KStyleFlagTitle | CartoType::Legend::KStyleFlagScaleInTitle | CartoType::Legend::KStyleFlagScaleBar);
        if (error)
            {
            m_main_window.ShowError("could not create Legend object when printing",error);
            return QImage();
            }

        CartoType::Color cartotype_red(178,31,41);
        legend->SetMinLineHeight(7,"pt");
        legend->SetFontSize(7,"pt");
        legend->AddTextLine("");
        legend->SetTextColor(cartotype_red);
        legend->AddTextLine("CARTOTYPE    www.cartotype.com");
        legend->SetBorder(CartoType::KGray,1,4,"pt");
        CartoType::Color b(CartoType::KWhite);
        b.SetAlpha(200);
        legend->SetBackgroundColor(b);
        auto bs = m_framework->BlendStyleSet();
        CartoType::Result error;
        auto legend_bitmap = legend->CreateBitmap(error,2,"in",{});
        if (!error)
            {
            auto gc = print_framework->MapGraphicsContext();
            auto margin = printer.resolution() / 8; // inset the legend by 1/8 in.
            gc->DrawBitmap(*legend_bitmap,CartoType::Point(aPageRect.width() - legend_bitmap->Width() - margin,aPageRect.height() - legend_bitmap->Height() - margin));
            }

        QImage image { int(aPageRect.width()),int(aPageRect.height()),QImage::Format_ARGB32_Premultiplied };
        CopyBitmapToImage(*map_bitmap,image);
        return image;
        };
    QImage image { CreateImage(page_rect) };
    if (image.isNull())
        return;

    auto Paint = [&image,&page_rect,&CreateImage](QPrinter* aPrinter)->void
        {
        QRectF cur_page_rect { aPrinter->pageRect(QPrinter::DevicePixel) };
        if (cur_page_rect != page_rect)
            {
            image = CreateImage(cur_page_rect);
            if (image.isNull())
                return;
            page_rect = cur_page_rect;
            }

        QPainter painter;
        painter.begin(aPrinter);
        painter.drawImage(0,0,image);
        painter.end();
        };

    if (aPreview)
        {
        QPrintPreviewDialog preview(&printer,this);
        connect(&preview,&QPrintPreviewDialog::paintRequested,Paint);
        preview.exec();
        }
    else
        Paint(&printer);
    }

/** Loads a map in addition to the current map as an overlay or side-by-side tile. */
void MapForm::LoadMap(const QString& aFileName)
    {
    CartoType::String filename = aFileName.utf16();
    CartoType::Result error = m_framework->LoadMap(filename);
    bool used_built_in_profiles = m_framework->BuiltInRouteProfileCount() > 0;
    if (error)
        m_main_window.ShowError("failed to load map in current map",error);
    else
        {
        m_filename_array.push_back(aFileName);
        if (used_built_in_profiles || m_framework->BuiltInRouteProfileCount() > 0)
            SetRouteProfileIndex(0);
        }
    }
