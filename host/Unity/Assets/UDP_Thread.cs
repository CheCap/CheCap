using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

public class UDP_Thread : MonoBehaviour {

	int LOCA_LPORT = 10001;
	static UdpClient udp;
	Thread thread;

	void Start ()
	{
		udp = new UdpClient(LOCA_LPORT);
		udp.Client.ReceiveTimeout = 10000;
		thread = new Thread(new ThreadStart(ThreadMethod));
		thread.Start(); 
	}

	void Update ()
	{
	}

	void OnApplicationQuit()
	{
		thread.Abort();
	}

	private static void ThreadMethod()
	{
		while(true)
		{
			IPEndPoint remoteEP = null;
			byte[] data = udp.Receive(ref remoteEP);
			string text = Encoding.ASCII.GetString(data);
			Debug.Log(text);
		}
	} 
}