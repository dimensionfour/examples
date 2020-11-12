import paho.mqtt.client as mqtt
import json
import datetime
import random
import time
import os
from dotenv import load_dotenv
load_dotenv(verbose=True, override=True)


MQTT_BROKER_ADDRESS = os.getenv("MQTT_BROKER_ADDRESS")
MQTT_BROKER_PORT = int(os.getenv("MQTT_BROKER_PORT"))
MQTT_BROKER_USERNAME = os.getenv("MQTT_BROKER_USERNAME")
MQTT_BROKER_PASSWORD = os.getenv("MQTT_BROKER_PASSWORD")

DEVICE_ID = os.getenv("DEVICE_ID")
TENANT_ID = os.getenv("TENANT_ID")
TENANT_KEY = os.getenv("TENANT_KEY")

mqttc = mqtt.Client()
mqttc.username_pw_set(username=MQTT_BROKER_USERNAME,password=MQTT_BROKER_PASSWORD)
mqttc.connect(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT, 60)
mqttc.loop_start()

while True:
    print("Sending data to dimensionfour.io:")

    payload = {
        "deviceId": DEVICE_ID,
        "tenantId": TENANT_ID,
        "tenantKey": TENANT_KEY,
        "signals": [
            {
                "value": str(round((random.randint(1800,2200) / 100.0),2)),
                "unit": "°C",
                "type":"Temperature",
                "timestamp": datetime.datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%S") + "+00:00"
            }
        ]
    }   

    print(json.dumps(payload, indent=2,ensure_ascii=False))
    (rc, mid) = mqttc.publish("POINT/PUSH", json.dumps(payload, ensure_ascii=False), qos=2)
    time.sleep(1)

mqttc.loop_stop()
mqttc.disconnect()