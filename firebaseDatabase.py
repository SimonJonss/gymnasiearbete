from firebase_admin import db
from firebase_admin import exceptions
import time


def add_new_sensor(sensor_name, status, sens_type):
    ref = db.reference("sensor")
    info = ref.get(False, True)

    sensId = int(list(info)[-1]) + 1

    new_sensor = ref.child(str(sensId))

    try:
        new_sensor.set({
            'name': sensor_name,
            'status': status,
            'timestamp': int(round(time.time() * 1000)),
            'type': sens_type,
        })
        return new_sensor.key
    except exceptions.FirebaseError as exc:
        return exc


def update_sensor_state(sensor_id, status):
    ref = db.reference("/sensor")
    sensor_ref = ref.child(sensor_id)
    try:
        sensor_ref.update({
            'status': status,
            'timestamp': int(round(time.time() * 1000))
        })
        return sensor_ref
    except exceptions.FirebaseError as exc:
        return exc


def get_sensor_name(sensor_id):
    ref = db.reference("/sensor").child(sensor_id)
    name = ref.get()
    return name