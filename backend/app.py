from flask import Flask, request, jsonify
import boto3

app = Flask(__name__)
app.debug = True

session = boto3.Session(region_name="us-west-2")

dynamodb = session.resource("dynamodb", endpoint_url="http://dynamodb:8000")
table = dynamodb.Table("BME280Data")


@app.route("/bme280", methods=["GET", "POST"])
def bme280_data():
    if request.method == "POST":
        data = request.get_json()
        table.put_item(Item=data)
        return jsonify({"message": "Data received"}), 200

    response = table.scan()
    data = response["Items"]
    return jsonify(data), 200


@app.route("/rain_check", methods=["GET"])
def get_rain_check():
    # You need to implement the logic for checking if it's going to rain
    is_going_to_rain = False
    return jsonify({"is_going_to_rain": is_going_to_rain}), 200


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
