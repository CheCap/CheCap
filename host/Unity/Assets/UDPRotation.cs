//UdpReceiverRx.cs
//UniRxを用いて高速なUdp受信を行う


using UnityEngine;
using System.Net;
using System.Net.Sockets;
using UniRx;

namespace UDPRotation{

public class UdpState : System.IEquatable<UdpState>
{
	//UDP通信の情報を収める。送受信ともに使える
	public IPEndPoint EndPoint {get; set;}
	public byte[] UdpMsg {get; set;}

	public UdpState(IPEndPoint ep, byte[] udpMsg)
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

public class UDPRotation : MonoBehaviour
{
	private const int listenPort = 10001;
	private static UdpClient myClient;
	private bool isAppQuitting;
	public IObservable<UdpState> _udpSequence;

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
					byte[] receivedMsg = myClient.Receive (ref remoteEP);
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

		_udpSequence
			.ObserveOnMainThread()
			.Subscribe(x =>{
				Debug.Log (string.Format ("{0},{1},{2}", x.UdpMsg[0],x.UdpMsg[1],x.UdpMsg[2]));
				transform.rotation = Quaternion.AngleAxis (90,new Vector3(x.UdpMsg[0],x.UdpMsg[1],x.UdpMsg[2]));
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