from bluetooth import *
import paho.mqtt.client as mqtt
from datetime import datetime
import schedule, time 

# IP e porta relativi al broker MQTT
broker_ip = "192.168.1.25" 
broker_port = 1883

publish_topic = "Progetto SOD"
client = mqtt.Client() # Creazione del client MQTT

# Connessione al broker 
client.connect(broker_ip, broker_port, 60)
client.loop_start()

# Invio della data e dell'orario all'ESP32
def send_time():
    current_time = int(time.time())
    print(current_time)
    sock.send(str(current_time))
            
def receive_data():
    data = sock.recv(2048)
    data = data.decode()
    if data=="Ho bisogno dell'orario":
    	send_time()
    else:            
        #Pubblicazione del topic da parte del client MQTT
    	client.publish(publish_topic, str(data), 1) 
    print(data)
        
#MAC address dell'ESP32
addr = "08:3A:8D:2F:29:DA"
service_matches = find_service(address = addr)

if len(service_matches) == 0:
    print("Impossibile trovare il servizio")
    sys.exit(0)
 
# Definizione dei parametri relativi all'ESP32
first_match = service_matches[0]
port = first_match["port"]
name = first_match["name"]
host = first_match["host"]

port = 1
print("Connecting to \"%s\" on %s, port %s" % (name, host, port))

# Creazione del client socket
sock=BluetoothSocket(RFCOMM)
sock.connect((host, port))

print("Connected")

schedule.every(2).minutes.do(send_time)

if __name__ == '__main__':
	while True:
		schedule.run_pending()
		receive_data()

