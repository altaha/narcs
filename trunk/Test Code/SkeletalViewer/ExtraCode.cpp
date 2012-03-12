/*
		stringstream xCoordNumToStringConverter,
					 yCoordNumToStringConverter,
					 zCoordNumToStringConverter;
		string xCoordString,
			   yCoordString,
			   zCoordString,
			   positionUpdateMessage;


		xCoordNumToStringConverter << xCoordNum;
		xCoordNumToStringConverter >> xCoordString;
	
		yCoordNumToStringConverter << yCoordNum;
		yCoordNumToStringConverter >> yCoordString;
	
		zCoordNumToStringConverter << zCoordNum;
		zCoordNumToStringConverter >> zCoordString;

		positionUpdateMessage = ", " + xCoordString + ", " + yCoordString + ", " + zCoordString;
		m_socketConnectivity.SendMessage(positionUpdateMessage);
		*/


/*
	string messageLengthString = "";
	unsigned int prevMessageLength = 0,
				 currMessageLength = messageString.size() +
				 	 	 	 	 	 messageLengthString.size();

	// Determine the length of the message
	do
	{
		prevMessageLength = currMessageLength;

		stringstream numStringConverter;
		numStringConverter << currMessageLength;
		numStringConverter >> messageLengthString;

		currMessageLength = messageString.size() +
							messageLengthString.size();
	} while(prevMessageLength != currMessageLength);

	string totalMessageString = messageLengthString + messageString;
	char *messageBuffer = new char[currMessageLength + 1];
	strcpy(messageBuffer, totalMessageString.c_str());
	*/

