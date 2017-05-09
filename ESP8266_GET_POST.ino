#include <ESP8266WiFi.h>

const char* ssid = "XXXX";
const char* password = "XXXX";
String publicIP = "";
String cloudURI = "/XXXX/XXX/data/MYIP_ADDR";

IPAddress ip(192, 168, 1, 100);  // Desired IP Address
IPAddress gateway(192, 168, 1, 1); // set gateway to match your network
WiFiServer server(80);
WiFiClient client;
WiFiClient publicIpClient;
WiFiClient cloudClient;
char IpaddressServer[] = "checkip.dyndns.org";
char cloudServer[] = "api.backendless.com";

void setup() {

  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet); 

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  // Get the Public Ip from this service http://checkip.dyndns.org/ and api.ipify.org
  
  if(publicIpClient.connect(IpaddressServer,80))
  {
       Serial.println("Connected To checkip.dyndns.org ");
       publicIpClient.println("GET HTTP/1.1");
       publicIpClient.println("Host: checkip.dyndns.org");
       publicIpClient.println("Connection: close");
       publicIpClient.println();
       
      Serial.println("IP Client Response available");

      while (publicIpClient.available()) 
      {
        //char c = publicIpClient.read();
        String c = publicIpClient.readStringUntil('\r');
        //Serial.write(c);
        publicIP = publicIP + c;
      } 
      publicIpClient.flush();

      publicIpClient.stop();   

      delay(1);

    if(cloudClient.connect(cloudServer,80))
      {
          publicIP.replace("\n","");
          String content = String("{ \"Addr\" : \"") + publicIP + String(" \" }");          
          String length = String(content.length());
          
          Serial.println("Connected To backendless ");
          Serial.println("Content " + content);
          Serial.println("Content Length " + length);
          
          cloudClient.println("POST "+ cloudURI +" HTTP/1.1");
          cloudClient.println("Host: api.backendless.com");
          cloudClient.println("Accept: */*");
          cloudClient.println("Content-Length: " + length);
          cloudClient.println("Content-Type: application/json");    
          //publicIpClient.println("Connection: close");
          cloudClient.println();
          cloudClient.println(content);

          Serial.println("POST END");
          while (cloudClient.available()) 
          {
            Serial.write(cloudClient.read());
          } 
          
          cloudClient.flush();
      }
      publicIpClient.stop();
      cloudClient.stop();
  }
}

void loop() {

  client = server.available();
  if (!client) {
    return;
  }

  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1>Sudhakar Home<h1>");
  client.println("<table>");
  client.println("<tr>");
  client.println("<td> Public IP Address :");
  client.println("</td>");
  client.println("<td>");
  client.println(publicIP);
  client.println("<td>");
  client.println("</td>");
  client.println("</tr>");
  client.println("</table>");
  client.println("<br><br>");
  client.println("</html>"); 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}
