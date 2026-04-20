using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CartoTypeDemo
{
    public partial class RouteDialog : Form
    {
        public RouteDialog()
        {
            InitializeComponent();
        }

        public void UpdateVisibility()
        {
            bool p = addOrEditPushpinButton.Checked || deletePushpinButton.Checked;
            pushpinNameLabel.Visible = p;
            pushpinName.Visible = p;
            pushpinDescLabel.Visible = p;
            pushpinDesc.Visible = p;
            pushpinColorButton.Visible = p;
        }

        public Color PushPinColor()
        {
            return m_pushpin_color;
        }

        private void startButton_CheckedChanged(object sender, EventArgs e)
        {
            UpdateVisibility();
        }

        private void endButton_CheckedChanged(object sender, EventArgs e)
        {
            UpdateVisibility();
        }

        private void addOrEditPushpinButton_CheckedChanged(object sender, EventArgs e)
        {
            UpdateVisibility();
        }

        private void deletePushpinButton_CheckedChanged(object sender, EventArgs e)
        {
            UpdateVisibility();
        }

        private void pushpinColorButton_Click(object sender, EventArgs e)
        {
            var color_dialog = new ColorDialog();
            if (color_dialog.ShowDialog() == DialogResult.OK)
            {
                m_pushpin_color = color_dialog.Color;
            }
        }

        private Color m_pushpin_color = Color.Empty;
    }
}
