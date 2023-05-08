from requests import get
from time import sleep

while(1):
    j = get("http://192.168.0.22/status").json()
    print(f"{j}")
    sleep(5)