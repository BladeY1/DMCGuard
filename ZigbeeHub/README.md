# Zigbee Hub Configuration And Use

	## 1 Software preparation

		### 1.1 Raspberry Pi as Hub

			Copy the hub.py to your working directory and install the required dependencies:

			```bash
			pip install -r ./hub/requirements.txt
			```

			Please note that if you use other USB serial devices, please modify them in 'hub.py'.

			It is recommended to use a virtual environment or package manager for environment isolation 
			so as not to affect the operation of other software.

			In './hub/localconfig.py', you need to fill in the Zigbee server address, server connection key, 
			and HTTPS root certificate. Here's an example:

			```python
			zigbee_server_url = "https://zigbee.server"
			server_key = "test"
			ca = "cert/rootCA.pem"
			```

		### 1.2 Zigbee Development board

			Use IAR Embedded Workbench with Ti Z-Stack environment configured to open './Z-Stack-trans/Projects/zstack/ZMain/TI2530DB/ZMain.c', 
			connect to the Zigbee emulator and then burn.

			Please note that the two development boards need to be burned Coordinator and EndDevice respectively.

			![Burn Link](https://picx.zhimg.com/80/v2-b4a1ffdfa175b9023729cc62d2e55877_r.jpg)

		### 2 Hardware preparation

			As shown in the figure below, the Raspberry Pi and Zigbee development boards are connected as hubs. 
			The Raspberry Pi running DMCGuard needs to make the same connection.

			Please note that the power supply mode of the development board should be USB, 
			otherwise the serial port function will not be available.

			![Connect with the Raspberry Pi](https://picx.zhimg.com/80/v2-8612c2eb74077adeadcaa50639a77627_r.jpg)

		### 3 Run

			'hub.py' uses a small number of version features and needs to be run with Python 3.5 and above.

			Since the serial port is read and written, you need to run 'hub.py' with administrator privileges.

			```bash
			sudo python3 ./hub/hub.py
			```
