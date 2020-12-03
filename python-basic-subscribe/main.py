from gql import gql, Client, WebsocketsTransport
import os
from dotenv import load_dotenv


load_dotenv(verbose=True, override=True)
SUBSCRIPTION_ADDRESS = os.getenv("SUBSCRIPTION_ADDRESS")
TENANT_ID = os.getenv("TENANT_ID")
TENANT_KEY = os.getenv("TENANT_KEY")


transport = WebsocketsTransport(
    url=SUBSCRIPTION_ADDRESS,
    headers={
    'x-tenant-id': TENANT_ID, 
    "x-tenant-key": TENANT_KEY
})

client = Client(
    transport=transport,
    fetch_schema_from_transport=True,
)

query = gql('''
 subscription {
  signalAdded {
    id
    mac
    timestamp
    unit
    type
    data {
        rawValue
    }
  }
}
''')


for result in client.subscribe(query):
    print (result)    