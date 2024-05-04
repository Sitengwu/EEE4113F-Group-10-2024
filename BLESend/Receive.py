from bluepy import Peripheral, UUID, DefaultDelegate
import time

# UUID of the BLE service and characteristic on the ESP32
service_uuid = "bb486657-341b-44f1-9594-41136946a471"
char_uuid = "293a7b0d-7067-4d0c-a60e-eec6b41ad45e"

# File to save the received data
file_name = "received_file.txt"

class NotificationDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleNotification(self, cHandle, data):
        with open(file_name, "ab") as f:
            f.write(data)
            print("Received data:", data)

# Connect to the ESP32 peripheral
peripheral = Peripheral("ESP32_MAC_ADDRESS")

# Get the service and characteristic
service = peripheral.getServiceByUUID(UUID(service_uuid))
char = service.getCharacteristics(UUID(char_uuid))[0]

# Set up notification delegate
delegate = NotificationDelegate()
peripheral.setDelegate(delegate)

# Enable notifications
peripheral.writeCharacteristic(char.valHandle + 1, b"\x01\x00", True)

# Wait for notifications
try:
    while True:
        if peripheral.waitForNotifications(1.0):
            continue
        print("Waiting...")
finally:
    peripheral.disconnect()
