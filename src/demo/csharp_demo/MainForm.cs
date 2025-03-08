using System;
using System.Windows.Forms;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace CartoTypeDemo
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            IsMdiContainer = true;
            InitializeComponent();

            // Strip the path down to a string ending just before \src\; if no such pattern is found, use the app path
            m_cartotype_source_path = m_app_path;
            int n = m_cartotype_source_path.LastIndexOf("\\src\\");
            if (n != -1)
                m_cartotype_source_path = m_cartotype_source_path.Substring(0, n + 1);
            if (m_cartotype_source_path.Length == 0 || m_cartotype_source_path[m_cartotype_source_path.Length - 1] != '\\')
                m_cartotype_source_path += '\\';

            /*
            Find the default style sheet.
            Paths are searched in this order:
            the application directory;
            the subdirectory "style" of the application directory;
            the directory "CartoType/src/style" in the development tree.
            */
            string style_sheet_name = "standard.ctstyle";
            m_default_style_sheet_path = m_app_path + style_sheet_name;
            if (!File.Exists(m_default_style_sheet_path))
            {
                m_default_style_sheet_path = m_app_path + "style\\" + style_sheet_name;
                if (!File.Exists(m_default_style_sheet_path))
                    m_default_style_sheet_path = m_cartotype_source_path + "style\\" + style_sheet_name;
            }

        }
        private void openMenuItem_Click(object sender, EventArgs e)
        {
            var open_file_dialog = new OpenFileDialog();
            open_file_dialog.Title = "Open a map";
            open_file_dialog.InitialDirectory = "c:\\";
            open_file_dialog.Filter = "CartoType maps (CTM1 files)|*.ctm1";

            if (open_file_dialog.ShowDialog() == DialogResult.OK)
            {
                var map_form = new MapForm();
                map_form.MdiParent = this;
                map_form.WindowState = FormWindowState.Maximized;
                if (map_form.Construct(open_file_dialog.FileName, ""))
                    map_form.Show();
                else
                    map_form.Close();
            }
        }

        private void openOnlineMapMenuItem_Click(object sender, EventArgs e)
        {
            var dialog = new OnlineMapDialog();
            dialog.URL.Text = "https://api.mapbox.com/v4/mapbox.mapbox-streets-v8";
            if (dialog.ShowDialog() == DialogResult.OK)
            {
                var map_form = new MapForm();
                map_form.MdiParent = this;
                map_form.WindowState = FormWindowState.Maximized;
                if (map_form.Construct(dialog.URL.Text, dialog.Key.Text))
                    map_form.Show();
                else
                    map_form.Close();
            }
        }

        private void quitMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void aboutCartoTypeMenuItem_Click(object sender, EventArgs e)
        {
            m_about_dialog.aboutText.Text = "";

            Font plain_font = m_about_dialog.aboutText.Font;
            var bold_font = new Font(m_about_dialog.aboutText.Font, FontStyle.Bold);
            m_about_dialog.aboutText.SelectionFont = bold_font;
            m_about_dialog.aboutText.AppendText("CartoType");
            m_about_dialog.aboutText.SelectionFont = plain_font;
            m_about_dialog.aboutText.AppendText(" allows you to view maps, calculate routes, find addresses and points of interest, and add your own data.\r\n\r\n" +
                   "See cartotype.com for information about licensing CartoType.\r\n\r\n" +
                   "This application was created using\r\nCartoType " + CartoType.Util.Description() + ".");

            m_about_dialog.aboutText.AppendText("\r\n\r\nTo pan the map use mouse left-click plus drag.\r\nTo zoom the map use the mouse wheel." +
                "\r\nTo rotate the map use CTRL plus mouse wheel.\r\nTo choose the start or end of a route use mouse right-click.");

            var map_form = ActiveMdiChild as MapForm;
            if (map_form != null)
            {
                var framework = map_form.Framework();
                var m = framework.MapMetaData(0);
                if (m != null)
                {
                    string s = "\r\n\r\nMap " + m.DataSetName;
                    if (m.CartoTypeVersionMajor > 0)
                        s += " created by CartoType " + m.CartoTypeVersionMajor + "." + m.CartoTypeVersionMinor + "." + m.CartoTypeBuild;
                    if (m.FileVersionMajor > 0)
                        s += ".\r\nCTM1 version " + m.FileVersionMajor + "." + m.FileVersionMinor;
                    s += ".\r\nProjection: " + m.ProjectionName + ".";

                    m_about_dialog.aboutText.AppendText(s);
                }
            }

            m_about_dialog.ShowDialog();
        }

        public string DefaultStyleSheetPath() { return m_default_style_sheet_path; }

        public string FontPath(string aFileName)
        {
            var font_path = m_app_path + aFileName;
            if (!File.Exists(font_path))
            {
                font_path = m_app_path + "font\\" + aFileName;
                if (!File.Exists(font_path))
                    font_path = m_cartotype_source_path + "font\\" + aFileName;
            }
            return font_path;
        }

        private string m_app_path = Application.StartupPath;
        private string m_cartotype_source_path;
        private string m_default_style_sheet_path;
        private AboutDialog m_about_dialog = new();
    }
}
