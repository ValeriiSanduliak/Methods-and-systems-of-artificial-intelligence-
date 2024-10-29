using System.Windows.Forms;

namespace Lab4
{
    public partial class Form1 : Form
    {
        private List<Point> points = new List<Point>();
        private List<int> vectorClasses = new List<int>();
        private List<int> savedVectorClasses = null;
        private List<int> testSavedVectorClasses = null;
        private List<int> normalizedVectorClasses = null;
        private List<int> normalizedTestVectorClasses = null;
        private Dictionary<int, List<List<int>>> normalizedVectorsMap = new Dictionary<int, List<List<int>>>();


        private Bitmap pic1;
        private Bitmap pic2;
        private bool drawing = false;
        private int separationDistance = 20;

        public Form1()
        {
            InitializeComponent();
            this.Text = "Lab 4";
            this.DoubleBuffered = true;

            // Initialize images for drawing
            pic1 = new Bitmap(pictureBox1.Width, pictureBox1.Height);
            pic2 = new Bitmap(pictureBox2.Width, pictureBox2.Height);

            // Assign Bitmap to PictureBox to ensure display
            pictureBox1.Image = pic1;
            pictureBox2.Image = pic2;

            // Assign event handlers for PictureBox
            pictureBox1.MouseDown += PictureBox_MouseDown;
            pictureBox1.MouseMove += PictureBox_MouseMove;
            pictureBox1.MouseUp += PictureBox_MouseUp;

            pictureBox2.MouseDown += PictureBox_MouseDown;
            pictureBox2.MouseMove += PictureBox_MouseMove;
            pictureBox2.MouseUp += PictureBox_MouseUp;

            textBox1.TextChanged += textBox1_TextChanged;
        }

        private List<int> normalizeVector(List<int> vector)
        {
            if (vector.Count <= 1)
            {
                return vector;
            }

            List<int> normalized = new List<int>();
            normalized.Add(vector[0]);

            for (int i = 1; i < vector.Count; i++)
            {
                int previous = vector[i - 1];
                if (vector[i] != previous)
                {
                    normalized.Add(vector[i]);
                }
            }

            return normalized;
        }

