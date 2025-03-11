namespace CartoTypeDemo
{
    partial class AboutDialog
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
            pictureBox1 = new PictureBox();
            aboutText = new RichTextBox();
            ((System.ComponentModel.ISupportInitialize)pictureBox1).BeginInit();
            SuspendLayout();
            // 
            // pictureBox1
            // 
            pictureBox1.BackColor = SystemColors.Window;
            pictureBox1.Image = Properties.Resources.CT_wordmark_RGB;
            pictureBox1.Location = new Point(25, 30);
            pictureBox1.Name = "pictureBox1";
            pictureBox1.Size = new Size(477, 59);
            pictureBox1.SizeMode = PictureBoxSizeMode.Zoom;
            pictureBox1.TabIndex = 0;
            pictureBox1.TabStop = false;
            // 
            // aboutText
            // 
            aboutText.BackColor = SystemColors.Window;
            aboutText.BorderStyle = BorderStyle.None;
            aboutText.DetectUrls = false;
            aboutText.Font = new Font("Lucida Sans Unicode", 11.25F, FontStyle.Regular, GraphicsUnit.Point, 0);
            aboutText.Location = new Point(25, 105);
            aboutText.Name = "aboutText";
            aboutText.ReadOnly = true;
            aboutText.Size = new Size(477, 344);
            aboutText.TabIndex = 1;
            aboutText.Text = "";
            // 
            // AboutDialog
            // 
            AutoScaleDimensions = new SizeF(7F, 17F);
            AutoScaleMode = AutoScaleMode.Font;
            BackColor = SystemColors.Window;
            ClientSize = new Size(527, 475);
            Controls.Add(aboutText);
            Controls.Add(pictureBox1);
            Font = new Font("Segoe UI", 9.75F, FontStyle.Regular, GraphicsUnit.Point, 0);
            Name = "AboutDialog";
            Text = "About CartoType";
            ((System.ComponentModel.ISupportInitialize)pictureBox1).EndInit();
            ResumeLayout(false);
        }

        #endregion

        private PictureBox pictureBox1;
        internal RichTextBox aboutText;
    }
}