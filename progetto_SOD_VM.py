from datetime import date
from flask import Flask, render_template
import paho.mqtt.client as mqtt

# Creazione della web application tramite Flask 
app = Flask(__name__)
 
# Dichiarazione delle liste contententi i valori 
date_list = []
time_list = []
temp_list = []
press_list = []
alt_list = []
light_list = []

# Connessione al broker MQTT
mqtt_broker = "192.168.1.25"  
mqtt_topic = "Progetto SOD"
 

# Callback quando il client MQTT si connette al broker
def on_connect(client, userdata, flags, rc):
    print("Connesso con codice: " + str(rc))
    client.subscribe(mqtt_topic)


# Callback quando un messaggio MQTT viene ricevuto
def on_message(client, userdata, msg):

    global date_list
    global time_list
    global temp_list
    global press_list
    global alt_list
    global light_list

    payload = msg.payload.decode('utf-8').split(';')
    # Inserimento nella rispettiva lista dell'ultimo valore letto 
    date_list.insert(0, payload[0])
    time_list.insert(0, payload[1])
    temp_list.insert(0, payload[2])
    press_list.insert(0, payload[3])
    alt_list.insert(0, payload[4])
    light_list.insert(0, payload[5])

    # Eliminazione del valore meno recente quando la lista supera i 5 elementi
    if len(date_list) > 5:
        date_list.pop(5)
        time_list.pop(5)
        temp_list.pop(5)
        press_list.pop(5)
        alt_list.pop(5)
        light_list.pop(5)


# Inizializzazione del client MQTT
mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect(mqtt_broker, 1883, 60)
mqtt_client.loop_start()  

app.jinja_env.globals.update(zip=zip)

# Pagina web per visualizzare i messaggi MQTT
@app.route('/')
def index():
    return render_template('index.html', date_msg = date_list, time_msg = time_list, temp_msg = temp_list, press_msg = press_list, alt_msg = alt_list, light_msg = light_list)     

# Avvio del server Flask
if __name__ == '__main__':
    app.run(debug=True)