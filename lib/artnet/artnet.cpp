#include <artnet.h>

Artnet::Artnet() {}

boolean Artnet::connected = false;
 WiFiUDP Artnet::udp;

void Artnet::begin(const char * ssid, const char * pw)
{
  //Artnet::connected = false;
  //Udp.begin(ART_NET_PORT);
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  //WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);

  //Initiate connection
  WiFi.begin(ssid, pw);

  Serial.println("Waiting for WIFI connection...");
  int tries=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tries++;
    if (tries > 30){
      break;
    }
  }
}

uint16_t Artnet::read()
{
  packetSize = Artnet::udp.parsePacket();

  if (packetSize <= MAX_BUFFER_ARTNET && packetSize > 0)
  {

      Artnet::udp.read(artnetPacket, MAX_BUFFER_ARTNET);

      // Check that packetID is "Art-Net" else ignore
      for (byte i = 0 ; i < 8 ; i++)
      {
        if (artnetPacket[i] != ART_NET_ID[i])
          return 0;
      }

      opcode = artnetPacket[8] | artnetPacket[9] << 8;

      if (opcode == ART_DMX)
      {
        sequence = artnetPacket[12];
        incomingUniverse = artnetPacket[14] | artnetPacket[15] << 8;
        dmxDataLength = artnetPacket[17] | artnetPacket[16] << 8;

        if (artDmxCallback) (*artDmxCallback)(incomingUniverse, dmxDataLength, sequence, artnetPacket + ART_DMX_START);
        return ART_DMX;
      }
      if (opcode == ART_POLL)
      {
        return ART_POLL;
      }
      return 0;
  }
  else
  {
    return 0;
  }
}

void Artnet::printPacketHeader()
{
  Serial.print("packet size = ");
  Serial.print(packetSize);
  Serial.print("\topcode = ");
  Serial.print(opcode, HEX);
  Serial.print("\tuniverse number = ");
  Serial.print(incomingUniverse);
  Serial.print("\tdata length = ");
  Serial.print(dmxDataLength);
  Serial.print("\tsequence n0. = ");
  Serial.println(sequence);
}

void Artnet::printPacketContent()
{
  for (uint16_t i = ART_DMX_START ; i < dmxDataLength ; i++){
    Serial.print(artnetPacket[i], DEC);
    Serial.print("  ");
  }
  Serial.println('\n');
}

void Artnet::WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());
          //initializes the UDP state
          //This initializes the transfer buffer

          Artnet::udp.begin(WiFi.localIP(),ART_NET_PORT);
          Artnet::connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          Artnet::connected = false;
          break;
    }
}
