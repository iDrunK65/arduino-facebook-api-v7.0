#ifndef FacebookApi_h
#define FacebookApi_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#define FB_API_HOST "graph.facebook.com"
#define FB_API_SSL_PORT 443
#define FB_API_TIMEOUT 1500

class FacebookApi
{
  public:
    FacebookApi(Client &client, String accessToken, String appId, String appSecret);
    String getFriends();
    int getTotalFriends();
    int getPageFanCount(String pageId);
    String extendAccessToken();
    String extendAccessToken(String appId, String appSecret);
    String sendGetToFacebook(String command);

  private:
    Client *client;
    String _appId;
    String _appSecret;
    String _accessToken;
    const int maxMessageLength = 10000;
};

#endif