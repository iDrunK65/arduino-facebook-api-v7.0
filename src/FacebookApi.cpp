#include "FacebookApi.h"

FacebookApi::FacebookApi(Client &client, String accessToken, String appId = "", String appSecret = "") {
	this->client = &client;
  _accessToken = accessToken;
	_appId = appId;
	_appSecret = appSecret;
}

String FacebookApi::sendGetToFacebook(String command) {
	String body;
	String headers;
	bool finishedHeaders = false;
	bool currentLineIsBlank = true;
	unsigned long now;
	bool avail;
	// Connect with facebook api over ssl
	if (client->connect(FB_API_HOST, FB_API_SSL_PORT)) {
		//Serial.println(".... connected to server");
		String a="";
		char c;
		int ch_count=0;
		client->println("GET " + command +  " HTTP/1.1");
		client->println("Host: " FB_API_HOST);
		client->println("User-Agent: arduino/1.0.0");
		client->println("");
		now=millis();
		avail=false;
		while (millis() - now < FB_API_TIMEOUT) {
			while (client->available()) {

				// Allow body to be parsed before finishing
				avail = finishedHeaders;
				char c = client->read();
				// Serial.write(c);

				if(!finishedHeaders){
					if (currentLineIsBlank && c == '\n') {
						finishedHeaders = true;
						//Serial.println(headers);
					}
					else{
						headers = headers + c;

					}
				} else {
					if (ch_count < maxMessageLength)  {
						body=body+c;
						ch_count++;
					}
				}

				if (c == '\n') {
					currentLineIsBlank = true;
				}else if (c != '\r') {
					currentLineIsBlank = false;
				}
			}
			if (avail) {
        // For some reason the body is coming back with a number on the outside of the start and end of it, we only want the JsonObject
        int firstJsonCharacterIndex = body.indexOf("{");
        int lastJsonCharacterIndex = body.lastIndexOf("}");
        if(firstJsonCharacterIndex != -1 && lastJsonCharacterIndex != -1){
          body = body.substring(firstJsonCharacterIndex, lastJsonCharacterIndex + 1);
        }
				// Serial.println("Body:");
				// Serial.println(body);
				// Serial.println("END");
				break;
			}
		}
	}
	return body;
}

String FacebookApi::getFriends(){
	String command = "/v7.0/me/friends?access_token="+_accessToken;
	//Serial.println(command);
	return sendGetToFacebook(command);  //recieve reply from facebook
}

String FacebookApi::extendAccessToken() {
		extendAccessToken(_appId , _appSecret);
}

String FacebookApi::extendAccessToken(String appId, String appSecret){
	String command="/oauth/access_token?client_id="+ appId +"&client_secret=" + appSecret + "&grant_type=fb_exchange_token&fb_exchange_token="+_accessToken;
	//Serial.println(command);
	String response = sendGetToFacebook(command);
  DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);
	if(!error) {
		if (doc.containsKey("access_token")) {
			Serial.println("Updated Token");
			_accessToken = doc["access_token"].as<String>();
			return _accessToken;
		} else {
      Serial.println("JSON respnse was not as expected");
    }
  } else {
    Serial.println("Failed to parse JSON");
  }
	return "";
}

int FacebookApi::getTotalFriends(){
  String response = getFriends();
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);
	if(!error) {
		if (doc.containsKey("summary")) {
			return doc["summary"]["total_count"].as<int>();
		} else {
      Serial.println("JSON respnse was not as expected");
    }
  } else {
    Serial.println("Failed to parse JSON");
  }

  return -1;
}

/**
 * From the official Facebook API documentation
 *
 * "The number of users who like the Page.
 *  For Global Pages this is the count for all Pages across the brand."
 *
 *  https://developers.facebook.com/docs/graph-api/reference/page
 */
int FacebookApi::getPageFanCount(String pageId) {
	String command = "/v7.0/" + pageId + "?fields=fan_count&access_token=" + _appId;
	String response = sendGetToFacebook(command);

    const size_t capacity = JSON_OBJECT_SIZE(2) + 40;
	DynamicJsonDocument doc(capacity);

    DeserializationError error = deserializeJson(doc, response);

	Serial.println(command);

	if(!error) {

		deserializeJson(doc, response);

		if(doc.containsKey("fan_count")) {
			return doc["fan_count"].as<int>();
		} else {
			Serial.println("JSON respnse was not as expected");
		}
  	}	else {
		Serial.println("Failed to parse JSON");
	}
  	return -1;
}