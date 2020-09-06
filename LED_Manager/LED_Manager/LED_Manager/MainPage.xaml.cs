using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using System.Threading;

namespace LED_Manager
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class MainPage : ContentPage
    {
        [DllImport("libsharedlibraryled.so", EntryPoint = "led1_init")]
        internal static extern void InitLED1();

        [DllImport("libsharedlibraryled.so", EntryPoint = "led1_on")]
        internal static extern void TurnOnLED1();

        [DllImport("libsharedlibraryled.so", EntryPoint = "led1_off")]
        internal static extern void TurnOffLED1();

        [DllImport("libsharedlibraryled.so", EntryPoint = "led2_init")]
        internal static extern void InitLED2();

        [DllImport("libsharedlibraryled.so", EntryPoint = "led2_on")]
        internal static extern void TurnOnLED2();

        [DllImport("libsharedlibraryled.so", EntryPoint = "led2_off")]
        internal static extern void TurnOffLED2();

        [DllImport("libsharedlibraryled.so", EntryPoint = "led3_init")]
        internal static extern void InitLED3();

        [DllImport("libsharedlibraryled.so", EntryPoint = "led3_on")]
        internal static extern void TurnOnLED3();

        [DllImport("libsharedlibraryled.so", EntryPoint = "led3_off")]
        internal static extern void TurnOffLED3();

        [DllImport("libsharedlibraryled.so", EntryPoint = "fan1_init")]
        internal static extern void InitFAN1();

        [DllImport("libsharedlibraryled.so", EntryPoint = "fan1_on")]
        internal static extern void TurnOnFAN1();

        [DllImport("libsharedlibraryled.so", EntryPoint = "fan1_off")]
        internal static extern void TurnOffFAN1();

        [DllImport("libsharedlibraryled.so", EntryPoint = "fan2_init")]
        internal static extern void InitFAN2();

        [DllImport("libsharedlibraryled.so", EntryPoint = "fan2_on")]
        internal static extern void TurnOnFAN2();

        [DllImport("libsharedlibraryled.so", EntryPoint = "fan2_off")]
        internal static extern void TurnOffFAN2();


        public MainPage()
        {
            InitializeComponent();

            InitLED1();
            InitLED2();
            InitLED3();
            InitFAN1();
            InitFAN2();
        }

        private void Button1_TurnOn(object sender, EventArgs e)
        {
            TurnOnLED1();
        }

        private void Button1_TurnOff(object sender, EventArgs e)
        {
            TurnOffLED1();
        }

        private void Button2_TurnOn(object sender, EventArgs e)
        {
            TurnOnLED2();
        }

        private void Button2_TurnOff(object sender, EventArgs e)
        {
            TurnOffLED2();
        }

        private void Button3_TurnOn(object sender, EventArgs e)
        {
            TurnOnLED3();
        }

        private void Button3_TurnOff(object sender, EventArgs e)
        {
            TurnOffLED3();
            Thread.Sleep(300);
            TurnOnLED3();
        }

        private void Button4_TurnOn(object sender, EventArgs e)
        {
            TurnOnFAN1();
            TurnOffFAN2();
        }

        private void Button4_TurnOff(object sender, EventArgs e)
        {
            TurnOffFAN1();
            TurnOffFAN2();
        }

    }
}