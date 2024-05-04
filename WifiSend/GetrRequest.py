import requests

# IP address of your ESP32
esp32_ip = '192.168.1.1'

# URL to send the GET request to
geturl = f'http://{esp32_ip}/download'
recurl = f'http://{esp32_ip}/received'
# Send the GET request
response = requests.get(geturl)

# Check if the request was successful (status code 200)
if response.status_code == 200:
    # Write the content of the response to a text file
    with open('downloaded_file.txt', 'w') as file:
        file.write(response.text)
    print("File downloaded successfully.")
else:
    print(f"Failed to download file. Status code: {response.status_code}: {response.text}")

response = requests.get(recurl)

print(f"Status code: {response.status_code}: {response.text}")