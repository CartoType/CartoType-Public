namespace CartoTypeDemo
{
    partial class FindAddressDialog
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
            label1 = new Label();
            label2 = new Label();
            label3 = new Label();
            label4 = new Label();
            label5 = new Label();
            label6 = new Label();
            button1 = new Button();
            button2 = new Button();
            Building = new TextBox();
            Street = new TextBox();
            Locality = new TextBox();
            AdminArea = new TextBox();
            Country = new TextBox();
            Postcode = new TextBox();
            SuspendLayout();
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(21, 31);
            label1.Name = "label1";
            label1.Size = new Size(192, 20);
            label1.TabIndex = 0;
            label1.Text = "Building or point of interest";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(21, 76);
            label2.Name = "label2";
            label2.Size = new Size(48, 20);
            label2.TabIndex = 1;
            label2.Text = "Street";
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(21, 123);
            label3.Name = "label3";
            label3.Size = new Size(144, 20);
            label3.TabIndex = 2;
            label3.Text = "City, town or locality";
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new Point(21, 172);
            label4.Name = "label4";
            label4.Size = new Size(173, 20);
            label4.TabIndex = 3;
            label4.Text = "State, province or county";
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Location = new Point(21, 219);
            label5.Name = "label5";
            label5.Size = new Size(60, 20);
            label5.TabIndex = 4;
            label5.Text = "Country";
            // 
            // label6
            // 
            label6.AutoSize = true;
            label6.Location = new Point(21, 264);
            label6.Name = "label6";
            label6.Size = new Size(85, 20);
            label6.TabIndex = 5;
            label6.Text = "Postal code";
            // 
            // button1
            // 
            button1.DialogResult = DialogResult.OK;
            button1.Location = new Point(554, 318);
            button1.Name = "button1";
            button1.Size = new Size(113, 34);
            button1.TabIndex = 6;
            button1.Text = "OK";
            button1.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            button2.DialogResult = DialogResult.Cancel;
            button2.Location = new Point(684, 318);
            button2.Name = "button2";
            button2.Size = new Size(109, 34);
            button2.TabIndex = 7;
            button2.Text = "Cancel";
            button2.UseVisualStyleBackColor = true;
            // 
            // Building
            // 
            Building.Location = new Point(229, 31);
            Building.Name = "Building";
            Building.Size = new Size(564, 27);
            Building.TabIndex = 8;
            // 
            // Street
            // 
            Street.Location = new Point(229, 76);
            Street.Name = "Street";
            Street.Size = new Size(564, 27);
            Street.TabIndex = 9;
            // 
            // Locality
            // 
            Locality.Location = new Point(229, 123);
            Locality.Name = "Locality";
            Locality.Size = new Size(564, 27);
            Locality.TabIndex = 10;
            // 
            // AdminArea
            // 
            AdminArea.Location = new Point(229, 172);
            AdminArea.Name = "AdminArea";
            AdminArea.Size = new Size(564, 27);
            AdminArea.TabIndex = 11;
            // 
            // Country
            // 
            Country.Location = new Point(229, 219);
            Country.Name = "Country";
            Country.Size = new Size(564, 27);
            Country.TabIndex = 12;
            // 
            // Postcode
            // 
            Postcode.Location = new Point(229, 264);
            Postcode.Name = "Postcode";
            Postcode.Size = new Size(564, 27);
            Postcode.TabIndex = 13;
            // 
            // FindAddressDialog
            // 
            AutoScaleDimensions = new SizeF(8F, 20F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(825, 384);
            Controls.Add(Postcode);
            Controls.Add(Country);
            Controls.Add(AdminArea);
            Controls.Add(Locality);
            Controls.Add(Street);
            Controls.Add(Building);
            Controls.Add(button2);
            Controls.Add(button1);
            Controls.Add(label6);
            Controls.Add(label5);
            Controls.Add(label4);
            Controls.Add(label3);
            Controls.Add(label2);
            Controls.Add(label1);
            Font = new Font("Segoe UI", 11.25F, FontStyle.Regular, GraphicsUnit.Point, 0);
            Margin = new Padding(3, 4, 3, 4);
            Name = "FindAddressDialog";
            Text = "Find Address";
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label label1;
        private Label label2;
        private Label label3;
        private Label label4;
        private Label label5;
        private Label label6;
        private Button button1;
        private Button button2;
        internal TextBox Building;
        internal TextBox Street;
        internal TextBox Locality;
        internal TextBox AdminArea;
        internal TextBox Country;
        internal TextBox Postcode;
    }
}