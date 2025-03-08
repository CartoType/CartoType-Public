using CartoType;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CartoTypeDemo
{
    public partial class MapForm : Form
    {
        public MapForm()
        {
            InitializeComponent();
            ClearRoute();
        }

        public CartoType.Framework Framework()
        {
            return m_framework;
        }

        private void ClearRoute()
        {
            m_route = null;
            m_route_coord_set = new();
            m_route_coord_set.CoordType = CoordType.Degree;
            m_route_coord_set.RoutePointList.Add(new CartoType.RoutePoint());
            m_route_coord_set.RoutePointList.Add(new CartoType.RoutePoint());
        }

        public bool Construct(string aFileName, string aKey)
        {
            m_parent = MdiParent as MainForm;
            if (m_parent == null)
                return false;

            var font = m_parent.FontPath("DejaVuSans.ttf");
            var style = m_parent.DefaultStyleSheetPath();

            try
            {
                var param = new CartoType.FrameworkParam();
                param.MapFileName = aFileName;
                param.Key = aKey;
                param.StyleSheetFileName = style;
                param.FontFileName = font;
                param.ViewWidth = ClientSize.Width;
                param.ViewHeight = ClientSize.Height;
                m_framework = new CartoType.Framework(param)!;
            }
            catch (Exception)
            {
                ShowError(0, "cannot create the CartoType framework");
            }

            if (m_framework == null)
                return false;

            // Load extra fonts
            LoadFont("DejaVuSans-Bold.ttf");
            LoadFont("DejaVuSans-Oblique.ttf");
            LoadFont("DejaVuSerif.ttf");
            LoadFont("DejaVuSerif-Bold.ttf");
            LoadFont("DejaVuSerif-Italic.ttf");
            LoadFont("DejaVuSerif-BoldItalic.ttf");
            LoadFont("DroidSansFallback.ttf");
            LoadFont("MapkeyIcons.ttf");

            m_framework.ResolutionDpi = 144;
            m_framework.FollowMode = CartoType.FollowMode.LocationHeading;
            m_framework.AnimateTransitions = true;
            Text = m_framework.DataSetName;
            return true;
        }

        private void MapForm_Paint(object sender, PaintEventArgs e)
        {
            m_map_renderer.Draw();
        }

        protected override void OnPaintBackground(PaintEventArgs e)
        {
            // do nothing: the whole window is drawn by MapForm_Paint
        }

        private void MapForm_ClientSizeChanged(object sender, EventArgs e)
        {
            if (!ClientSize.IsEmpty && m_framework != null)
            {
                m_framework.Resize(ClientSize.Width, ClientSize.Height);
            }

        }

        /** The Load event occurs before the form is displayed for the first time. */
        private void MapForm_Load(object sender, EventArgs e)
        {
            // Create the map renderer here rather than in Construct, where it would fail because the window is not yet set up.
            m_map_renderer = new CartoType.MapRenderer(m_framework, Handle);
            if (m_map_renderer == null)
            {
                throw new Exception("cannot create OpenGL map renderer");
            }

            // Hide the child window's menu because it is merged into the main menu.
            MainMenuStrip?.Hide();
        }

        private void MapForm_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                m_map_drag_anchor = new CartoType.Point(e.Location.X, e.Location.Y);
                // Convert the drag anchor to map coords so that dragging works properly in perspective mode.
                m_framework.ConvertPoint(m_map_drag_anchor, CartoType.CoordType.Display, CartoType.CoordType.Map);
            }
            else if (e.Button == MouseButtons.Right)
            {
                var p = new CartoType.Point(e.Location.X, e.Location.Y);
                m_framework.ConvertPoint(p, CartoType.CoordType.Display, CartoType.CoordType.Degree);

                // Get the address of the point clicked on.
                var address = new CartoType.Address();
                m_framework.GetAddress(address, p.X, p.Y, CartoType.CoordType.Degree);
                m_route_dialog.addressText.Text = address.ToString(true);
                m_route_dialog.addressText.Text += "\r\nlat: " + p.Y + " long: " + p.X;

                int height = m_framework.Height(p.X, p.Y, CartoType.CoordType.Degree);
                if (height > -32768)
                    m_route_dialog.addressText.Text += "\r\naltitude:" + height;

                var rp = new CartoType.RoutePoint();
                rp.X = p.X;
                rp.Y = p.Y;

                bool create_route = false;
                if (m_route_coord_set.RoutePointList[0].X == 0 && m_route_coord_set.RoutePointList[0].Y == 0)
                    m_route_dialog.startButton.Checked = true;
                else
                    m_route_dialog.endButton.Checked = true;

                if (m_route_dialog.ShowDialog() == DialogResult.OK)
                {
                    if (m_route_dialog.startButton.Checked)
                        m_route_coord_set.RoutePointList[0] = rp;
                    else if (m_route_dialog.endButton.Checked)
                        m_route_coord_set.RoutePointList[m_route_coord_set.RoutePointList.Count - 1] = rp;
                    create_route = true;
                }

                if (create_route)
                    CalculateAndDisplayRoute();
            }
        }

        private void CreateRouteAsyncCallback(CartoType.Result aResult, CartoType.Route aRoute)
        {
            // Using Invoke allows us to make changes to the data in this form from another thread without race conditions.
            Invoke(new MethodInvoker(delegate
            {
                if (aResult == Result.Success && aRoute != null)
                {
                    aResult = m_framework.UseRoute(aRoute,true);
                    m_route = aRoute;
                }

                if (aResult != Result.Success)
                    ShowError(aResult, "routing error");
            }));
        }

        private void CalculateAndDisplayRoute()
        {
            // Do nothing if either of the route points are (0,0), which by convention is the illegal position.
            if (m_route_coord_set.RoutePointList[0].X == 0 && m_route_coord_set.RoutePointList[0].Y == 0)
                return;
            int last = m_route_coord_set.RoutePointList.Count - 1;
            if (m_route_coord_set.RoutePointList[last].X == 0 && m_route_coord_set.RoutePointList[last].Y == 0)
                return;

            var profile = m_framework.Profile(0);
            m_framework.CreateRouteAsync(CreateRouteAsyncCallback, profile, m_route_coord_set, true);
        }

        private void MapForm_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                m_framework.Pan(m_map_drag_anchor.X, m_map_drag_anchor.Y, CartoType.CoordType.Map,
                    e.Location.X, e.Location.Y, CartoType.CoordType.Display);
                m_map_drag_anchor = new CartoType.Point(e.Location.X, e.Location.Y);
                m_framework.ConvertPoint(m_map_drag_anchor, CartoType.CoordType.Display, CartoType.CoordType.Map);
            }
        }

        private void MapForm_MouseWheel(object sender, MouseEventArgs e)
        {
            var pos = e.Location;

            // Use CTRL + mouse wheel to rotate.
            if (Control.ModifierKeys == Keys.Control)
            {
                double rotation_degrees = e.Delta / 12.0;
                if (ClientRectangle.Contains(pos))
                    m_framework.RotateAt(rotation_degrees, pos.X, pos.Y, CartoType.CoordType.Screen);
                else
                    m_framework.Rotate(rotation_degrees);
                northUpMenuItem.Enabled = m_framework.Rotation != 0;
            }

            // Use the mouse wheel to zoom.
            else
            {
                int zoom_count = e.Delta / 120;
                double zoom = Math.Sqrt(2);
                if (zoom_count == 0)
                    zoom_count = e.Delta >= 0 ? 1 : -1;
                zoom = Math.Pow(zoom, zoom_count);
                if (ClientRectangle.Contains(pos))
                    m_framework.ZoomAt(zoom, pos.X, pos.Y, CartoType.CoordType.Screen);
                else
                    m_framework.Zoom(zoom);
            }
        }

        private void openInCurrentMapMenuItem_Click(object sender, EventArgs e)
        {
            var open_file_dialog = new OpenFileDialog();
            open_file_dialog.Title = "Add a map to the current map";
            open_file_dialog.InitialDirectory = "c:\\";
            open_file_dialog.Filter = "CartoType maps (CTM1 files)|*.ctm1";

            if (open_file_dialog.ShowDialog() == DialogResult.OK)
            {
                var result = m_framework.LoadMap(open_file_dialog.FileName);
                if (result != CartoType.Result.Success)
                    ShowError(result, "cannot load map " + open_file_dialog.FileName);
            }
        }

        private void saveImageAsPNGFileMenuItem_Click(object sender, EventArgs e)
        {
            var save_file_dialog = new SaveFileDialog();
            save_file_dialog.Title = "Save image as PNG";
            save_file_dialog.Filter = "PNG files|*.png";
            if (save_file_dialog.ShowDialog() == DialogResult.OK)
            {
                var error = m_framework.WriteMapImage(save_file_dialog.FileName, CartoType.FileType.Png, true);
                if (error != CartoType.Result.Success)
                    ShowError(error,"cannot write PNG file" + save_file_dialog.FileName);
            }
        }

        private void findMenuItem_Click(object sender, EventArgs e)
        {
            m_find_dialog.Set(m_framework);

            while (m_find_dialog.ShowDialog() == DialogResult.OK)
            {
                if (m_find_dialog.FindText.Text.Length == 0)
                    continue;

                if (m_find_dialog.FoundObjectList().Count > 0)
                {
                    m_found_object_list = m_find_dialog.FoundObjectList();
                    long count = 0;
                    m_framework.DeleteMapObjects(0, m_found_object_id, m_found_object_id, ref count, null);
                    m_found_object_index = 0;
                    findNextMenuItem.Enabled = m_found_object_list.Count > 1;
                    ShowNextFoundObject();
                    break;
                }

                MessageBox.Show(m_find_dialog.FindText.Text + " not found");
                m_find_dialog.FindText.Focus();
                continue;
            }
        }

        private void findAddressMenuItem_Click(object sender, EventArgs e)
        {
            while (m_find_address_dialog.ShowDialog() == DialogResult.OK)
            {
                CartoType.Address address = new()
                {
                    Building = m_find_address_dialog.Building.Text,
                    Street = m_find_address_dialog.Street.Text,
                    Locality = m_find_address_dialog.Locality.Text,
                    AdminArea = m_find_address_dialog.AdminArea.Text,
                    Country = m_find_address_dialog.Country.Text,
                    PostCode = m_find_address_dialog.Postcode.Text
                };

                CartoType.MapObjectList addresses = new();
                var error = m_framework.FindAddress(addresses, 64, address, true);
                if (error != Result.Success || addresses.Count == 0)
                {
                    ShowError(error, "address not found");
                    if (error != Result.Success)
                        break;
                }
                else
                {
                    m_found_object_list = addresses;
                    m_found_object_index = 0;
                    findNextMenuItem.Enabled = m_found_object_list.Count > 1;
                    ShowNextFoundObject();
                    break;
                }
            }
        }

        private void findNextMenuItem_Click(object sender, EventArgs e)
        {
            ShowNextFoundObject();
        }

        private void scaleMenuItem_CheckedChanged(object sender, EventArgs e)
        {
            EnableDrawScale(scaleMenuItem.Checked);
        }

        private void legendMenuItem_CheckedChanged(object sender, EventArgs e)
        {
            EnableDrawLegend(legendMenuItem.Checked);
        }

        private void northUpMenuItem_Click(object sender, EventArgs e)
        {
            m_framework.Rotation = 0;
            northUpMenuItem.Enabled = false;
        }

        private void perspectiveViewMenuItem_CheckedChanged(object sender, EventArgs e)
        {
            m_framework.Perspective = perspectiveViewMenuItem.Checked;
        }

        private void metricUnitsMenuItem_CheckedChanged(object sender, EventArgs e)
        {
            m_framework.MetricUnits = metricUnitsMenuItem.Checked;
            CreateLegend();
        }

        private void threeDBuildingsMenuItem_CheckedChanged(object sender, EventArgs e)
        {
            m_framework.Draw3DBuildings = threeDBuildingsMenuItem.Checked;
        }

        private void nightModeMenuItem_CheckedChanged(object sender, EventArgs e)
        {
            m_framework.NightMode = nightModeMenuItem.Checked;
        }

        private void routeMenu_DropDownOpening(object sender, EventArgs e)
        {
            reverseRouteMenuItem.Enabled = m_route != null;
            deleteRouteMenuItem.Enabled = m_route != null;
        }

        private void reverseRouteMenuItem_Click(object sender, EventArgs e)
        {
            var error = m_framework.ReverseRoutes();
            if (error == Result.Success)
                m_route_coord_set.RoutePointList.Reverse();
        }

        private void deleteRouteMenuItem_Click(object sender, EventArgs e)
        {
            ClearRoute();
            m_framework.DeleteRoutes();
        }

        private void driveMenuItem_Click(object sender, EventArgs e)
        {
            driveMenuItem.Checked = true;
            cycleMenuItem.Checked = false;
            walkMenuItem.Checked = false;
            hikeMenuItem.Checked = false;
            m_framework.SetMainProfile(new RouteProfile(RouteProfileType.Car));
            CalculateAndDisplayRoute();
        }

        private void cycleMenuItem_Click(object sender, EventArgs e)
        {
            driveMenuItem.Checked = false;
            cycleMenuItem.Checked = true;
            walkMenuItem.Checked = false;
            hikeMenuItem.Checked = false;
            m_framework.SetMainProfile(new RouteProfile(RouteProfileType.Cycle));
            CalculateAndDisplayRoute();
        }

        private void walkMenuItem_Click(object sender, EventArgs e)
        {
            driveMenuItem.Checked = false;
            cycleMenuItem.Checked = false;
            walkMenuItem.Checked = true;
            hikeMenuItem.Checked = false;
            m_framework.SetMainProfile(new RouteProfile(RouteProfileType.Walk));
            CalculateAndDisplayRoute();
        }

        private void hikeMenuItem_Click(object sender, EventArgs e)
        {
            driveMenuItem.Checked = false;
            cycleMenuItem.Checked = false;
            walkMenuItem.Checked = false;
            hikeMenuItem.Checked = true;
            m_framework.SetMainProfile(new RouteProfile(RouteProfileType.Hike));
            CalculateAndDisplayRoute();
        }

        private void chooseStyleSheetMenuItem_Click(object sender, EventArgs e)
        {
            var open_file_dialog = new OpenFileDialog();
            open_file_dialog.Title = "Choose a style sheet";
            open_file_dialog.InitialDirectory = "c:\\";
            open_file_dialog.Filter = "CartoType style sheets|*.ctstyle|All files|*.*";

            if (open_file_dialog.ShowDialog() == DialogResult.OK)
            {
                var result = m_framework.SetStyleSheet(open_file_dialog.FileName, 0);
                if (result == CartoType.Result.Success)
                    CreateLegend();
                else
                    ShowError(result, "cannot load style sheet " + open_file_dialog.FileName);
            }
        }

        private void reloadStyleSheetMenuItem_Click(object sender, EventArgs e)
        {
            m_framework.ReloadStyleSheet(0);
            CreateLegend();
        }

        private void ShowNextFoundObject()
        {
            var map_object = m_found_object_list[m_found_object_index];
            m_framework.InsertEnvelopeMapObject(0, "found", new CartoType.Geometry(map_object),
                5, CartoType.CoordType.MapMeter, "", null, ref m_found_object_id, true);
            m_framework.SetView(map_object, 64, 1000);
            m_found_object_index++;
            if (m_found_object_index >= m_found_object_list.Count)
                m_found_object_index = 0;
        }

        private void LoadFont(string aFileName)
        {
            var result = m_framework.LoadFont(m_parent.FontPath(aFileName));
            if (result != CartoType.Result.Success)
                ShowError(result, "unable to load font " + aFileName);
        }

        private void ShowError(CartoType.Result aError, string aMessage)
        {
            string s = aMessage + ": " + CartoType.Util.ErrorString(aError);
            MessageBox.Show(s, "CartoType error");
        }

        private void CreateLegend()
        {
            uint style = CartoType.LegendStyleFlag.Title | CartoType.LegendStyleFlag.ScaleInTitle;
            if (m_draw_legend)
                style |= CartoType.LegendStyleFlag.MapObjects;
            if (m_draw_scale)
                style |= CartoType.LegendStyleFlag.ScaleBar;
            var legend = new CartoType.Legend(m_framework, style);
            var pos = new CartoType.NoticePosition(CartoType.NoticeAnchor.TopRight, 2.5, "in");
            m_framework.SetLegend(pos, legend);
            m_framework.LegendEnabled = m_draw_legend || m_draw_scale;
        }

        void EnableDrawScale(bool aEnable)
        {
            if (aEnable != m_draw_scale)
            {
                m_draw_scale = aEnable;
                CreateLegend();
            }
        }

        void EnableDrawLegend(bool aEnable)
        {
            if (aEnable != m_draw_legend)
            {
                m_draw_legend = aEnable;
                CreateLegend();
            }
        }

        private MainForm m_parent;
        private CartoType.Framework m_framework;
        private CartoType.MapRenderer m_map_renderer;
        private CartoType.Point m_map_drag_anchor = new();
        private FindDialog m_find_dialog = new();
        private FindAddressDialog m_find_address_dialog = new();
        private CartoType.MapObjectList m_found_object_list = [];
        private int m_found_object_index = 0;
        private long m_found_object_id = 0;
        private bool m_draw_scale = false;
        private bool m_draw_legend = false;
        private RouteDialog m_route_dialog = new();
        private CartoType.RouteCoordSet m_route_coord_set;
        private CartoType.Route? m_route;
    }
}
