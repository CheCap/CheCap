//UdpReceiverRx.cs
//UniRxを用いて高速なUdp受信を行う

namespace UdpReceiverUniRx {

	using UnityEngine;
	using System.Net;
	using System.Net.Sockets;
	using UniRx;

	public class UdpState : System.IEquatable<UdpState>
	{
		//UDP通信の情報を収める。送受信ともに使える
		public IPEndPoint EndPoint {get; set;}
		public string UdpMsg {get; set;}

		public UdpState(IPEndPoint ep, string udpMsg)
		{
			this.EndPoint = ep;
			this.UdpMsg = udpMsg;
		}
		public override int GetHashCode() {
			return EndPoint.Address.GetHashCode();
		}

		public bool Equals(UdpState s)
		{
			if ( s == null ) {
				return false;
			}
			return EndPoint.Address.Equals(s.EndPoint.Address);
		}
	}

	public class UdpReceiverRx : MonoBehaviour
	{
		private const int listenPort = 10001;
		private static UdpClient myClient;
		private bool isAppQuitting;
		public IObservable<UdpState> _udpSequence;
		TextMesh Box;

		void Awake()
		{
			Debug.Log ("Awake");
			_udpSequence = Observable.Create<UdpState> (observer => {
				Debug.Log ("Obs");
				Debug.Log (string.Format ("_udpSequence thread: {0}", System.Threading.Thread.CurrentThread.ManagedThreadId));
				try {
					myClient = new UdpClient (listenPort);
				} catch (SocketException ex) {
					observer.OnError (ex);
				}
				IPEndPoint remoteEP = null;
				myClient.EnableBroadcast = true;
				myClient.Client.ReceiveTimeout = 50000;
				while (!isAppQuitting) {
					try {
						remoteEP = null;
						var receivedMsg = System.Text.Encoding.ASCII.GetString (myClient.Receive (ref remoteEP));
						observer.OnNext (new UdpState (remoteEP, receivedMsg));
					} catch (SocketException) {
						Debug.Log ("UDP::Receive timeout");
					}
				}
				observer.OnCompleted ();
				return null;
				//return Disposable.Empty;
			})
				.SubscribeOn(Scheduler.ThreadPool)
				.Publish()
				.RefCount();
		}

		void Start () {
			Box = GetComponent<TextMesh> ();

			_udpSequence
				.ObserveOnMainThread()
				.Subscribe(x =>{
					print(x.UdpMsg);
					Box.text=x.UdpMsg;
				})
				.AddTo(this);
		}

		void OnApplicationQuit()
		{
			isAppQuitting = true;
			myClient.Client.Blocking = false;
		}
	}
}