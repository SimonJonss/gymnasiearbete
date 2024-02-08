from firebase_admin import messaging


def send_message(sensor_id, state):
    response = messaging.send(
        messaging.Message(
            data={
                'id': sensor_id,
                'state': state
            },
            topic=sensor_id
        )
    )
    return response
