from flask import Flask, request, abort
import firebaseMessage as fcm
import firebaseDatabase as fdb
import firebase_admin
from firebase_admin import exceptions
from firebase_admin import db
from firebase_admin import credentials

app = Flask(__name__)

verifyKey = "MMZLSJ"

cred = credentials.Certificate("cred/serviceAccountKey.json")
default_app = firebase_admin.initialize_app(cred, {
    'databaseURL': "https://sensdoor-d5505-default-rtdb.europe-west1.firebasedatabase.app/"
})

arr = {}

@app.route('/send', methods=['POST'])
def receive_from_sensor():
    data = request.form

    if verifyKey != data["api_key"]:
        return abort(401, message="Unauthorized")

    if data.get("sensor_id") is None or data.get("status") is None:
        return abort(400, message="Bad Request")

    if str(data.get("sensor_id")) not in arr.keys() or str(arr[str(data.get("sensor_id"))]) != str(data.get("status")):
        arr[str(data.get("sensor_id"))] = str(data.get("status"))

        response = fdb.update_sensor_state(
            sensor_id=data.get("sensor_id"),
            status=int(data.get("status"))
        )

        if response is exceptions.FirebaseError:
            return abort(409, message="Operation Failed")
        
        if response is db.reference:
            fcm.send_message(
                data.get("sensor_id"),
                data.get("status")
            )

            return ''
    else:
        print("Useless update")

    return ''


@app.route('/add', methods=['POST'])
def add_sensor():
    data = request.form

    if verifyKey != data["api_key"]:
        return abort(401, message="Unauthorized")

    if data.get("sensor_name") is None or data.get("status") is None:
        return abort(400, message="Bad Request")

    response = fdb.add_new_sensor(
        sensor_name=data.get("sensor_name"),
        status=int(data.get("status")),
        sens_type=0
    )

    if response is exceptions.FirebaseError:
        return abort(409, message="Operation Failed")

    if response is db.reference:
        return response

    return str(response)


app.run(debug=True, port=8080, host="0.0.0.0")
