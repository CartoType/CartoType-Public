namespace CartoTypeDemo
{
    partial class RouteDialog
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
            startButton = new RadioButton();
            endButton = new RadioButton();
            groupBox1 = new GroupBox();
            addressText = new TextBox();
            okButton = new Button();
            cancelButton = new Button();
            groupBox1.SuspendLayout();
            SuspendLayout();
            // 
            // startButton
            // 
            startButton.AutoSize = true;
            startButton.Checked = true;
            startButton.Location = new Point(17, 25);
            startButton.Name = "startButton";
            startButton.Size = new Size(125, 21);
            startButton.TabIndex = 0;
            startButton.TabStop = true;
            startButton.Text = "Set start of route";
            startButton.UseVisualStyleBackColor = true;
            // 
            // endButton
            // 
            endButton.AutoSize = true;
            endButton.Location = new Point(17, 63);
            endButton.Name = "endButton";
            endButton.Size = new Size(121, 21);
            endButton.TabIndex = 1;
            endButton.TabStop = true;
            endButton.Text = "Set end of route";
            endButton.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(startButton);
            groupBox1.Controls.Add(endButton);
            groupBox1.Location = new Point(26, 21);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new Size(158, 107);
            groupBox1.TabIndex = 2;
            groupBox1.TabStop = false;
            // 
            // addressText
            // 
            addressText.Location = new Point(231, 31);
            addressText.Multiline = true;
            addressText.Name = "addressText";
            addressText.ReadOnly = true;
            addressText.Size = new Size(541, 97);
            addressText.TabIndex = 3;
            // 
            // okButton
            // 
            okButton.DialogResult = DialogResult.OK;
            okButton.Location = new Point(493, 146);
            okButton.Name = "okButton";
            okButton.Size = new Size(132, 38);
            okButton.TabIndex = 4;
            okButton.Text = "OK";
            okButton.UseVisualStyleBackColor = true;
            // 
            // cancelButton
            // 
            cancelButton.DialogResult = DialogResult.Cancel;
            cancelButton.Location = new Point(647, 147);
            cancelButton.Name = "cancelButton";
            cancelButton.Size = new Size(125, 37);
            cancelButton.TabIndex = 5;
            cancelButton.Text = "Cancel";
            cancelButton.UseVisualStyleBackColor = true;
            // 
            // RouteDialog
            // 
            AutoScaleDimensions = new SizeF(7F, 17F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(795, 209);
            Controls.Add(cancelButton);
            Controls.Add(okButton);
            Controls.Add(addressText);
            Controls.Add(groupBox1);
            Font = new Font("Segoe UI", 9.75F, FontStyle.Regular, GraphicsUnit.Point, 0);
            Name = "RouteDialog";
            Text = "Routing";
            groupBox1.ResumeLayout(false);
            groupBox1.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private RadioButton radioButton1;
        private RadioButton radioButton2;
        private GroupBox groupBox1;
        internal RadioButton startButton;
        internal TextBox addressText;
        internal RadioButton endButton;
        private Button okButton;
        private Button cancelButton;
    }
}