namespace CartoTypeDemo
{
    partial class MainForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            mainMenu = new MenuStrip();
            fileMenu = new ToolStripMenuItem();
            openMenuItem = new ToolStripMenuItem();
            openOnlineMapMenuItem = new ToolStripMenuItem();
            quitMenuItem = new ToolStripMenuItem();
            helpMenu = new ToolStripMenuItem();
            aboutCartoTypeMenuItem = new ToolStripMenuItem();
            mainMenu.SuspendLayout();
            SuspendLayout();
            // 
            // mainMenu
            // 
            mainMenu.Items.AddRange(new ToolStripItem[] { fileMenu, helpMenu });
            mainMenu.Location = new Point(0, 0);
            mainMenu.Name = "mainMenu";
            mainMenu.Size = new Size(800, 24);
            mainMenu.TabIndex = 0;
            mainMenu.Text = "mainMenu";
            // 
            // fileMenu
            // 
            fileMenu.DropDownItems.AddRange(new ToolStripItem[] { openMenuItem, openOnlineMapMenuItem, quitMenuItem });
            fileMenu.Name = "fileMenu";
            fileMenu.Size = new Size(37, 20);
            fileMenu.Text = "File";
            fileMenu.DropDownOpening += fileMenu_DropDownOpening;
            // 
            // openMenuItem
            // 
            openMenuItem.Name = "openMenuItem";
            openMenuItem.ShortcutKeys = Keys.Control | Keys.O;
            openMenuItem.Size = new Size(180, 22);
            openMenuItem.Text = "Open...";
            openMenuItem.Click += openMenuItem_Click;
            // 
            // openOnlineMapMenuItem
            // 
            openOnlineMapMenuItem.Name = "openOnlineMapMenuItem";
            openOnlineMapMenuItem.Size = new Size(180, 22);
            openOnlineMapMenuItem.Text = "Open online map...";
            openOnlineMapMenuItem.Click += openOnlineMapMenuItem_Click;
            // 
            // quitMenuItem
            // 
            quitMenuItem.Name = "quitMenuItem";
            quitMenuItem.Size = new Size(180, 22);
            quitMenuItem.Text = "Quit";
            quitMenuItem.Click += quitMenuItem_Click;
            // 
            // helpMenu
            // 
            helpMenu.DropDownItems.AddRange(new ToolStripItem[] { aboutCartoTypeMenuItem });
            helpMenu.Name = "helpMenu";
            helpMenu.Size = new Size(44, 20);
            helpMenu.Text = "Help";
            // 
            // aboutCartoTypeMenuItem
            // 
            aboutCartoTypeMenuItem.Name = "aboutCartoTypeMenuItem";
            aboutCartoTypeMenuItem.Size = new Size(172, 22);
            aboutCartoTypeMenuItem.Text = "About CartoType...";
            aboutCartoTypeMenuItem.Click += aboutCartoTypeMenuItem_Click;
            // 
            // MainForm
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(800, 450);
            Controls.Add(mainMenu);
            Icon = (Icon)resources.GetObject("$this.Icon");
            MainMenuStrip = mainMenu;
            Name = "MainForm";
            Text = "CartoType";
            mainMenu.ResumeLayout(false);
            mainMenu.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private MenuStrip mainMenu;
        private ToolStripMenuItem fileMenu;
        private ToolStripMenuItem openMenuItem;
        private ToolStripMenuItem openOnlineMapMenuItem;
        private ToolStripMenuItem quitMenuItem;
        private ToolStripMenuItem helpMenu;
        private ToolStripMenuItem aboutCartoTypeMenuItem;
    }
}
