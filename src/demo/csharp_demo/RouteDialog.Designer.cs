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
            deletePushpinButton = new RadioButton();
            addOrEditPushpinButton = new RadioButton();
            pushpinDesc = new TextBox();
            okButton = new Button();
            cancelButton = new Button();
            address = new TextBox();
            pushpinDescLabel = new Label();
            pushpinColorButton = new Button();
            pushpinName = new TextBox();
            pushpinNameLabel = new Label();
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
            startButton.CheckedChanged += startButton_CheckedChanged;
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
            endButton.CheckedChanged += endButton_CheckedChanged;
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(deletePushpinButton);
            groupBox1.Controls.Add(addOrEditPushpinButton);
            groupBox1.Controls.Add(startButton);
            groupBox1.Controls.Add(endButton);
            groupBox1.Location = new Point(26, 12);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new Size(158, 197);
            groupBox1.TabIndex = 2;
            groupBox1.TabStop = false;
            // 
            // deletePushpinButton
            // 
            deletePushpinButton.AutoSize = true;
            deletePushpinButton.Location = new Point(17, 142);
            deletePushpinButton.Name = "deletePushpinButton";
            deletePushpinButton.Size = new Size(113, 21);
            deletePushpinButton.TabIndex = 3;
            deletePushpinButton.TabStop = true;
            deletePushpinButton.Text = "Delete pushpin";
            deletePushpinButton.UseVisualStyleBackColor = true;
            deletePushpinButton.CheckedChanged += deletePushpinButton_CheckedChanged;
            // 
            // addOrEditPushpinButton
            // 
            addOrEditPushpinButton.AutoSize = true;
            addOrEditPushpinButton.Location = new Point(17, 101);
            addOrEditPushpinButton.Name = "addOrEditPushpinButton";
            addOrEditPushpinButton.Size = new Size(100, 21);
            addOrEditPushpinButton.TabIndex = 2;
            addOrEditPushpinButton.TabStop = true;
            addOrEditPushpinButton.Text = "Add pushpin";
            addOrEditPushpinButton.UseVisualStyleBackColor = true;
            addOrEditPushpinButton.CheckedChanged += addOrEditPushpinButton_CheckedChanged;
            // 
            // pushpinDesc
            // 
            pushpinDesc.Location = new Point(357, 174);
            pushpinDesc.Name = "pushpinDesc";
            pushpinDesc.Size = new Size(415, 25);
            pushpinDesc.TabIndex = 3;
            // 
            // okButton
            // 
            okButton.DialogResult = DialogResult.OK;
            okButton.Location = new Point(497, 222);
            okButton.Name = "okButton";
            okButton.Size = new Size(132, 37);
            okButton.TabIndex = 4;
            okButton.Text = "OK";
            okButton.UseVisualStyleBackColor = true;
            // 
            // cancelButton
            // 
            cancelButton.DialogResult = DialogResult.Cancel;
            cancelButton.Location = new Point(647, 222);
            cancelButton.Name = "cancelButton";
            cancelButton.Size = new Size(125, 37);
            cancelButton.TabIndex = 5;
            cancelButton.Text = "Cancel";
            cancelButton.UseVisualStyleBackColor = true;
            // 
            // address
            // 
            address.Location = new Point(207, 21);
            address.Multiline = true;
            address.Name = "address";
            address.ReadOnly = true;
            address.Size = new Size(565, 84);
            address.TabIndex = 6;
            // 
            // pushpinDescLabel
            // 
            pushpinDescLabel.AutoSize = true;
            pushpinDescLabel.Location = new Point(207, 177);
            pushpinDescLabel.Name = "pushpinDescLabel";
            pushpinDescLabel.Size = new Size(122, 17);
            pushpinDescLabel.TabIndex = 8;
            pushpinDescLabel.Text = "Pushpin description";
            // 
            // pushpinColorButton
            // 
            pushpinColorButton.Location = new Point(207, 222);
            pushpinColorButton.Name = "pushpinColorButton";
            pushpinColorButton.Size = new Size(128, 27);
            pushpinColorButton.TabIndex = 9;
            pushpinColorButton.Text = "Pushpin color...";
            pushpinColorButton.UseVisualStyleBackColor = true;
            pushpinColorButton.Click += pushpinColorButton_Click;
            // 
            // pushpinName
            // 
            pushpinName.Location = new Point(356, 128);
            pushpinName.Name = "pushpinName";
            pushpinName.Size = new Size(416, 25);
            pushpinName.TabIndex = 10;
            // 
            // pushpinNameLabel
            // 
            pushpinNameLabel.AutoSize = true;
            pushpinNameLabel.Location = new Point(207, 131);
            pushpinNameLabel.Name = "pushpinNameLabel";
            pushpinNameLabel.Size = new Size(89, 17);
            pushpinNameLabel.TabIndex = 11;
            pushpinNameLabel.Text = "Pushpin name";
            // 
            // RouteDialog
            // 
            AutoScaleDimensions = new SizeF(7F, 17F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(795, 282);
            Controls.Add(pushpinNameLabel);
            Controls.Add(pushpinName);
            Controls.Add(pushpinColorButton);
            Controls.Add(pushpinDescLabel);
            Controls.Add(address);
            Controls.Add(cancelButton);
            Controls.Add(okButton);
            Controls.Add(pushpinDesc);
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
        private GroupBox groupBox1;
        internal RadioButton startButton;
        internal TextBox pushpinDesc;
        internal RadioButton endButton;
        private Button okButton;
        private Button cancelButton;
        internal RadioButton addOrEditPushpinButton;
        internal RadioButton deletePushpinButton;
        private Label pushpinDescLabel;
        internal TextBox address;
        internal Button pushpinColorButton;
        private Label pushpinNameLabel;
        internal TextBox pushpinName;
    }
}