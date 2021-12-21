using System;
using System.Threading;
using System.IO;
using Antmicro.Renode.Logging;
using Antmicro.Renode.Peripherals.Miscellaneous;
using System.Collections.Generic;

namespace Antmicro.Renode.Peripherals
{
    public class IRSensor : Button, IDisposable
    {
        private Stream _resource;
        private bool _disposed;
        private bool _disposedValue;
        private Queue<KeyValuePair<int, int>> queue;
        private Semaphore semaphore; 
        private bool active;
        Thread thread1;
        public IRSensor()
        {
            semaphore = new Semaphore(0, 10);
            active = true;
            queue = new Queue<KeyValuePair<int, int>>();
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

        // Triggers random press in times between 700 and 3000 ms
        private void TriggerThread(){
            while(true){
                semaphore.WaitOne();
                if(active){
                    KeyValuePair<int, int> element = queue.Dequeue();
                    this.Log(LogLevel.Debug, "Run " + element.Key + " triggers every " + element.Value + " ms");
                    for(int i = 0; i < element.Key; i++){
                        Thread.Sleep(element.Value);
                        PressAndRelease();
                    }
                }
            }
        }

        public void TriggerSensorNTimes(int nTimes, int interval = 450)
        {
            if(active){
                queue.Enqueue(new KeyValuePair<int, int>(nTimes, interval));
                this.Log(LogLevel.Debug, "Set " + nTimes + " triggers every " + interval + " ms");
                semaphore.Release();
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