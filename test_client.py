import time
import zmq
import random
import json

class ZMQSocket(object):

    def __init__(self, zmq_type=zmq.REQ):
        
        print('Initializing socket of type: {}'.format(zmq_type))
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq_type)

    def connect(self, address='otter_service'):
    
        #connection = 'ipc://{}'.format(address)    
        connection = 'tcp://localhost:5555'    

        print('Connecting to: {}'.format(connection))
        self.socket.connect(connection)
        print('Connected!')

    def _send(self, message):
        self.socket.send_string(message)
        return self.socket.recv()

    def update(self, new_target=None, msg_type="target", power="on"):
        timestamp = int(time.time())
        payload = json.dumps(
            {
                "type": msg_type,
                "target": toSigned32(int(new_target)),
                "power": power,
                "timestamp": timestamp
            }
        )
        return self._send(payload)

def toSigned32(n):
    n = n & 0xffffffff
    return n | (-(n & 0x80000000))

def main():

    zmq = ZMQSocket()
    zmq.connect()

    for request in range(10):
        print("Sending request {} …".format(request))

        target = str(random.randint(-10,10))
        
       
        reply = zmq.update(target)

        print("Received reply {} [ {} ]".format(request, reply.decode("utf-8")))




if __name__ == "__main__":
    main()
