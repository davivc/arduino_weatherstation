import boto3

session = boto3.Session(region_name="us-west-2")

dynamodb = session.resource("dynamodb", endpoint_url="http://dynamodb:8000")

table = dynamodb.create_table(
    TableName="HortaData",
    KeySchema=[
        {"AttributeName": "id", "KeyType": "HASH"},  # Primary key
        {"AttributeName": "date", "KeyType": "RANGE"},  # Sort key
    ],
    AttributeDefinitions=[
        {"AttributeName": "id", "AttributeType": "S"},
        {"AttributeName": "date", "AttributeType": "S"},
    ],
    ProvisionedThroughput={"ReadCapacityUnits": 10, "WriteCapacityUnits": 10},
)

print("Table status:", table.table_status)
