using Antmicro.Renode.Peripherals.I2C;
using Antmicro.Renode.Utilities;
using Antmicro.Renode.EmulationEnvironment;
using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.IO.Pipes;
using Antmicro.Migrant;
using Antmicro.Migrant.Hooks;
using Antmicro.Renode.Core;
using Antmicro.Renode.Exceptions;
using Antmicro.Renode.Logging;
using Antmicro.Renode.UserInterface;
using System.Windows.Forms;
using System.Threading;
using System.Drawing;

namespace Antmicro.Renode.Peripherals
{
    public class I2CDisplay : II2CPeripheral, IExternal
    {
        DisplayForm display;
        public partial class DisplayForm : Form
        {
           
            public static int numberOfCharPerRow = 16;
            enum FontDimension {M_5_11, M_5_8}
            enum EntryMode {ID_RIGHT, ID_LEFT};
            enum State
            {
                NO_INIT,
                INIT1,
                INIT2,
                INIT3,
                BIT4,
                BIT8
            }
            EntryMode i_d;
            FontDimension fontDim;
            State currState;
            I2CDisplay i2CDisplay;
            int numberOfLines;
            int interfaceBits;
            Boolean isDiplayOn;
            char[,] charMatrix;
            Label[,] labelMatrix;
            int rowPos, colPos;
            public DisplayForm(I2CDisplay i2CDisplay)
            {
                this.i2CDisplay = i2CDisplay;
                numberOfLines = 0;
                interfaceBits = 8; //Start at 8 bits interface
                currState = State.NO_INIT;
                fontDim = FontDimension.M_5_8;
                isDiplayOn = true;
                i_d = EntryMode.ID_LEFT;
                charMatrix = new char [2, numberOfCharPerRow];
                rowPos = 0;
                colPos = 0;
                
                // Create matrix
                labelMatrix = new Label[2, numberOfCharPerRow];
                for(int a = 0; a < 2; a++)
                    for (int i = 0; i < numberOfCharPerRow; i++){
                        Label labl = new Label();
                        labl.Location = new Point(i*50, a*80);
                        labl.Visible = true;
                        labl.AutoSize = false;
                        labl.BackColor = Color.Blue;
                        labl.Font = new Font("5x8 LCD HD44780U A02", 49);
                        labl.TextAlign = ContentAlignment.MiddleCenter;
                        labl.Size = new System.Drawing.Size(45, 65);
                        Controls.Add(labl);
                        labelMatrix[a,i] = labl;
                    }
            }

            public void setDisplaySize() {
                for(int a = 0; a < numberOfCharPerRow; a++){
                    labelMatrix[1,a].Visible = (numberOfLines==2);
                }
            }

            protected override void OnPaint(PaintEventArgs e)
            {
                base.OnPaint(e);
                setDisplaySize();
                setDisplayText();
            }

            public void setDisplayText(){
                 for(int a = 0; a < numberOfLines; a++)
                    for (int i = 0; i < numberOfCharPerRow; i++){
                        labelMatrix[a,i].Text = charMatrix[a,i].ToString();
                    }
            }

            public void updateView(){
                this.Invalidate();
            }

            void clearDisplay(){
                 for(int a = 0; a < 2; a++)
                    for (int i = 0; i < numberOfCharPerRow; i++){
                        charMatrix[a,i] = ' ';
                    }
            }

            void addChar(char c){

                if(i_d == EntryMode.ID_RIGHT){
                    if(colPos < numberOfCharPerRow){
                        charMatrix[rowPos,colPos++] = c;
                    }
                } else {
                    if(colPos < 0){
                        charMatrix[rowPos,colPos--] = c;
                    }
                }
            }

