namespace CartoTypeDemo
{
    partial class OnlineMapDialog
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
            URL = new TextBox();
            Key = new TextBox();
            label1 = new Label();
            label2 = new Label();
            okButton = new Button();
            cancelButton = new Button();
            SuspendLayout();
            // 
            // URL
            // 
            URL.Location = new Point(145, 28);
            URL.Name = "URL";
            URL.Size = new Size(546, 25);
            URL.TabIndex = 0;
            // 
            // Key
            // 
            Key.Location = new Point(145, 79);
            Key.Name = "Key";
            Key.Size = new Size(546, 25);
            Key.TabIndex = 1;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(33, 32);
            label1.Name = "label1";
            label1.Size = new Size(31, 17);
            label1.TabIndex = 2;
            label1.Text = "URL";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Font = new Font("Segoe UI", 9.75F, FontStyle.Regular, GraphicsUnit.Point, 0);
            label2.Location = new Point(33, 79);
            label2.Name = "label2";
            label2.Size = new Size(49, 17);
            label2.TabIndex = 3;
            label2.Text = "API key";
            // 
            // okButton
            // 
            okButton.DialogResult = DialogResult.OK;
            okButton.Location = new Point(468, 131);
            okButton.Name = "okButton";
            okButton.Size = new Size(101, 31);
            okButton.TabIndex = 4;
            okButton.Text = "OK";
            okButton.UseVisualStyleBackColor = true;
            // 
            // cancelButton
            // 
            cancelButton.DialogResult = DialogResult.Cancel;
            cancelButton.Location = new Point(595, 131);
            cancelButton.Name = "cancelButton";
            cancelButton.Size = new Size(96, 31);
            cancelButton.TabIndex = 5;
            cancelButton.Text = "Cancel";
            cancelButton.UseVisualStyleBackColor = true;
            // 
            // OnlineMapDialog
            // 
            AutoScaleDimensions = new SizeF(7F, 17F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(739, 190);
            Controls.Add(cancelButton);
            Controls.Add(okButton);
            Controls.Add(label2);
            Controls.Add(label1);
            Controls.Add(Key);
            Controls.Add(URL);
            Font = new Font("Segoe UI", 9.75F, FontStyle.Regular, GraphicsUnit.Point, 0);
            Name = "OnlineMapDialog";
            Text = "Open an online map";
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion
        private Label label1;
        private Label label2;
        private Button okButton;
        private Button cancelButton;
        internal TextBox URL;
        internal TextBox Key;
    }
}