using System;
using System.Runtime.InteropServices;
using Xamarin.Forms;

namespace LED_Manager
{
    class Program : global::Xamarin.Forms.Platform.Tizen.FormsApplication
    {
        [DllImport("libsharedlibraryled.so", EntryPoint = "led1_terminate")]
        internal static extern void TerminateLED1();

        [DllImport("libsharedlibraryled.so", EntryPoint = "led2_terminate")]
        internal static extern void TerminateLED2();

        [DllImport("libsharedlibraryled.so", EntryPoint = "led3_terminate")]
        internal static extern void TerminateLED3();

        [DllImport("libsharedlibraryled.so", EntryPoint = "fan1_terminate")]
        internal static extern void TerminateFAN1();

        [DllImport("libsharedlibraryled.so", EntryPoint = "fan2_terminate")]
        internal static extern void TerminateFAN2();

        protected override void OnCreate()
        {
            base.OnCreate();

            LoadApplication(new App());
        }

        protected override void OnTerminate()
        {
            TerminateLED1();
            TerminateLED2();
            TerminateLED3();
            TerminateFAN1();
            TerminateFAN2();

            base.OnTerminate();
        }

        static void Main(string[] args)
        {
            var app = new Program();
            Forms.Init(app);
            app.Run(args);
        }
    }
}
