using System;
using System.IO;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Popups;

namespace CartoTypeUWPDemo
{
    public sealed partial class MainPage : Page
    {
        private CartoType.Framework m_framework;
        private CartoType.MapRenderer m_renderer;
        private double m_map_drag_anchor_x;
        private double m_map_drag_anchor_y;
        private CartoType.Point m_last_point = new CartoType.Point();

        public MainPage()
        {
            this.InitializeComponent();

            // NOTE: assets must have build action set to 'content' to be found.
            var fp = new CartoType.FrameworkParam();
            fp.MapFileName = "Assets\\isle_of_wight.ctm1";
            bool b = File.Exists(fp.MapFileName);
            fp.StyleSheetFileName = "Assets\\standard.ctstyle";
            b = File.Exists(fp.StyleSheetFileName);
            fp.FontFileName = "Assets\\DejaVuSans.ttf";
            b = File.Exists(fp.FontFileName);
            fp.ViewWidth = 1024;
            fp.ViewHeight = 1024;
            m_framework = new CartoType.Framework(fp);
            m_framework.AnimateTransitions = true;
            m_framework.Draw3DBuildings = true;
            var metadata = m_framework.MapMetaData(0);

            GLESSwapChainPanel1.OnRender += OnRender;
            GLESSwapChainPanel1.EnableRenderLoop = true;
            GLESSwapChainPanel1.DrawInBackground = true;

            PointerPressed += OnPointerPressed;
            PointerReleased += OnPointerReleased;
            PointerMoved += OnPointerMoved;
            PointerWheelChanged += OnPointerWheelChanged;
            SizeChanged += OnSizeChanged;

            // NOTE: ordinary KeyDown function doesn't work: see https://stackoverflow.com/questions/58908845/keydown-event-doesnt-trigger-from-selected-grid-in-uwp
            Window.Current.CoreWindow.KeyDown += OnKeyDown;
        }

        void OnSizeChanged(object aSender, SizeChangedEventArgs aEvent)
        {
            if (m_framework != null)
                m_framework.Resize((int)aEvent.NewSize.Width, (int)aEvent.NewSize.Height);
        }

        void OnPointerPressed(object aSender, PointerRoutedEventArgs aEvent)
        {
            Windows.UI.Input.PointerPoint p = aEvent.GetCurrentPoint(GLESSwapChainPanel1);
            if (p.Properties.IsLeftButtonPressed)
            {
                m_map_drag_anchor_x = p.Position.X;
                m_map_drag_anchor_y = p.Position.Y;
                aEvent.Handled = true;
            }
            else if (p.Properties.IsRightButtonPressed)
            {
                var p2 = new CartoType.Point(p.Position.X, p.Position.Y);
                m_framework.ConvertPoint(p2, CartoType.CoordType.Screen, CartoType.CoordType.Degree);

                if (m_last_point.X != 0 && m_last_point.Y != 0)
                {
                    var cs = new CartoType.RouteCoordSet();
                    cs.CoordType = CartoType.CoordType.Degree;
                    var rp = new CartoType.RoutePoint();
                    rp.X = m_last_point.X;
                    rp.Y = m_last_point.Y;
                    cs.RoutePointList.Add(rp);
                    rp = new CartoType.RoutePoint();
                    rp.X = p2.X;
                    rp.Y = p2.Y;
                    cs.RoutePointList.Add(rp);
                    var result = m_framework.StartNavigation(cs);
                    if (result != CartoType.Result.Success)
                    {
                        var s = CartoType.Util.ErrorString(result);
                        var m = new MessageDialog("Error: " + s);
                        _ = m.ShowAsync();
                    }
                }
                m_last_point = p2;
            }
            else if (p.Properties.IsMiddleButtonPressed)
            {
                m_framework.Perspective = !m_framework.Perspective;
            }
        }
        void OnPointerReleased(object aSender, PointerRoutedEventArgs aEvent)
        {
            Windows.UI.Input.PointerPoint p = aEvent.GetCurrentPoint(this);
            if (p.Properties.IsLeftButtonPressed)
            {
                aEvent.Handled = true;
            }
        }

        void OnPointerMoved(object aSender, PointerRoutedEventArgs aEvent)
        {
            Windows.UI.Input.PointerPoint p = aEvent.GetCurrentPoint(this);
            if (p.Properties.IsLeftButtonPressed)
            {
                double dx = Math.Round(m_map_drag_anchor_x - p.Position.X);
                double dy = Math.Round(m_map_drag_anchor_y - p.Position.Y);
                m_framework.Pan((int)(dx), (int)(dy));
                m_map_drag_anchor_x = p.Position.X;
                m_map_drag_anchor_y = p.Position.Y;
                aEvent.Handled = true;
            }
        }

        void OnPointerWheelChanged(object aSender, PointerRoutedEventArgs aEvent)
        {
            Windows.UI.Input.PointerPoint p = aEvent.GetCurrentPoint(this);
            int zoom_count = p.Properties.MouseWheelDelta / 120;
            double zoom = Math.Sqrt(2);
            if (zoom_count == 0)
                zoom_count = p.Properties.MouseWheelDelta >= 0 ? 1 : -1;
            zoom = Math.Pow(zoom, zoom_count);
            var r = new CartoType.Rect();
            m_framework.GetView(r, CartoType.CoordType.Screen);
            if (p.Position.X >= 0 && p.Position.X < r.MaxX && p.Position.Y >= 0 && p.Position.Y < r.MaxY)
                m_framework.ZoomAt(zoom, p.Position.X, p.Position.Y, CartoType.CoordType.Screen);
            else
                m_framework.Zoom(zoom);
            aEvent.Handled = true;
        }

        void OnKeyDown(Windows.UI.Core.CoreWindow sender, Windows.UI.Core.KeyEventArgs aEvent)
        {
            switch (aEvent.VirtualKey)
            {
                case Windows.System.VirtualKey.P:
                    m_framework.Perspective = !m_framework.Perspective;
                    break;

                case Windows.System.VirtualKey.N:
                    m_framework.NightMode = !m_framework.NightMode;
                    break;

                case Windows.System.VirtualKey.R:
                    m_framework.Rotate(10);
                    break;

                case Windows.System.VirtualKey.L:
                    m_framework.Rotate(-10);
                    break;

                case Windows.System.VirtualKey.U:
                    m_framework.Rotation = 0;
                    break;

                case Windows.System.VirtualKey.S:
                    m_framework.ScaleDenominator = 100000;
                    break;

                case Windows.System.VirtualKey.E:
                    m_framework.EnableLayer("building", !m_framework.LayerIsEnabled("building"));
                    break;
            }

            aEvent.Handled = true;
        }

        void OnRender(object aSender, EventArgs aEvent)
        {
            if (m_renderer == null)
                m_renderer = new CartoType.MapRenderer(m_framework);

            m_renderer.Draw();
        }
    }
}
