///-----------------------------------------------------------------------------------
///-----------------------------------------------------------------------------------
/// O2U Server - Unity Indie VRPN Adapter 
/// 
/// File: UIVA.cs
/// 
/// Description:
///     Each device has a UIVA_Device class, which contains data buffers and callbacks 
///     functions.
/// 
/// OpenViBE2Unity (UIVA Modified)
/// Author: 
/// Modified by Seong-Jooon Jeong, Soo-Yong Kim, Min-Kyu Ahn.
/// Handong BCI LAB (https://sites.google.com/handong.edu/ahnbcilab)
///
///-----------------------------------------------------------------------------------
///-----------------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Vrpn;
using System.Threading;

namespace UIVA_Server
{
    class UIVA_OpenvibeToUnityAnalog
    {
        // Remote
        AnalogRemote anaRemote;

        // Data buffers
        public DateTime anaTimeStamp;   // Time event for analog events     
        public int numOfChannels;
        public List<double> signal = new List<double>(); // signal data  
        public bool firstTrial = true;


        public UIVA_OpenvibeToUnityAnalog(String peripheralName)  // peripheralName => name => token[1] => openvibe_vrpn_analog@localhost (cfg file)
        {
            anaRemote = new AnalogRemote(peripheralName);
            anaRemote.AnalogChanged += new AnalogChangeEventHandler(OpenvibeAnalogChanged);
            anaRemote.MuteWarnings = true;
        }

        /* Analog event handler */
        private void OpenvibeAnalogChanged(object sender, AnalogChangeEventArgs e)
        {
            anaTimeStamp = e.Time.ToLocalTime();
            numOfChannels = e.Channels.Length;

            if (firstTrial)
            {
                for (int i = 0; i < numOfChannels; i++)
                {
                    // Without any rounding about the signal data, you can only receive maximum 11 channels safely. 
                    // Because the buffer string is corrupted by exceeding the size of buffer.
                    // It causes an 'ArgumentOutOfRange' error in UIVA_Client.
                    signal.Add(Math.Round(e.Channels[i], 3));
                }
                firstTrial = false;
            }
            else
            {
                for (int i = 0; i < numOfChannels; i++)
                {
                    signal[i] = Math.Round(e.Channels[i], 3);
                }
            }
        }

        /* Remote mainloop() */
        public void Update()
        {
            anaRemote.Update();
        }

        /* Encode OpenViBE analog signal to UIVA_Client */
        public String Encode()
        {
            String anaStr = String.Format("OPENVIBE_ANALOG,,{0},{1}", anaTimeStamp.ToString("o"), numOfChannels);
            for (int i = 0; i < numOfChannels; i++)
            {
                anaStr += "," + signal[i].ToString();
            }
            anaStr += "\n";
            return anaStr;
        }
    }

    class UIVA_OpenvibeToUnityButton
    {
        // Remote
        ButtonRemote buttRemote;

        // Data buffers
        public DateTime buttTimeStamp;  // Time event for button events
        // If you want more buttons, you can add them like this.
        public bool buttA;       // KEYBOARD STIMULATION 'A' => OVTK_StimulationId_Label_01 , OVTK_StimulationId_Label_00
        public bool buttZ;       // KEYBOARD STIMULATION 'Z' => OVTK_StimulationId_Label_02 , OVTK_StimulationId_Label_00        


        public UIVA_OpenvibeToUnityButton(String peripheralName)  // peripheralName => name => token[1] => openvibe_vrpn_button@localhost (cfg file)
        {
            buttRemote = new ButtonRemote(peripheralName);
            buttRemote.ButtonChanged += new ButtonChangeEventHandler(OpenvibeButtonChanged);
            buttRemote.MuteWarnings = true;
        }

        /* Button event handler */
        private void OpenvibeButtonChanged(object sender, ButtonChangeEventArgs e)
        {
            buttTimeStamp = e.Time.ToLocalTime();
            switch (e.Button)
            {
                case 0: // 0 means that the first input of 'VRPN Button Server' in OpenViBE
                    if (e.IsPressed) { buttA = true; } else { buttA = false; }
                    break;
                case 1: // 1 means that the second input of 'VRPN Button Server' in OpenViBE
                    if (e.IsPressed) { buttZ = true; } else { buttZ = false; }
                    break;
                default:
                    break;
            }
        }

        /*Remote mainloop() */
        public void Update()
        {
            buttRemote.Update();
        }

        /* Encode OpenViBE button signal to UIVA_Client */
        public String Encode()
        {
            String buttStr = String.Format("OPENVIBE_BUTTON,,{0}", buttTimeStamp.ToString("o"));
            // Uppercase for button press, lowercase for button release
            if (buttA) { buttStr += ",A"; } else { buttStr += ",a"; }
            if (buttZ) { buttStr += ",Z"; } else { buttStr += ",z"; }
            buttStr += "\n";
            return buttStr;
        }
    }

    class UIVA_UnityToOpenvibeButton
    {
        // Server
        ButtonServer unityButtServer;
        Connection vrpnConnection;
        int numOfButt = 6;  // You can use the number of buttons as you need by chainging this value.

        public UIVA_UnityToOpenvibeButton()
        {
            vrpnConnection = Connection.CreateServerConnection(50555);// port number
            unityButtServer = new ButtonServer("unity_button@localhost", vrpnConnection, 32);
            unityButtServer.MuteWarnings = true;
        }

        /* Check the state of VRPN connection */
        public void WakeUpTest()
        {
            for (int i = 1; i < 6; i++)
            {
                Thread.Sleep(1000);

                unityButtServer.Buttons[numOfButt-1] = true;
                unityButtServer.Update();
                vrpnConnection.Update();

                unityButtServer.Buttons[numOfButt-1] = false;
                unityButtServer.Update();
                vrpnConnection.Update();

                Console.WriteLine("UIVA_UnityToOpenvibeButton : VRPN Connection Test..." + i);
            }
            Console.WriteLine("UIVA_UnityToOpenvibeButton : VRPN Connection Completed!");
        }

        /* Mark a stimulation of Unity on OpenViBE */
        public void Mark(int buttNum)
        {
            unityButtServer.Buttons[buttNum] = true;
            unityButtServer.Update();
            vrpnConnection.Update();
            unityButtServer.Buttons[buttNum] = false;
            unityButtServer.Update();
            vrpnConnection.Update();
        }
    }
}