            // Refer to: http://web.alfredstate.edu/faculty/weimandn/lcd/lcd_initialization/lcd_initialization_index.html
            public void byteIn(byte[] data){
                byte command = (byte)((data[1]&0xf0) | ((data[3]&0xf0)>>4));
                i2CDisplay.Log(LogLevel.Debug, "C " + command.ToString("X2"));
                if (currState == State.NO_INIT && (command&0x30).Equals(0x30))
                    currState = State.INIT1;
                else if (currState == State.INIT1 && (command&0x30).Equals(0x30))
                    currState = State.INIT2;
                else if (currState == State.INIT2 && (command&0x30).Equals(0x30))
                    currState = State.INIT3;
                else if (currState == State.INIT3 && (command&0x20).Equals(0x20)){
                    currState = State.BIT4;
                    i2CDisplay.Log(LogLevel.Debug, "4 BIT interface started");
                }
                else if (currState == State.INIT3 && (command&0x30).Equals(0x30)){
                    currState = State.BIT8;
                    i2CDisplay.Log(LogLevel.Debug, "8 BIT interface started - not supported");
                }

                if(currState == State.BIT8) {
                    i2CDisplay.Log(LogLevel.Error, "8 BIT interface not supported");
                } else if(currState == State.BIT4) {
                    if((data[0]&0x3).Equals(0x0) && (data[1]&0x80).Equals(0x80)) {
                        // Set cursor position
                        colPos = Math.Min(numberOfCharPerRow, ((int)(data[2]&0xF0)>>4));
                        rowPos = ((data[1]&0x40)>>6).Equals(0x1) ? 1 : 0;
                        i2CDisplay.Log(LogLevel.Debug, "Cursor set to: " + rowPos + " " + colPos);
                    } else if((data[0]&0x3).Equals(0x1)) {
                        // Write data
                        addChar((char) command);
                    } else if((command&0xF1).Equals(0x1)){
                        clearDisplay();
                        i2CDisplay.Log(LogLevel.Debug, "Display cleared");
                    } else if((data[0]&0x3).Equals(0x0) && (command&0xfC).Equals(0x04)){
                        // Entry mode set
                        i_d = (command&0x2).Equals(0x2) ? EntryMode.ID_RIGHT : EntryMode.ID_LEFT; 
                        i2CDisplay.Log(LogLevel.Debug, "I/D: " + i_d.ToString());
                    } else if((data[0]&0x3).Equals(0x0) &&  (command&0xF8).Equals(0x08)) {
                        // Display ON OFF
                        isDiplayOn = (command&0x4).Equals(0x4) ? true : false;
                        i2CDisplay.Log(LogLevel.Debug, "Display is now: " + isDiplayOn.ToString());
                    } else if((data[0]&0x3).Equals(0x0) && (command&0x20).Equals(0x20)) {
                        // Function Set
                        numberOfLines = (command&0x8).Equals(0x8) ? 2 : 1;
                        fontDim = (command&0x4).Equals(0x4) ? FontDimension.M_5_11 : FontDimension.M_5_8;
                        i2CDisplay.Log(LogLevel.Debug, "Number of line: " + numberOfLines + " Font dimension: " + fontDim.ToString());
                    } else {
                        i2CDisplay.Log(LogLevel.Error, "Unknown command");
                    }
                }
                updateView();
            }
        }

        public void DoWork(){
            display = new DisplayForm(this);
            display.Size = new Size(DisplayForm.numberOfCharPerRow*50+5, 180);
            display.Text = "Display 2x" + DisplayForm.numberOfCharPerRow ;
            display.ShowDialog();
        }

        public I2CDisplay()
        {
            outputBuffer = new Queue<byte>();
            this.Log(LogLevel.Debug, "Display i2c connected");
            Thread thread1 = new Thread(DoWork);
            thread1.Start();
            Reset();
        }

        public void FinishTransmission() { }

        public byte[] Read(int count = 1)
        {
            byte temp;
            outputBuffer.Clear();
            if(active)
                temp=0x01;
            else
                temp=0x00;
            outputBuffer.Enqueue(temp);
            var result = outputBuffer.ToArray();
            this.Log(LogLevel.Debug, "Reading {0} bytes from the device (asked for {1} bytes).", result.Length, count);
            outputBuffer.Clear();
            return result;
        }

        public void Write(byte[] data)
        {
            //display.updateView();
            byte[] b=data.ToArray();
            byte off=0x00;
            //this.Log(LogLevel.Debug, "Received {0} bytes: [{1}]", data.Length, string.Join(", ", data.Select(x => x.ToString())));
            display.byteIn(data);
            if(data.Length>1){
            if(b[1].Equals(off)){
                outputBuffer.Clear();
                outputBuffer.Enqueue(b[1]);
                active=false;
            }else {
                outputBuffer.Clear();
                outputBuffer.Enqueue(b[1]);
                active=true;
            } 
            }
            return;
        }

        public void Reset()
        {
            Active = false;
            outputBuffer.Clear();
            outputBuffer.Enqueue(0x00);
        }

        public bool Active
        {
            get
            {
                return active;
            }
            set
            {
                active = value;
            }        }


        private bool active;


        private readonly Queue<byte> outputBuffer;
    }
}