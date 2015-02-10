using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;
using ZedGraph;

namespace Temperature_Monitor
{
    public partial class MainForm : Form
    {
        int tickStart = 0;
        bool connected = false;
        int logStart = 0;
        SerialPort port;
        SerialDataReceivedEventHandler handler;

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            PopulatePortList();
        }

        private void PopulatePortList()
        {
            string[] ports = SerialPort.GetPortNames();
            cbPorts.DataSource = ports;
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if (connected)
            {
                Disconnect();
            }
            else
            {
                Connect();
                CreateGraph(tempGraph, "", "Time (s)", "Temperature (°C)", "temperature");
                CreateGraph(humGraph, "", "Time (s)", "Relative Humidity (%)", "humidity");
            }
        }

        private void Connect()
        {
            string portName = cbPorts.SelectedValue.ToString();
            try
            {
                OpenPort(portName);
                panel.Invoke((MethodInvoker)delegate
                {
                    connected = true;
                    SetConnectedControls(true);
                });
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void SetConnectedControls(bool connected){
            btnConnect.Text = connected ? "Disconnect" : "Connect";
            cbPorts.Enabled = !connected;
        }

        private void OpenPort(string portName)
        {
            port = new SerialPort();
            port = new SerialPort(portName, 9600, Parity.None, 8, StopBits.One);
            port.Open();
            handler = new SerialDataReceivedEventHandler(SerialDataReceived);
            port.DataReceived += handler;
        }

        private void Disconnect()
        {
            ClosePort();
            panel.Invoke((MethodInvoker)delegate
            {
                connected = false;
                SetConnectedControls(false);
            });
        }

        private void ClosePort()
        {
            if (port != null)
            {
                try
                {
                    port.DataReceived -= handler;
                    handler = null;
                    port.Close();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        private void SerialDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort senderPort = (SerialPort)sender;
            try
            {
                if (senderPort.BytesToRead > 2)
                {
                    string rawData = senderPort.ReadTo("\n");
                    if (rawData.Length > 1)
                    {
                        string[] data = rawData.Split(',');
                        switch (data[0])
                        {
                            case "OK":
                                float temp = float.Parse(data[1]);
                                float hum = float.Parse(data[2]);
                                panel.Invoke((MethodInvoker)delegate
                                {
                                    lblTempReading.Text = temp.ToString() + "°C";
                                    lblHumReading.Text = hum.ToString() + "%";
                                    AddData(tempGraph, temp);
                                    AddData(humGraph, hum);
                                });

                                int currentTickCount = Environment.TickCount;
                                if ((currentTickCount - logStart) > 60000.0)
                                {
                                    string fileName = String.Format(@"{0}\log.csv", Application.StartupPath);
                                    using (StreamWriter w = File.AppendText(fileName))
                                    {
                                        DateTime now = DateTime.Now;
                                        w.Write(String.Format("{0:g}", now) + "," + temp + "," + hum + "\r\n");
                                        w.Close();
                                    }
                                    logStart = currentTickCount;
                                }

                                break;
                            case "ERROR":
                                MessageBox.Show("DHT22 Error " + data[1]);
                                Disconnect();
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {

            }

        }

        private void DropDown(object sender, EventArgs e)
        {
            PopulatePortList();
        }

        private void CreateGraph(ZedGraphControl zgc, string title, string xTitle, string yTitle, string label)
        {
            GraphPane myPane = zgc.GraphPane;
            myPane.CurveList.Clear();
            myPane.Title.Text = title;
            myPane.XAxis.Title.Text = xTitle;
            myPane.YAxis.Title.Text = yTitle;

            // Save 150 points.  At 400 ms sample rate, this is one minute
            // The RollingPointPairList is an efficient storage class that always
            // keeps a rolling set of point data without needing to shift any data values
            RollingPointPairList list = new RollingPointPairList(150);

            // Initially, a curve is added with no data points (list is empty)
            // Color is blue, and there will be no symbols
            LineItem curve = myPane.AddCurve(label, list, Color.Blue, SymbolType.None);

            // Just manually control the X axis range so it scrolls continuously
            // instead of discrete step-sized jumps
            myPane.XAxis.Scale.Min = 0;
            myPane.XAxis.Scale.Max = 30;
            myPane.XAxis.Scale.MinorStep = 1;
            myPane.XAxis.Scale.MajorStep = 5;

            // Scale the axes
            zgc.AxisChange();

            // Save the beginning time for reference
            tickStart = Environment.TickCount;
        }

        private void AddData(ZedGraphControl graph, float yValue)
        {
            // Make sure that the curvelist has at least one curve
            if (graph.GraphPane.CurveList.Count <= 0)
                return;

            // Get the first CurveItem in the graph
            LineItem curve = graph.GraphPane.CurveList[0] as LineItem;
            if (curve == null)
                return;

            // Get the PointPairList
            IPointListEdit list = curve.Points as IPointListEdit;
            // If this is null, it means the reference at curve.Points does not
            // support IPointListEdit, so we won't be able to modify it
            if (list == null)
                return;

            // Time is measured in seconds
            double time = (Environment.TickCount - tickStart) / 1000.0;

            list.Add(time, yValue);

            // Keep the X scale at a rolling 30 second interval, with one
            // major step between the max X value and the end of the axis
            Scale xScale = graph.GraphPane.XAxis.Scale;
            if (time > xScale.Max - xScale.MajorStep)
            {
                xScale.Max = time + xScale.MajorStep;
                xScale.Min = xScale.Max - 30.0;
            }

            // Make sure the Y axis is rescaled to accommodate actual data
            graph.AxisChange();

            // Force a redraw
            graph.Invalidate();
        }

    }


}
