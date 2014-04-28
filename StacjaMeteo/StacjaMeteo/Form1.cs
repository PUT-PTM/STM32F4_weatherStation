using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
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
        

        public Form1()
        {
            InitializeComponent();
            this.ClientSize = new System.Drawing.Size(900, 450);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // utworzenie nowego wykresu
            chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            // pozycja wykresu
            chart1.Location = new System.Drawing.Point(10, 10);
            // rozmiar wykresu
            chart1.Size = new Size(880, 420);



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


    }
}
