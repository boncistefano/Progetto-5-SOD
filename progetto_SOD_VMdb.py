import paho.mqtt.client as mqtt
from time import time
import sqlite3


mqtt_host = '192.168.1.25'
mqtt_port = 1883
topic = 'Progetto SOD'

database_file = 'progetto.db'

# Connessione al database
conn = sqlite3.connect(database_file)  

# Creazione della tabella
with conn:
    conn.execute('''
                CREATE TABLE IF NOT EXISTS sensors_data (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    topic TEXT NOT NULL, 
                    timestamp TEXT NOT NULL,
                    temperature TEXT NOT NULL,
                    pressure TEXT NOT NULL,
                    altitude TEXT NOT NULL,
                    light TEXT NOT NULL
                );
                ''')

def on_connect(db_client, user_data, flags, conn_result):
    db_client.subscribe(topic)


def on_message(db_client, user_data, message):

    payload = message.payload.decode('utf-8').split(';')
    timestamp = payload[0]+" "+payload[1]
    temperature = payload[2]
    pressure = payload[3]
    altitude = payload[4]
    light = payload[5]
    conn = sqlite3.connect(database_file)
    with conn: 
        conn.execute("INSERT INTO sensors_data (topic, timestamp, temperature, pressure, altitude, light) VALUES (?, ?, ?, ?, ?, ?)", 
                    (message.topic, timestamp, temperature, pressure, altitude, light))
    conn.close()


db_client = mqtt.Client()

db_client.on_connect = on_connect
db_client.on_message = on_message

db_client.connect(mqtt_host, mqtt_port)
db_client.loop_forever()