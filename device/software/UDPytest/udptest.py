# -*- coding: cp932 -*-
from __future__ import print_function
import socket
import time
from contextlib import closing
import sys

def main():
    host = '192.168.200.232' # esp32��IP�A�h���X
    port = 10000 # �|�[�g�ԍ�
    port2=10001
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    #with closing(sock):
    sock.bind(('192.168.200.233',port2))#esp32�̂ƘA�ԂłȂ���dhcp���œ����Ȃ�
    while True:
        message = 'A'.encode('utf-8') # ���镶����(�����ł́uA�v�Ƃ���������𑗂��Ă���)
        #print(message)
        sock.sendto(message, (host, port))
        #time.sleep(1)
        try:
          data,adr=sock.recvfrom(1024)
        except socket.error:
          pass
        else:
          print("rcvd:",str(data[0]),str(data[1]),str(data[2]),str(data[3]))
          #print("rcvd",data)
    return

if __name__ == '__main__':
  main()
