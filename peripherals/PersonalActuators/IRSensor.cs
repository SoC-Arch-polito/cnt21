using System;
using System.Threading;
using System.IO;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Peripherals.Miscellaneous;

namespace Antmicro.Renode.Peripherals
{
    public class IRSensor : Button, IDisposable
    {
        private Stream _resource;
        private bool _disposed;
        private static readonly Random getrandom = new Random();
        private bool _disposedValue;
        private bool active;
        Thread thread1;
        public IRSensor()
        {
            active = true;
            thread1 = new Thread(TriggerThread);
            thread1.Start();
        }

        ~IRSensor() => Dispose(false);

        // Public implementation of Dispose pattern callable by consumers.
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        // Protected implementation of Dispose pattern.
        protected virtual void Dispose(bool disposing)
        {
            if (!_disposedValue)
            {
                if (disposing)
                {
                    thread1.Abort();
                }

                _disposedValue = true;
            }
        }

        public static int GetRandomNumber(int min, int max)
        {
            lock(getrandom) // synchronize
            {
                return getrandom.Next(min, max);
            }
        }

        // Triggers random press in times between 700 and 3000 ms
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