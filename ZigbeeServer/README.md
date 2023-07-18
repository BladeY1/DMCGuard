# Speaker Server
	Execute 'python app.py' to run the server.

	## requirement
		Enter the following command to install the dependency.
			```
			pip install -r requirements.txt
			 ```
			<!--
	## Database
		[reference](http://www.ityouknow.com/python/2019/10/30/python_web_database-046.html)


	## AWS cert
		Obtain a certificate for the item from the AWS control platform and place it under the /cert/aws folder.
		-->
	## SSL config 
		### Use mkcert
			SSL certificates can be generated using the mkcert configuration. 
			Put the obtained certificate file in the /cert folder.
			And change the SSL_KEYS_1 and SSL_KEYS_2 in the config.py
			[reference](https://www.jianshu.com/p/7cb5c2cffaaa)
		### Use the OpenSSL tool
			Use the openssl tool to manually build a CA and issue certificates.

	## config
		Must correctly fill in your server IP, ZigBee Server IP, and Speaker Server IP 
		in the MAN_ADDR, ZIGBEE_ADDR, and SPEAKER_ADDR of each config.py.