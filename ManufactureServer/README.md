# Manufacture Server
	Execute 'python app.py' to run the server.

	## requirement
		Enter the following command to install the dependency.
		```
		pip install -r requirements.txt
		 ```
		<!--
		## Database
		[reference](http://www.ityouknow.com/python/2019/10/30/python_web_database-046.html)
		-->

	## AWS cert
		Obtain a certificate for the item from the AWS control platform and place it under the /cert/aws folder.

	## SSL config 
		### Use mkcert
			SSL certificates can be generated using the mkcert configuration. 
			Put the obtained certificate file in the /cert/server folder.
			And change the SSL_KEYS_1 and SSL_KEYS_2 in the config.py
			[reference](https://www.jianshu.com/p/7cb5c2cffaaa)
		### Use the OpenSSL tool
			Use the openssl tool to manually build a CA and issue certificates.
			
# !  Please note that you must correctly fill in your server IP, ZigBee Server IP, 
	 and Speaker Server IP in the MAN_ADDR, ZIGBEE_ADDR, and SPEAKER_ADDR of each config.py.
	 If you want to deploy three servers at the same time on a remote cloud server, 
	 you can use different ports such as 5001, 5002, 5003, just change the 501 line "port = " in the app.py.
	 But you need to add two IPs to the config.py: ZIGBEE_IP and SPEAKER_IP, 
	 and change the ZIGBEE_ADDR and SPEAKER_ADDR of the 448 lines of the app.py to ZIGBEE_IP and SPEAKER_IP
