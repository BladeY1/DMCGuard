try:
    import serial
    import sys
    from time import *
    import serial.tools.list_ports
    import requests
    import localconfig
except ImportError:
    print("Import error occurred")
    exit(0)


# --------------------------------------------------#
# Function: open serial port, exit if having error
# --------------------------------------------------#

platform = sys.platform
serial_port = "/dev/ttyUSB0" if platform == "linux" else "COM3"
try:
    ser = serial.Serial(serial_port, baudrate=9600, timeout=5.0)
except serial.serialutil.SerialException:
    print("Current platform: " + platform)
    serial_port = "/dev/ttyUSB1" if platform == "linux" else "COM4"
    ser = serial.Serial(serial_port, baudrate=9600, timeout=5.0)

try:
    # insure that the serial port is closed before
    ser.close()
    ser.open()
except serial.SerialException:
    print("Please check your serial port")
    exit(0)

# --------------------------------------------------#
# Function: get device_key form server, send the key to device insuring that the device is certified
# --------------------------------------------------#


def main() -> None:
    """ """
    # --------------------------------------------------#
    # get device key from server first
    # if the server is not connected, hub will continue to wait for the server to connect
    # once connected, device_key will be sent to device
    # --------------------------------------------------#

    device_key: str = ""
    data_key: dict = {"key": localconfig.server_key}

    # --------------------------------------------------#
    # get token for reading status and brightness
    # --------------------------------------------------#

    while device_key == "" or device_key == None:
        try:
            print(localconfig.zigbee_server_url + "/api/devicekey")
            response = requests.post(
                url=localconfig.zigbee_server_url + "/api/devicekey", data=data_key, verify=localconfig.ca
            )
            data_response = response.json()
            if data_response["msg"] == "success":
                device_key = data_response["devicekey"]
                print("Device key recieved: {}".format(device_key))
            else:
                print("Device key not received")
                sleep(2)
        #except requests.exceptions.RequestException:
        except Exception as e:
            print(e)
            print("Server is not connected")
        

    # --------------------------------------------------#
    # get read status and brightness tokens from server
    # --------------------------------------------------#

    status_token: str = getToken(R="0")
    brightness_token: str = getToken(R="2")
    color_token: str = getToken(R="4")

    # --------------------------------------------------#
    # first send an msg to activate the device
    # then send the device_key to device
    # check if the device is certified
    # --------------------------------------------------#

    print("Waiting for certification")
    ser.write("$WAITINGFORCERTIFY\n".encode("utf-8"))

    is_certified: bool = False
    uncertified_count: int = 0

    while is_certified != True:
        # --------------------------------------------------#
        # If uncertified_count is greater than 5, get device_key from server again
        # --------------------------------------------------#

        if uncertified_count > 5:
            return

        # --------------------------------------------------#
        # Normally, the device_key will be sent to device
        # --------------------------------------------------#

        ser.write("${}\n".format(device_key).encode("utf-8"))

        # sleep(0.3)

        certify_msg = ser.readline().decode("utf-8")
        print(certify_msg)
        if certify_msg != "$" and certify_msg != "":
            print("CERTIFIED")
            is_certified = True
        else:
            uncertified_count += 1

    # --------------------------------------------------#
    # Function: transfer msg between device and server
    # when sending msg to the device, if timeout( > 5s), reconnect to the device
    # --------------------------------------------------#

    # init data_send_to_server
    status: str = "init"
    brightness: str = "init"
    color: str = "init"

    while True:

        # --------------------------------------------------#
        # first get status brightness and color from device
        # --------------------------------------------------#

        data_send_status = "$" + status_token + "0" + "0" + "0" + "\n"
        status = sendMsg(data_send_status)
        if status == None:
            return

        data_send_brightness = "$" + brightness_token + "2" + "0" + "0" + "\n"
        brightness = sendMsg(data_send_brightness)
        if brightness == None:
            return

        data_send_color = "$" + color_token + "4" + "0" + "0" + "\n"
        color = sendMsg(data_send_color)
        if color == None:
            return


        # sleep(2)

        # --------------------------------------------------#
        # Function: get operation msg from server by posting status and brightness
        # --------------------------------------------------#
        try:
            response = requests.post(
                url=localconfig.zigbee_server_url + "/api/status",
                data={
                    "key": localconfig.server_key,
                    "status": status,
                    "brightness": brightness,
                    "color": color,
                },
                verify=localconfig.ca,
            )
            data_response = response.json()
            print(data_response)
        except requests.exceptions.RequestException:
            print("Server is not connected")
            sleep(2)
            continue

        # --------------------------------------------------#
        # Function: send operation msg to device
        # --------------------------------------------------#

        if data_response["msg"] == "success":
            data_send_operate = (
                "$"
                + data_response["token"]
                + data_response["R"]
                + "0"
                + data_response["param"]
                + "\n"
            )
            ser.write(data_send_operate.encode("utf-8"))
        else:
            continue

        time_zero = time()
        recv_msg = ser.readline().decode("utf-8").replace("\n", "")
        if recv_msg == "":
            if time() - time_zero > 10:
                print("10s no response. Reconnecting to device")
                return
            continue


# --------------------------------------------------#
# get designated token by R
# --------------------------------------------------#
def getToken(R: str) -> str:
    token: str = ""
    while token == "" or token == None:
        try:
            response = requests.post(
                url=localconfig.zigbee_server_url + "/api/token",
                data={"key": localconfig.server_key, "R": R},
                verify=localconfig.ca,
            )
            data_response = response.json()
            print(data_response)
        except requests.exceptions.RequestException:
            print("Server is not connected")
            sleep(2)
            continue
        if data_response["msg"] == "success":
            token = data_response["token"]
            if token == None:
                continue
            print("Token {} recieved for R {}".format(token, R))
        else:
            print("Token not received")
            sleep(2)

    return token


# --------------------------------------------------#
# send msg to device
# --------------------------------------------------#
def sendMsg(msg: str) -> str:

    
    while True:
        time_zero = time()
        ser.write(msg.encode("utf-8"))
        recv_msg = ser.readline().decode("utf-8").replace("\n", "")

        if len(msg) > 33 and recv_msg != '':
            print(msg)
            print(recv_msg)
            if msg[33] == '0' and recv_msg not in ['lon', 'lof', 'err']:
                continue
            elif msg[33] == '2' and recv_msg not in [chr(i) for i in range(10, 120, 10)]:
                continue
            elif msg[33] == '4' and recv_msg not in ['red', 'yellow', 'green']:
                continue

        if recv_msg == "":
            if time() - time_zero > 10:
                print("10s no response. Reconnecting to device")
                return None
            continue
        else:
            return recv_msg


try:
    if __name__ == "__main__":
        while True:
            main()
except KeyboardInterrupt:
    ser.close()
    print("\n\nProgram terminated by user")
