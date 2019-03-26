#include <IIPClient.h>

IIPClient::IIPClient(String IIPServer) {
  _IIPServer=IIPServer;
}

IIPClient::~IIPClient() {
}

// ----------------------------------------------------------------------------
// IIPCreateArtifactId()
// Crates a unique Id to be used as artifact id
// unique id: MAC-adress + random number + locale date + local time
// ----------------------------------------------------------------------------
String IIPClient::IIPCreateArtifactId()
{
  String artifactId;
  time_t now;
  struct tm *t;
  byte mac[6];
  char macbuffer[13];
  char tmbuffer[16];
  char rndbuffer[5];
  
  // get current system time  
  time(&now);
  t = localtime(&now);  
  snprintf(tmbuffer, sizeof(tmbuffer), "%04d%02d%02d%02d%02d%02d", 
          1900+t->tm_year, t->tm_mon+1, t->tm_mday,
          t->tm_hour, t->tm_min, t->tm_sec);

  // get random number
  randomSeed(analogRead(0));
  snprintf(rndbuffer, sizeof(rndbuffer), "%04d", (int)random(0,9999));

  // get mac address
  WiFi.macAddress(mac);
  snprintf(macbuffer, sizeof(macbuffer), "%02x%02x%02x%02x%02x%02x",
                  mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

  // compose unique Id
  artifactId=String(macbuffer) + String(rndbuffer) + String(tmbuffer);
  Serial.printf("[Id] %s\n", artifactId.c_str());
 
  return artifactId;
}

// ----------------------------------------------------------------------------
// IIPSendFile()
// Uploads a file to the IIP data management system
// ----------------------------------------------------------------------------
void IIPClient::IIPSendFile(String FilePathAndName)
{
  String httpPost;
  String artifactId=IIPClient::IIPCreateArtifactId();  // needs to be a unique ID
  String boundary=artifactId; 

  // compose destination
  _httpCmd = "http://" + _IIPServer + "/dsms/v2.2/artifact/" + artifactId;
  _httpMethod="POST";

  // compose post message
  httpPost="------" + boundary;
  httpPost+="\r\nContent-Disposition: form-data; name=\"file_af\"; filename=\"/pic/1.jpg\"\r\n";
  httpPost+="Content-Type: image/jpeg\r\n\r\n\r\n------";
  httpPost+=boundary + "--";

  _iip.begin(_httpCmd);                                         // specify request destination
  _iip.addHeader("Content-Type", "application/form_data");     // specify content-type header
  _iip.addHeader("cache-control", "no-cache");                 
  _iip.addHeader("content-type", "multipart/form-data; boundary=----" + boundary); 
  _httpCode  =_iip.POST(httpPost);     // post message 
  _httpReturn=_iip.getString();        // get the response 
  _iip.end();

  _httpCmd+="\nContent-Type: application/form_data\n";
  _httpCmd+="cache-control: no-cache\n";
  _httpCmd+="content-type multipart/form-data; boundary=" + boundary + "\n";
  _httpCmd+=httpPost;

  HTTPResult();
}

// ----------------------------------------------------------------------------
// IIPdsms()
// ----------------------------------------------------------------------------
void IIPClient::IIPdsms() {
  _httpCmd = "http://" + _IIPServer + "/dsms/v2.2/";
  _httpMethod="GET";

  _iip.begin(_httpCmd); //HTTP
  _httpCode  =_iip.GET();
  _httpReturn=_iip.getString();
  _httpString=_iip.errorToString(_httpCode);
  _iip.end();

  HTTPResult();
}

// ----------------------------------------------------------------------------
// HTTPResult()
// ----------------------------------------------------------------------------
void IIPClient::HTTPResult() {
  Serial.printf("[IIP] %s %s\n", _httpMethod.c_str(), _httpCmd.c_str());

  // httpCode will be negative on error
  if (_httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[IIP] return code: %d\n", _httpCode);
    Serial.printf("[IIP] return data:\n");
    Serial.printf("%s\n", _httpReturn.c_str());
  } else {
    Serial.printf("[IIP] return code: %d, error: %s\n", _httpCode, _httpString.c_str());
  }
}
  
  
