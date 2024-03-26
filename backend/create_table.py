import boto3

dynamodb = boto3.resource('dynamodb', endpoint_url='http://localhost:8000')

table = dynamodb.create_table(
    TableName='BME280Data',
    KeySchema=[
        {
            'AttributeName': 'date',
            'KeyType': 'HASH'  # Partition key
        }
    ],
    AttributeDefinitions=[
        {
            'AttributeName': 'date',
            'AttributeType': 'S'
        },
    ],
    ProvisionedThroughput={
        'ReadCapacityUnits': 10,
        'WriteCapacityUnits': 10
    }
)

print("Table status:", table.table_status)