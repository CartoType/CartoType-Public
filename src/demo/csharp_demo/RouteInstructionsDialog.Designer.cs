namespace CartoTypeDemo
{
    partial class RouteInstructionsDialog
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
            instructions = new TextBox();
            SuspendLayout();
            // 
            // instructions
            // 
            instructions.Location = new Point(12, 12);
            instructions.Multiline = true;
            instructions.Name = "instructions";
            instructions.ScrollBars = ScrollBars.Vertical;
            instructions.Size = new Size(465, 426);
            instructions.TabIndex = 0;
            // 
            // RouteInstructionsDialog
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(489, 450);
            Controls.Add(instructions);
            Name = "RouteInstructionsDialog";
            Text = "Route Instructions";
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        internal TextBox instructions;
    }
}