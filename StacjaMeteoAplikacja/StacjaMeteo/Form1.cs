using StacjaMeteo.Properties;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace StacjaMeteo
{
    public partial class Form1 : Form
    {
        private Chart chart1;
        private ChartArea MeteoGraph;
        private Series ChartSeries;
        private Legend ChartLegend;

        // The main control for communicating through the RS-232 port
        private SerialPort comport = new SerialPort();

        private Settings settings = Settings.Default;

        public Form1()
        {
            // Load user settings
            settings.Reload();
            
            InitializeComponent();

            // Restore the users settings
            InitializeControlValues();



            this.ClientSize = new System.Drawing.Size(900, 410);

            // When data is recieved through the port, call this method
            comport.DataReceived += new SerialDataReceivedEventHandler(port_DataReceived);
            //comport.PinChanged += new SerialPinChangedEventHandler(comport_PinChanged);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // utworzenie nowego wykresu
            chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            // pozycja wykresu
            chart1.Location = new System.Drawing.Point(10, 10);
            // rozmiar wykresu
            chart1.Size = new Size(880, 320);



            // dodanie serii danych na temperature
            chart1.Series.Add("Temperature");
            ChartSeries = chart1.Series["Temperature"];
            // wykres liniowy
            ChartSeries.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;




            // typ osi X na "czas"
            ChartSeries.XValueType = ChartValueType.Time;
            // grubosc linii na wykresie
            ChartSeries.BorderWidth = 3;
            // styl osi na wykresie
            ChartSeries.BorderDashStyle = ChartDashStyle.Dash;



            // nazewnictwo serii danych
            chart1.ChartAreas.Add("MeteoGraph");
            chart1.Palette = ChartColorPalette.EarthTones;

            MeteoGraph = chart1.ChartAreas["MeteoGraph"];
            MeteoGraph.AxisX.Title = "Godzina";
            MeteoGraph.AxisX.LabelStyle.Format = "HH:mm";
            MeteoGraph.AxisX.Interval = 1;
            MeteoGraph.AxisX.IntervalType = DateTimeIntervalType.Hours;
            MeteoGraph.AxisX.LineWidth = 3;

            // MeteoGraph.AxisX.Minimum = DateTime.Now.AddHours(-24).ToOADate();
            // MeteoGraph.AxisX.Maximum = DateTime.Now.ToOADate();

            MeteoGraph.AxisY.Minimum = -25;
            MeteoGraph.AxisY.Maximum = 50;
            MeteoGraph.AxisY.Title = "Temperatura";
            MeteoGraph.AxisY.Interval = 5;
            MeteoGraph.AxisY.LineWidth = 3;



            Random random = new Random();
            for (int x = 24; x > 0; x--)
            {
                ChartSeries.Points.AddXY(DateTime.Now.AddHours(-x).ToOADate(), random.Next(-26, 51));
            }



            // legenda wykresu
            ChartSeries.LegendText = "temp";
            chart1.Legends.Add("Legend");
            ChartLegend = chart1.Legends["Legend"];
            ChartLegend.BorderColor = Color.DarkRed;



            // dodanie wykresu do forma
            Controls.Add(chart1);
        }


        /// <summary> Populate the form's controls with default settings. </summary>
        private void InitializeControlValues()
        {
            cmbParity.Items.Clear(); cmbParity.Items.AddRange(Enum.GetNames(typeof(Parity)));
            cmbStopBits.Items.Clear(); cmbStopBits.Items.AddRange(Enum.GetNames(typeof(StopBits)));

            cmbParity.Text = settings.Parity.ToString();
            cmbStopBits.Text = settings.StopBits.ToString();
            cmbDataBits.Text = settings.DataBits.ToString();
            cmbParity.Text = settings.Parity.ToString();
            cmbBaudRate.Text = settings.BaudRate.ToString();

            RefreshComPortList();

            //chkClearOnOpen.Checked = settings.ClearOnOpen;
            //chkClearWithDTR.Checked = settings.ClearWithDTR;

            // If it is still avalible, select the last com port used
            if (cmbPortName.Items.Contains(settings.PortName)) cmbPortName.Text = settings.PortName;
            else if (cmbPortName.Items.Count > 0) cmbPortName.SelectedIndex = cmbPortName.Items.Count - 1;
            else
            {
                MessageBox.Show(this, "There are no COM Ports detected on this computer.\nPlease install a COM Port and restart this app.", "No COM Ports Installed", MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.Close();
            }
        }

        private void port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            // If the com port has been closed, do nothing
            if (!comport.IsOpen) return;

            // This method will be called when there is data waiting in the port's buffer


                // Read all the data waiting in the buffer
                string data = comport.ReadExisting();

                // Display the text to the user in the terminal
                System.Console.WriteLine("[port_DataReceived]" + data);
 
        }

        private void RefreshComPortList()
        {
            // Determain if the list of com port names has changed since last checked
            string selected = RefreshComPortList(cmbPortName.Items.Cast<string>(), cmbPortName.SelectedItem as string, comport.IsOpen);

            // If there was an update, then update the control showing the user the list of port names
            if (!String.IsNullOrEmpty(selected))
            {
                cmbPortName.Items.Clear();
                cmbPortName.Items.AddRange(OrderedPortNames());
                cmbPortName.SelectedItem = selected;
            }
        }
        private string[] OrderedPortNames()
        {
            // Just a placeholder for a successful parsing of a string to an integer
            int num;

            // Order the serial port names in numberic order (if possible)
            return SerialPort.GetPortNames().OrderBy(a => a.Length > 3 && int.TryParse(a.Substring(3), out num) ? num : 0).ToArray();
        }

        private string RefreshComPortList(IEnumerable<string> PreviousPortNames, string CurrentSelection, bool PortOpen)
        {
            // Create a new return report to populate
            string selected = null;

            // Retrieve the list of ports currently mounted by the operating system (sorted by name)
            string[] ports = SerialPort.GetPortNames();

            // First determain if there was a change (any additions or removals)
            bool updated = PreviousPortNames.Except(ports).Count() > 0 || ports.Except(PreviousPortNames).Count() > 0;

            // If there was a change, then select an appropriate default port
            if (updated)
            {
                // Use the correctly ordered set of port names
                ports = OrderedPortNames();

                // Find newest port if one or more were added
                string newest = SerialPort.GetPortNames().Except(PreviousPortNames).OrderBy(a => a).LastOrDefault();

                // If the port was already open... (see logic notes and reasoning in Notes.txt)
                if (PortOpen)
                {
                    if (ports.Contains(CurrentSelection)) selected = CurrentSelection;
                    else if (!String.IsNullOrEmpty(newest)) selected = newest;
                    else selected = ports.LastOrDefault();
                }
                else
                {
                    if (!String.IsNullOrEmpty(newest)) selected = newest;
                    else if (ports.Contains(CurrentSelection)) selected = CurrentSelection;
                    else selected = ports.LastOrDefault();
                }
            }

            // If there was a change to the port list, return the recommended default selection
            return selected;
        }

        private void buttonConnect_Click(object sender, EventArgs e)
        {
            bool error = false;

          // If the port is open, close it.
            if (comport.IsOpen) comport.Close();
            else
            {
                // Set the port's settings
                comport.BaudRate = int.Parse(cmbBaudRate.Text);
                comport.DataBits = int.Parse(cmbDataBits.Text);
                comport.StopBits = (StopBits)Enum.Parse(typeof(StopBits), cmbStopBits.Text);
                comport.Parity = (Parity)Enum.Parse(typeof(Parity), cmbParity.Text);
                comport.PortName = cmbPortName.Text;

                try
                {
                    // Open the port
                    comport.Open();
                }
                catch (UnauthorizedAccessException) { error = true; }
                catch (IOException) { error = true; }
                catch (ArgumentException) { error = true; }

                if (error) MessageBox.Show(this, "Could not open the COM port.  Most likely it is already in use, has been removed, or is unavailable.", "COM Port Unavalible", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                else
                {
                    // Show the initial pin states
                    //UpdatePinState();
                    //chkDTR.Checked = comport.DtrEnable;
                    //chkRTS.Checked = comport.RtsEnable;
                    buttonDownload.Enabled = true;
                    System.Console.WriteLine("Polaczono z " + cmbPortName.Text);
                }

            }
        }

        private void buttonDownload_Click(object sender, EventArgs e)
        {
            // Send the user's text straight out the port
            System.Console.WriteLine("[buttonDownload_Click] test");
            comport.Write("test");
        }

    }
}
