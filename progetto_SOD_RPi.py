from bluetooth import *
import paho.mqtt.client as mqtt
from datetime import datetime

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
    
    now = datetime.now()
    current_time = now.strftime("%d/%m/%y %H:%M:%S")
    print(current_time)
    sock.send(current_time)
    
# Ricezione tramite bluetooth delle misurazione dall'ESP32        
def receive_data():
    
    while True:
        data = sock.recv(2048)
        if data:
            data = data.decode()
            print(data)
            
            # Pubblicazione del topic da parte del client MQTT
            client.publish(publish_topic, str(data), 1) 
            
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

send_time()
receive_data()

sock.close()
