# Smart speaker configuration and use
	## run
		After starting the hub.py, it enters the listening state (waiting for the DMCerrand's speaker channel to connect).
		After running the speakerStart.py, the program waits for the user's voice input.


	## Model training
		1. Sign in to the training site for the sound model https://snowboy.hahack.com
		2. Click Enable Mocrophone and click Record to start recording the wake word after waiting two seconds，
			the recording time is three seconds at a time, and to ensure the integrity and recognition of the models, 
			please record each model at least three times
		3. After recording, click Submit, and then click Download to download the model file you trained, and save the file in PMDL format
		4. When using wake-up words, give the full path to the wake-up word model file on the command line

	## Snowboy speech recognition code
		Source code URL：https://github.com/seasalt-ai/snowboy

	## Test the environment configuration
		When testing with different servers, modify the relevant variables in the localconfig.py