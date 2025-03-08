namespace CartoTypeDemo
{
    partial class MapForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            mapMenu = new MenuStrip();
            fileMenu = new ToolStripMenuItem();
            openInCurrentMapMenuItem = new ToolStripMenuItem();
            findMenu = new ToolStripMenuItem();
            findMenuItem = new ToolStripMenuItem();
            findNextMenuItem = new ToolStripMenuItem();
            findAddressMenuItem = new ToolStripMenuItem();
            viewMenu = new ToolStripMenuItem();
            scaleMenuItem = new ToolStripMenuItem();
            legendMenuItem = new ToolStripMenuItem();
            northUpMenuItem = new ToolStripMenuItem();
            perspectiveViewMenuItem = new ToolStripMenuItem();
            metricUnitsMenuItem = new ToolStripMenuItem();
            threeDBuildingsMenuItem = new ToolStripMenuItem();
            nightModeMenuItem = new ToolStripMenuItem();
            routeMenu = new ToolStripMenuItem();
            reverseRouteMenuItem = new ToolStripMenuItem();
            deleteRouteMenuItem = new ToolStripMenuItem();
            toolStripSeparator1 = new ToolStripSeparator();
            driveMenuItem = new ToolStripMenuItem();
            cycleMenuItem = new ToolStripMenuItem();
            walkMenuItem = new ToolStripMenuItem();
            hikeMenuItem = new ToolStripMenuItem();
            styleMenu = new ToolStripMenuItem();
            chooseStyleSheetMenuItem = new ToolStripMenuItem();
            reloadStyleSheetMenuItem = new ToolStripMenuItem();
            toolStripSeparator2 = new ToolStripSeparator();
            saveImageAsPNGFileMenuItem = new ToolStripMenuItem();
            mapMenu.SuspendLayout();
            SuspendLayout();
            // 
            // mapMenu
            // 
            mapMenu.Items.AddRange(new ToolStripItem[] { fileMenu, findMenu, viewMenu, routeMenu, styleMenu });
            mapMenu.Location = new Point(0, 0);
            mapMenu.Name = "mapMenu";
            mapMenu.Size = new Size(800, 24);
            mapMenu.TabIndex = 0;
            mapMenu.Text = "mapMenu";
            // 
            // fileMenu
            // 
            fileMenu.DropDownItems.AddRange(new ToolStripItem[] { openInCurrentMapMenuItem, toolStripSeparator2, saveImageAsPNGFileMenuItem });
            fileMenu.MergeAction = MergeAction.MatchOnly;
            fileMenu.Name = "fileMenu";
            fileMenu.Size = new Size(37, 20);
            fileMenu.Text = "File";
            // 
            // openInCurrentMapMenuItem
            // 
            openInCurrentMapMenuItem.MergeAction = MergeAction.Insert;
            openInCurrentMapMenuItem.MergeIndex = 1;
            openInCurrentMapMenuItem.Name = "openInCurrentMapMenuItem";
            openInCurrentMapMenuItem.Size = new Size(205, 22);
            openInCurrentMapMenuItem.Text = "Open in Current Map...";
            openInCurrentMapMenuItem.Click += openInCurrentMapMenuItem_Click;
            // 
            // findMenu
            // 
            findMenu.DropDownItems.AddRange(new ToolStripItem[] { findMenuItem, findNextMenuItem, findAddressMenuItem });
            findMenu.MergeAction = MergeAction.Insert;
            findMenu.MergeIndex = 1;
            findMenu.Name = "findMenu";
            findMenu.Size = new Size(42, 20);
            findMenu.Text = "Find";
            // 
            // findMenuItem
            // 
            findMenuItem.Name = "findMenuItem";
            findMenuItem.ShortcutKeys = Keys.Control | Keys.F;
            findMenuItem.Size = new Size(167, 22);
            findMenuItem.Text = "Find...";
            findMenuItem.Click += findMenuItem_Click;
            // 
            // findNextMenuItem
            // 
            findNextMenuItem.Enabled = false;
            findNextMenuItem.Name = "findNextMenuItem";
            findNextMenuItem.ShortcutKeys = Keys.Control | Keys.G;
            findNextMenuItem.Size = new Size(167, 22);
            findNextMenuItem.Text = "Find Next";
            findNextMenuItem.Click += findNextMenuItem_Click;
            // 
            // findAddressMenuItem
            // 
            findAddressMenuItem.Name = "findAddressMenuItem";
            findAddressMenuItem.Size = new Size(167, 22);
            findAddressMenuItem.Text = "Find Address...";
            findAddressMenuItem.Click += findAddressMenuItem_Click;
            // 
            // viewMenu
            // 
            viewMenu.DropDownItems.AddRange(new ToolStripItem[] { scaleMenuItem, legendMenuItem, northUpMenuItem, perspectiveViewMenuItem, metricUnitsMenuItem, threeDBuildingsMenuItem, nightModeMenuItem });
            viewMenu.MergeAction = MergeAction.Insert;
            viewMenu.MergeIndex = 2;
            viewMenu.Name = "viewMenu";
            viewMenu.Size = new Size(44, 20);
            viewMenu.Text = "View";
            // 
            // scaleMenuItem
            // 
            scaleMenuItem.CheckOnClick = true;
            scaleMenuItem.Name = "scaleMenuItem";
            scaleMenuItem.Size = new Size(162, 22);
            scaleMenuItem.Text = "Scale";
            scaleMenuItem.CheckedChanged += scaleMenuItem_CheckedChanged;
            // 
            // legendMenuItem
            // 
            legendMenuItem.CheckOnClick = true;
            legendMenuItem.Name = "legendMenuItem";
            legendMenuItem.Size = new Size(162, 22);
            legendMenuItem.Text = "Legend";
            legendMenuItem.CheckedChanged += legendMenuItem_CheckedChanged;
            // 
            // northUpMenuItem
            // 
            northUpMenuItem.Enabled = false;
            northUpMenuItem.Name = "northUpMenuItem";
            northUpMenuItem.Size = new Size(162, 22);
            northUpMenuItem.Text = "North Up";
            northUpMenuItem.Click += northUpMenuItem_Click;
            // 
            // perspectiveViewMenuItem
            // 
            perspectiveViewMenuItem.CheckOnClick = true;
            perspectiveViewMenuItem.Name = "perspectiveViewMenuItem";
            perspectiveViewMenuItem.Size = new Size(162, 22);
            perspectiveViewMenuItem.Text = "Perspective View";
            perspectiveViewMenuItem.CheckedChanged += perspectiveViewMenuItem_CheckedChanged;
            // 
            // metricUnitsMenuItem
            // 
            metricUnitsMenuItem.Checked = true;
            metricUnitsMenuItem.CheckOnClick = true;
            metricUnitsMenuItem.CheckState = CheckState.Checked;
            metricUnitsMenuItem.Name = "metricUnitsMenuItem";
            metricUnitsMenuItem.Size = new Size(162, 22);
            metricUnitsMenuItem.Text = "Metric Units";
            metricUnitsMenuItem.CheckedChanged += metricUnitsMenuItem_CheckedChanged;
            // 
            // threeDBuildingsMenuItem
            // 
            threeDBuildingsMenuItem.CheckOnClick = true;
            threeDBuildingsMenuItem.Name = "threeDBuildingsMenuItem";
            threeDBuildingsMenuItem.Size = new Size(162, 22);
            threeDBuildingsMenuItem.Text = "3D Buildings";
            threeDBuildingsMenuItem.CheckedChanged += threeDBuildingsMenuItem_CheckedChanged;
            // 
            // nightModeMenuItem
            // 
            nightModeMenuItem.CheckOnClick = true;
            nightModeMenuItem.Name = "nightModeMenuItem";
            nightModeMenuItem.Size = new Size(162, 22);
            nightModeMenuItem.Text = "Night Mode";
            nightModeMenuItem.CheckedChanged += nightModeMenuItem_CheckedChanged;
            // 
            // routeMenu
            // 
            routeMenu.DropDownItems.AddRange(new ToolStripItem[] { reverseRouteMenuItem, deleteRouteMenuItem, toolStripSeparator1, driveMenuItem, cycleMenuItem, walkMenuItem, hikeMenuItem });
            routeMenu.MergeAction = MergeAction.Insert;
            routeMenu.MergeIndex = 3;
            routeMenu.Name = "routeMenu";
            routeMenu.Size = new Size(50, 20);
            routeMenu.Text = "Route";
            routeMenu.DropDownOpening += routeMenu_DropDownOpening;
            // 
            // reverseRouteMenuItem
            // 
            reverseRouteMenuItem.Name = "reverseRouteMenuItem";
            reverseRouteMenuItem.Size = new Size(148, 22);
            reverseRouteMenuItem.Text = "Reverse Route";
            reverseRouteMenuItem.Click += reverseRouteMenuItem_Click;
            // 
            // deleteRouteMenuItem
            // 
            deleteRouteMenuItem.Name = "deleteRouteMenuItem";
            deleteRouteMenuItem.Size = new Size(148, 22);
            deleteRouteMenuItem.Text = "Delete Route";
            deleteRouteMenuItem.Click += deleteRouteMenuItem_Click;
            // 
            // toolStripSeparator1
            // 
            toolStripSeparator1.Name = "toolStripSeparator1";
            toolStripSeparator1.Size = new Size(145, 6);
            // 
            // driveMenuItem
            // 
            driveMenuItem.Checked = true;
            driveMenuItem.CheckOnClick = true;
            driveMenuItem.CheckState = CheckState.Checked;
            driveMenuItem.Name = "driveMenuItem";
            driveMenuItem.Size = new Size(148, 22);
            driveMenuItem.Text = "Drive";
            driveMenuItem.Click += driveMenuItem_Click;
            // 
            // cycleMenuItem
            // 
            cycleMenuItem.Name = "cycleMenuItem";
            cycleMenuItem.Size = new Size(148, 22);
            cycleMenuItem.Text = "Cycle";
            cycleMenuItem.Click += cycleMenuItem_Click;
            // 
            // walkMenuItem
            // 
            walkMenuItem.Name = "walkMenuItem";
            walkMenuItem.Size = new Size(148, 22);
            walkMenuItem.Text = "Walk";
            walkMenuItem.Click += walkMenuItem_Click;
            // 
            // hikeMenuItem
            // 
            hikeMenuItem.Name = "hikeMenuItem";
            hikeMenuItem.Size = new Size(148, 22);
            hikeMenuItem.Text = "Hike";
            hikeMenuItem.Click += hikeMenuItem_Click;
            // 
            // styleMenu
            // 
            styleMenu.DropDownItems.AddRange(new ToolStripItem[] { chooseStyleSheetMenuItem, reloadStyleSheetMenuItem });
            styleMenu.MergeAction = MergeAction.Insert;
            styleMenu.MergeIndex = 4;
            styleMenu.Name = "styleMenu";
            styleMenu.Size = new Size(44, 20);
            styleMenu.Text = "Style";
            // 
            // chooseStyleSheetMenuItem
            // 
            chooseStyleSheetMenuItem.Name = "chooseStyleSheetMenuItem";
            chooseStyleSheetMenuItem.Size = new Size(210, 22);
            chooseStyleSheetMenuItem.Text = "Choose Style Sheet...";
            chooseStyleSheetMenuItem.Click += chooseStyleSheetMenuItem_Click;
            // 
            // reloadStyleSheetMenuItem
            // 
            reloadStyleSheetMenuItem.Name = "reloadStyleSheetMenuItem";
            reloadStyleSheetMenuItem.ShortcutKeys = Keys.Control | Keys.L;
            reloadStyleSheetMenuItem.Size = new Size(210, 22);
            reloadStyleSheetMenuItem.Text = "Reload Style Sheet";
            reloadStyleSheetMenuItem.Click += reloadStyleSheetMenuItem_Click;
            // 
            // toolStripSeparator2
            // 
            toolStripSeparator2.Name = "toolStripSeparator2";
            toolStripSeparator2.Size = new Size(202, 6);
            // 
            // saveImageAsPNGFileMenuItem
            // 
            saveImageAsPNGFileMenuItem.Name = "saveImageAsPNGFileMenuItem";
            saveImageAsPNGFileMenuItem.Size = new Size(205, 22);
            saveImageAsPNGFileMenuItem.Text = "Save Image as PNG File...";
            saveImageAsPNGFileMenuItem.Click += saveImageAsPNGFileMenuItem_Click;
            // 
            // MapForm
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(800, 450);
            Controls.Add(mapMenu);
            MainMenuStrip = mapMenu;
            Name = "MapForm";
            Text = "MapForm";
            Load += MapForm_Load;
            ClientSizeChanged += MapForm_ClientSizeChanged;
            Paint += MapForm_Paint;
            MouseDown += MapForm_MouseDown;
            MouseMove += MapForm_MouseMove;
            MouseWheel += MapForm_MouseWheel;
            mapMenu.ResumeLayout(false);
            mapMenu.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion
        private ToolStripMenuItem fileMenu;
        private ToolStripMenuItem openInCurrentMapMenuItem;
        private ToolStripMenuItem findMenu;
        private ToolStripMenuItem viewMenu;
        private ToolStripMenuItem routeMenu;
        private ToolStripMenuItem styleMenu;
        private ToolStripMenuItem findMenuItem;
        internal ToolStripMenuItem findNextMenuItem;
        internal MenuStrip mapMenu;
        private ToolStripMenuItem findAddressMenuItem;
        private ToolStripMenuItem scaleMenuItem;
        private ToolStripMenuItem legendMenuItem;
        private ToolStripMenuItem northUpMenuItem;
        private ToolStripMenuItem perspectiveViewMenuItem;
        private ToolStripMenuItem metricUnitsMenuItem;
        private ToolStripMenuItem threeDBuildingsMenuItem;
        private ToolStripMenuItem nightModeMenuItem;
        private ToolStripMenuItem reverseRouteMenuItem;
        private ToolStripMenuItem deleteRouteMenuItem;
        private ToolStripSeparator toolStripSeparator1;
        private ToolStripMenuItem driveMenuItem;
        private ToolStripMenuItem cycleMenuItem;
        private ToolStripMenuItem walkMenuItem;
        private ToolStripMenuItem hikeMenuItem;
        private ToolStripMenuItem chooseStyleSheetMenuItem;
        private ToolStripMenuItem reloadStyleSheetMenuItem;
        private ToolStripSeparator toolStripSeparator2;
        private ToolStripMenuItem saveImageAsPNGFileMenuItem;
    }
}