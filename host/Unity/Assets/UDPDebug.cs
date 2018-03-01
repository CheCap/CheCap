using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UniRx;
using UdpReceiverUniRx;

public class UDPDebug : MonoBehaviour {
	public UdpReceiverRx _udpReceiverRx;
	private IObservable<UdpState> myUdpSequence;

	TextMesh Box;

	// Use this for initialization
	void Start () {
		//myUdpSequence = _udpReceiverRx._udpSequence;
		Box = GetComponent<TextMesh> ();

		/*myUdpSequence*/_udpReceiverRx._udpSequence
			.ObserveOnMainThread()
			.Subscribe(x =>{
				print(x.UdpMsg);
				Box.text=x.UdpMsg;
			})
			.AddTo(this);
	}
	
	// Update is called once per frame
	void Update () {
	}
}