        private void PictureBox_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                drawing = true;
                points.Clear();
                vectorClasses.Clear();
                if (sender == pictureBox1)
                {
                    using (Graphics g = Graphics.FromImage(pic1))
                    {
                        g.Clear(Color.White);
                    }
                    pictureBox1.Invalidate();
                }
                else if (sender == pictureBox2)
                {
                    using (Graphics g = Graphics.FromImage(pic2))
                    {
                        g.Clear(Color.White);
                    }
                    pictureBox2.Invalidate();
                }
            }
        }

        private void PictureBox_MouseMove(object sender, MouseEventArgs e)
        {
            // Capture all points as the user draws freely
            if (drawing)
            {
                points.Add(e.Location);

                if (sender == pictureBox1)
                {
                    using (Graphics g = Graphics.FromImage(pic1))
                    {
                        if (points.Count > 1)
                        {
                            Point previousPoint = points[points.Count - 2];
                            Point currentPoint = points[points.Count - 1];
                            g.DrawLine(Pens.Black, previousPoint, currentPoint);
                        }
                    }
                    pictureBox1.Invalidate();
                }
                else if (sender == pictureBox2)
                {
                    using (Graphics g = Graphics.FromImage(pic2))
                    {
                        if (points.Count > 1)
                        {
                            Point previousPoint = points[points.Count - 2];
                            Point currentPoint = points[points.Count - 1];
                            g.DrawLine(Pens.Black, previousPoint, currentPoint);
                        }
                    }
                    pictureBox2.Invalidate();
                }
            }
        }

        private void PictureBox_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                drawing = false;

                // Get segment length from textbox1
                if (!int.TryParse(textBox1.Text, out int segmentLength) || segmentLength <= 0)
                {
                    MessageBox.Show("Please enter a valid segment length in textbox1.");
                    return;
                }

                List<Point> segmentPoints = new List<Point>();
                vectorClasses.Clear();

                for (int i = 0; i < points.Count - 1; i++)
                {
                    Point start = points[i];
                    segmentPoints.Add(start);

                    // Find the next point that is at least 'segmentLength' distance away
                    for (int j = i + 1; j < points.Count; j++)
                    {
                        if (Distance(start, points[j]) >= segmentLength)
                        {
                            Point end = points[j];
                            segmentPoints.Add(end);

                            // Calculate the direction vector class based on the angle
                            double angle = GetAngle(start, end);
                            int vectorClass = ClassifyVector(angle);
                            vectorClasses.Add(vectorClass);

                            // Draw the segment in red and label it with the vector class
                            if (sender == pictureBox1)
                            {
                                using (Graphics g = Graphics.FromImage(pic1))
                                {
                                    DrawVector(g, start, end, vectorClass, Pens.Red);
                                }
                                pictureBox1.Invalidate();
                            }
                            else if (sender == pictureBox2)
                            {
                                using (Graphics g = Graphics.FromImage(pic2))
                                {
                                    DrawVector(g, start, end, vectorClass, Pens.Red);
                                }
                                pictureBox2.Invalidate();
                            }

                            i = j - 1; // Move to the next segment start point
                            break;
                        }
                    }
                }

                // Normalize vector classes
                List<int> normalizedVector = normalizeVector(vectorClasses);

                // Save normalized vector depending on the picture box
                if (sender == pictureBox1)
                {
                    normalizedVectorClasses = normalizedVector;
                }
                else if (sender == pictureBox2)
                {
                    normalizedTestVectorClasses = normalizedVector;
                    outToLog("Test vector saved.");
                }
            }
        }

        // Updated DrawVector method to accept a custom pen color
        private void DrawVector(Graphics g, Point p1, Point p2, int vectorClass, Pen pen)
        {
            g.DrawLine(pen, p1, p2);
            g.DrawString(vectorClass.ToString(), Font, Brushes.Red, (p1.X + p2.X) / 2, (p1.Y + p2.Y) / 2);
        }

        private int ClassifyVector(double angle)
        {
            angle = angle % 360; // Нормалізуємо кут в діапазон 0-359

            if (angle < 22.5 || angle >= 337.5) return 1; // 0-22.5 та 337.5-360
            if (angle < 67.5) return 2; // 22.5-67.5
            if (angle < 112.5) return 3; // 67.5-112.5
            if (angle < 157.5) return 4; // 112.5-157.5
            if (angle < 202.5) return 5; // 157.5-202.5
            if (angle < 247.5) return 6; // 202.5-247.5
            if (angle < 292.5) return 7; // 247.5-292.5
            return 8; // 292.5-337.5
        }


        private double GetAngle(Point p1, Point p2)
        {
            double deltaY = p2.Y - p1.Y;
            double deltaX = p2.X - p1.X;
            double angle = Math.Atan2(deltaY, deltaX) * 180 / Math.PI;
            angle += 90;
            if (angle < 0) angle += 360;

            return angle;
        }

        private double Distance(Point p1, Point p2)
        {
            return Math.Sqrt(Math.Pow(p2.X - p1.X, 2) + Math.Pow(p2.Y - p1.Y, 2));
        }

       

        private double ComparePatterns(List<int> pattern1, List<int> pattern2)
        {
            if (pattern1 == null || pattern2 == null)
                return 0;

            int minLength = Math.Min(pattern1.Count, pattern2.Count);
            int matches = 0;

            for (int i = 0; i < minLength; i++)
            {
                if (pattern1[i] == pattern2[i])
                    matches++;
            }

            return (double)matches / minLength;
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            if (int.TryParse(textBox1.Text, out int newDistance) && newDistance > 0)
            {
                separationDistance = newDistance;
            }
            else
            {
                MessageBox.Show("Please enter a valid number greater than zero.");
                textBox1.Text = separationDistance.ToString();
            }
        }

        private void Form1_Load(object sender, EventArgs e) { }

        public void outToLog(string output)
        {
            Console.WriteLine(output);
        }


        // Save normalized vector to the map using the button's specific key
        private void SaveVectorToMap(int key)
        {
            if (normalizedVectorClasses != null && normalizedVectorClasses.Count > 0)
            {
                // Перевіряємо, чи вже існує список для ключа, інакше створюємо новий
                if (!normalizedVectorsMap.ContainsKey(key))
                {
                    normalizedVectorsMap[key] = new List<List<int>>();
                }

                // Додаємо поточний нормалізований вектор до списку
                normalizedVectorsMap[key].Add(new List<int>(normalizedVectorClasses));
                MessageBox.Show($"Normalized vector saved to key {key}.");
                outToLog($"Saved normalized vector for key {key} - ({string.Join(", ", normalizedVectorClasses)})");
            }
            else
            {
                MessageBox.Show("No vector available to save.");
            }
        }


        private void button1_Click(object sender, EventArgs e)
        {
            SaveVectorToMap(1);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            SaveVectorToMap(2);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            SaveVectorToMap(3);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            SaveVectorToMap(4);
        }

        private void button5_Click(object sender, EventArgs e)
        {
            SaveVectorToMap(5);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            SaveVectorToMap(6);
        }

        private void button7_Click(object sender, EventArgs e)
        {
            SaveVectorToMap(7);
        }

        private void button8_Click(object sender, EventArgs e)
        {
            SaveVectorToMap(8);
        }

        private void button9_Click(object sender, EventArgs e)
        {
            SaveVectorToMap(9);
        }

        private void button11_Click(object sender, EventArgs e)
        {
            foreach (KeyValuePair<int, List<List<int>>> entry in normalizedVectorsMap)
            {
                outToLog($"Key: {entry.Key}");
                int vectorIndex = 1;
                foreach (var vector in entry.Value)
                {
                    outToLog($"Vector {vectorIndex}: ({string.Join(", ", vector)})");
                    vectorIndex++;
                }
            }
        }

        private void button10_Click(object sender, EventArgs e)
        {
            if (normalizedTestVectorClasses == null)
            {
                MessageBox.Show("Please load or draw a test vector first.");
                return;
            }

            Dictionary<int, double> similarityScores = CompareWithAllKeys(normalizedTestVectorClasses);

            // Display the likelihood of belonging to each class
            string resultMessage = "Similarity Scores:\n";
            foreach (var score in similarityScores)
            {
                resultMessage += $"Class {score.Key}: {score.Value * 100:0.00}%\n";
            }

            MessageBox.Show(resultMessage);
        }


        private Dictionary<int, double> CompareWithAllKeys(List<int> testVector)
        {
            Dictionary<int, double> similarityScores = new Dictionary<int, double>();

            foreach (var entry in normalizedVectorsMap)
            {
                int key = entry.Key;
                List<List<int>> vectors = entry.Value;
                double highestScore = 0;

                // Compare test vector with each saved vector for this key
                foreach (var savedVector in vectors)
                {
                    double matchScore = ComparePatterns(testVector, savedVector);
                    if (matchScore > highestScore)
                    {
                        highestScore = matchScore;
                    }
                }

                similarityScores[key] = highestScore;
            }

            return similarityScores;
        }
    }
}
