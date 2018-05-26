using UnityEngine;
using System.Net;
using System.Net.Sockets;
using UniRx;

namespace UDPQuatTest{

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

	public class UDPQuatTest : MonoBehaviour
	{
		private const int listenPort = 10001;
		private static UdpClient myClient;
		private bool isAppQuitting;
		public IObservable<UdpState> _udpSequence;
		private long[] lquat = new long[]{0,0,0,0};
		private float[] quat = new float[]{ 0, 0, 0, 0 };

		long[] byteToLong(byte[] input, int sizeofout){
			long[] output = new long[]{ 0, 0, 0, 0 };
			for(int i=0; i<sizeofout; i++)
				output[i] = (((long)input[4*i+0])<<24) | (((long)input[4*i+1])<<16) | (((long)input[4*i+2])<<8) | (((long)input[4*i+3]));
			return output;
		}

		float LPF(float newdata, float postdata){
			float i = 0.1f;
			return newdata * i + postdata * (1 - i);
		}



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
					lquat = byteToLong (x.UdpMsg,4);

					
					for(int i=0; i<4; i++)
						quat[i]= LPF(lquat[i]/1073741824f,quat[i]);
					Debug.Log (string.Format ("{0:X},{1:X},{2:X},{3:X}", lquat[0],lquat[1],lquat[2],lquat[3]));
					//transform.rotation = Quaternion.AngleAxis (90,new Vector3(x.UdpMsg[0],x.UdpMsg[1],x.UdpMsg[2]));
					transform.rotation = new Quaternion (quat[0],quat[1],quat[2],quat[3]);
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