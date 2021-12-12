using System;
using System.Threading;
using Antmicro.Renode.Peripherals.Miscellaneous;

namespace Antmicro.Renode.Peripherals
{
    public class IRSensor : Button
    {
        private static readonly Random getrandom = new Random();

        private bool active;
        public IRSensor()
        {
            active = true;
            Thread thread1 = new Thread(TriggerThread);
            thread1.Start();
        }

        public static int GetRandomNumber(int min, int max)
        {
            lock(getrandom) // synchronize
            {
                return getrandom.Next(min, max);
            }
        }

        public void TriggerThread(){
            while(true){
                Thread.Sleep(GetRandomNumber(700, 3000));
                if(active)
                    PressAndRelease();
            }
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
            }
        }

    }
}