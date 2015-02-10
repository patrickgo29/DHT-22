namespace Temperature_Monitor
{
    partial class MainForm
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
            this.components = new System.ComponentModel.Container();
            this.panel = new System.Windows.Forms.Panel();
            this.btnConnect = new System.Windows.Forms.Button();
            this.cbPorts = new System.Windows.Forms.ComboBox();
            this.lblPorts = new System.Windows.Forms.Label();
            this.lblTemperature = new System.Windows.Forms.Label();
            this.lblHumidity = new System.Windows.Forms.Label();
            this.lblTempReading = new System.Windows.Forms.Label();
            this.lblHumReading = new System.Windows.Forms.Label();
            this.tempGraph = new ZedGraph.ZedGraphControl();
            this.humGraph = new ZedGraph.ZedGraphControl();
            this.pnlReading = new System.Windows.Forms.Panel();
            this.panel.SuspendLayout();
            this.pnlReading.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel
            // 
            this.panel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel.Controls.Add(this.btnConnect);
            this.panel.Controls.Add(this.cbPorts);
            this.panel.Controls.Add(this.lblPorts);
            this.panel.Location = new System.Drawing.Point(25, 12);
            this.panel.Name = "panel";
            this.panel.Size = new System.Drawing.Size(422, 37);
            this.panel.TabIndex = 0;
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(342, 7);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(75, 23);
            this.btnConnect.TabIndex = 2;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // cbPorts
            // 
            this.cbPorts.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbPorts.FormattingEnabled = true;
            this.cbPorts.Location = new System.Drawing.Point(69, 7);
            this.cbPorts.Name = "cbPorts";
            this.cbPorts.Size = new System.Drawing.Size(267, 21);
            this.cbPorts.TabIndex = 1;
            this.cbPorts.DropDown += new System.EventHandler(this.DropDown);
            // 
            // lblPorts
            // 
            this.lblPorts.AutoSize = true;
            this.lblPorts.Location = new System.Drawing.Point(8, 10);
            this.lblPorts.Name = "lblPorts";
            this.lblPorts.Size = new System.Drawing.Size(55, 13);
            this.lblPorts.TabIndex = 0;
            this.lblPorts.Text = "Serial Port";
            // 
            // lblTemperature
            // 
            this.lblTemperature.AutoSize = true;
            this.lblTemperature.Location = new System.Drawing.Point(307, 12);
            this.lblTemperature.Name = "lblTemperature";
            this.lblTemperature.Size = new System.Drawing.Size(70, 13);
            this.lblTemperature.TabIndex = 1;
            this.lblTemperature.Text = "Temperature:";
            // 
            // lblHumidity
            // 
            this.lblHumidity.AutoSize = true;
            this.lblHumidity.Location = new System.Drawing.Point(3, 10);
            this.lblHumidity.Name = "lblHumidity";
            this.lblHumidity.Size = new System.Drawing.Size(92, 13);
            this.lblHumidity.TabIndex = 2;
            this.lblHumidity.Text = "Relative Humidity:";
            // 
            // lblTempReading
            // 
            this.lblTempReading.AutoSize = true;
            this.lblTempReading.Location = new System.Drawing.Point(101, 10);
            this.lblTempReading.Name = "lblTempReading";
            this.lblTempReading.Size = new System.Drawing.Size(24, 13);
            this.lblTempReading.TabIndex = 3;
            this.lblTempReading.Text = "0°C";
            // 
            // lblHumReading
            // 
            this.lblHumReading.AutoSize = true;
            this.lblHumReading.Location = new System.Drawing.Point(383, 12);
            this.lblHumReading.Name = "lblHumReading";
            this.lblHumReading.Size = new System.Drawing.Size(21, 13);
            this.lblHumReading.TabIndex = 4;
            this.lblHumReading.Text = "0%";
            // 
            // tempGraph
            // 
            this.tempGraph.Location = new System.Drawing.Point(25, 66);
            this.tempGraph.Name = "tempGraph";
            this.tempGraph.ScrollGrace = 0D;
            this.tempGraph.ScrollMaxX = 0D;
            this.tempGraph.ScrollMaxY = 0D;
            this.tempGraph.ScrollMaxY2 = 0D;
            this.tempGraph.ScrollMinX = 0D;
            this.tempGraph.ScrollMinY = 0D;
            this.tempGraph.ScrollMinY2 = 0D;
            this.tempGraph.Size = new System.Drawing.Size(422, 251);
            this.tempGraph.TabIndex = 5;
            // 
            // humGraph
            // 
            this.humGraph.Location = new System.Drawing.Point(466, 66);
            this.humGraph.Name = "humGraph";
            this.humGraph.ScrollGrace = 0D;
            this.humGraph.ScrollMaxX = 0D;
            this.humGraph.ScrollMaxY = 0D;
            this.humGraph.ScrollMaxY2 = 0D;
            this.humGraph.ScrollMinX = 0D;
            this.humGraph.ScrollMinY = 0D;
            this.humGraph.ScrollMinY2 = 0D;
            this.humGraph.Size = new System.Drawing.Size(422, 251);
            this.humGraph.TabIndex = 6;
            // 
            // pnlReading
            // 
            this.pnlReading.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnlReading.Controls.Add(this.lblHumidity);
            this.pnlReading.Controls.Add(this.lblHumReading);
            this.pnlReading.Controls.Add(this.lblTemperature);
            this.pnlReading.Controls.Add(this.lblTempReading);
            this.pnlReading.Location = new System.Drawing.Point(466, 12);
            this.pnlReading.Name = "pnlReading";
            this.pnlReading.Size = new System.Drawing.Size(422, 37);
            this.pnlReading.TabIndex = 7;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(914, 338);
            this.Controls.Add(this.pnlReading);
            this.Controls.Add(this.humGraph);
            this.Controls.Add(this.tempGraph);
            this.Controls.Add(this.panel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.Text = "Temperature and Humidity";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.panel.ResumeLayout(false);
            this.panel.PerformLayout();
            this.pnlReading.ResumeLayout(false);
            this.pnlReading.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.ComboBox cbPorts;
        private System.Windows.Forms.Label lblPorts;
        private System.Windows.Forms.Label lblTemperature;
        private System.Windows.Forms.Label lblHumidity;
        private System.Windows.Forms.Label lblTempReading;
        private System.Windows.Forms.Label lblHumReading;
        private ZedGraph.ZedGraphControl tempGraph;
        private ZedGraph.ZedGraphControl humGraph;
        private System.Windows.Forms.Panel pnlReading;
    }
}

