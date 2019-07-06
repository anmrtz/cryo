import time
import zmq
import random
import json

class ZMQSocket(object):

    def __init__(self, zmq_type=zmq.REQ):
        
        print(f'Initializing socket of type: {zmq_type}')
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq_type)

    def connect(self, address='otter_service'):
    
        connection = f'ipc://{address}'    

        print(f'Connecting to: {connection}')
        self.socket.connect(connection)
        print(f'Connected!')

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
        print("Sending request %s …" % request)

        target = str(random.randint(-10,10))
        
       
        reply = zmq.update(target)

        print("Received reply %s [ %s ]" % (request, reply.decode("utf-8") ))




if __name__ == "__main__":
    main()
