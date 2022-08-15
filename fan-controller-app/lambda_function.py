import boto3
import json

client = boto3.client('iot-data', region_name='us-east-2')

def lambda_handler(event, context):
    print('received event:')
    print(event)
    print('received context:')
    print(context)
    
    if (event['key'] == 'TheRagsdales'):
        response = client.publish(
            topic='master_bedroom/fan/control',
            qos=1,
            payload=json.dumps({"message": event['message'] })
        )
        print('mqtt response:')
        print(response)
        return {
            'statusCode': 200,
            'body': response
        }
    else:
        print('Missing or invalid key')
        return {
            'statusCode': 401,
            'body': json.dumps('Unauthorized')
        }
