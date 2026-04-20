namespace CartoTypeDemo
{
    partial class FindDialog
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
            FindText = new TextBox();
            FindList = new ListBox();
            Symbols = new CheckBox();
            FuzzyMatch = new CheckBox();
            groupBox1 = new GroupBox();
            button1 = new Button();
            button2 = new Button();
            groupBox1.SuspendLayout();
            SuspendLayout();
            // 
            // FindText
            // 
            FindText.Font = new Font("Segoe UI", 9.75F, FontStyle.Regular, GraphicsUnit.Point, 0);
            FindText.Location = new Point(178, 22);
            FindText.Name = "FindText";
            FindText.Size = new Size(590, 25);
            FindText.TabIndex = 0;
            FindText.TextChanged += FindText_TextChanged;
            FindText.KeyDown += FindText_KeyDown;
            // 
            // FindList
            // 
            FindList.FormattingEnabled = true;
            FindList.ItemHeight = 17;
            FindList.Location = new Point(178, 73);
            FindList.Name = "FindList";
            FindList.Size = new Size(590, 378);
            FindList.TabIndex = 1;
            FindList.DoubleClick += FindList_DoubleClick;
            FindList.KeyDown += FindList_KeyDown;
            // 
            // Symbols
            // 
            Symbols.AutoSize = true;
            Symbols.Location = new Point(16, 31);
            Symbols.Name = "Symbols";
            Symbols.Size = new Size(84, 24);
            Symbols.TabIndex = 2;
            Symbols.Text = "Symbols";
            Symbols.UseVisualStyleBackColor = true;
            Symbols.CheckedChanged += Symbols_CheckedChanged;
            // 
            // FuzzyMatch
            // 
            FuzzyMatch.AutoSize = true;
            FuzzyMatch.Location = new Point(16, 58);
            FuzzyMatch.Name = "FuzzyMatch";
            FuzzyMatch.Size = new Size(109, 24);
            FuzzyMatch.TabIndex = 3;
            FuzzyMatch.Text = "Fuzzy match";
            FuzzyMatch.UseVisualStyleBackColor = true;
            FuzzyMatch.CheckedChanged += FuzzyMatch_CheckedChanged;
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(Symbols);
            groupBox1.Controls.Add(FuzzyMatch);
            groupBox1.Font = new Font("Segoe UI", 11.25F, FontStyle.Regular, GraphicsUnit.Point, 0);
            groupBox1.Location = new Point(21, 22);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new Size(141, 101);
            groupBox1.TabIndex = 4;
            groupBox1.TabStop = false;
            groupBox1.Text = "Find options";
            // 
            // button1
            // 
            button1.DialogResult = DialogResult.OK;
            button1.Location = new Point(69, 380);
            button1.Name = "button1";
            button1.Size = new Size(93, 27);
            button1.TabIndex = 5;
            button1.Text = "OK";
            button1.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            button2.DialogResult = DialogResult.Cancel;
            button2.Location = new Point(69, 422);
            button2.Name = "button2";
            button2.Size = new Size(93, 29);
            button2.TabIndex = 6;
            button2.Text = "Cancel";
            button2.UseVisualStyleBackColor = true;
            // 
            // FindDialog
            // 
            AutoScaleDimensions = new SizeF(7F, 17F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(793, 477);
            Controls.Add(button2);
            Controls.Add(button1);
            Controls.Add(groupBox1);
            Controls.Add(FindList);
            Controls.Add(FindText);
            Font = new Font("Segoe UI", 9.75F, FontStyle.Regular, GraphicsUnit.Point, 0);
            Name = "FindDialog";
            Text = "Find a place";
            Shown += FindDialog_Shown;
            groupBox1.ResumeLayout(false);
            groupBox1.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        internal TextBox FindText;
        internal ListBox FindList;
        internal CheckBox Symbols;
        internal CheckBox FuzzyMatch;
        private GroupBox groupBox1;
        private Button button1;
        private Button button2;
    }
}