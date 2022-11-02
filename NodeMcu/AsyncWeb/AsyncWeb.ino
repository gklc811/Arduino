#include <ESP8266WiFi.h>

#include <ESP8266WebServer.h>

/* Put your SSID & Password */
const char * ssid = "GklWifi1"; // Enter SSID here
const char * password = "OpexLlama-12#"; //Enter Password here

WiFiServer server(80);

int LED = 2; // Assign LED1 to pin GPIO16
int SC_AC = 16;
int FC_AC = 5;
int SC_DC_P = 4;
int SC_DC_N = 14;
int FC_DC_P = 12;
int FC_DC_N = 13;

int sc_status = 0;
int fc_status = 0;
int sc_on_time = 60000 * 30;
int fc_on_time = 0;
int sc_off_time = 0;
int fc_off_time = 0;

int scf_on = 1;
int scf_of = 0;
int fcf_on = 0;
int fcf_of = 0;

int time_step = 60000 * 30;

long initial_time = millis();

void setup() {

  // initialize GPIO2 and GPIO16 as an output

  pinMode(LED, OUTPUT);
  pinMode(SC_AC, OUTPUT);
  pinMode(FC_AC, OUTPUT);
  pinMode(SC_DC_P, OUTPUT);
  pinMode(SC_DC_N, OUTPUT);
  pinMode(FC_DC_P, OUTPUT);
  pinMode(FC_DC_N, OUTPUT);

  digitalWrite(LED, HIGH);
  digitalWrite(SC_AC, HIGH);
  digitalWrite(FC_AC, HIGH);
  digitalWrite(SC_DC_P, HIGH);
  digitalWrite(SC_DC_N, HIGH);
  digitalWrite(FC_DC_P, HIGH);
  digitalWrite(FC_DC_N, HIGH);

  Serial.begin(115200);

  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
  Serial.println("HTTP server started");

}

// the loop function runs forever

void loop() {
  long current_time = millis();
  long elapsed_time = current_time - initial_time;
  if (sc_status == 0 && scf_on == 1) {
    if ((sc_on_time - elapsed_time) < 0) {
      sc_status = 1;
      scf_on = 0;
      slow_charger_on();
      sc_on_time = 0;
    }
  }

  if (sc_status == 1 && scf_of == 1) {
    if ((sc_off_time - elapsed_time) < 0) {
      sc_status = 0;
      scf_of = 0;
      slow_charger_off();
      sc_off_time = 0;
    }
  }

  if (fc_status == 0 && fcf_on == 1) {
    if ((fc_on_time - elapsed_time) < 0) {
      fc_status = 1;
      fcf_on = 0;
      fast_charger_on();
      fc_on_time = 0;
    }
  }

  if (fc_status == 1 && fcf_of == 1) {
    if ((fc_off_time - elapsed_time) < 0) {
      fc_status = 0;
      fcf_of = 0;
      fast_charger_off();
      fc_off_time = 0;
    }
  }

  if (current_time % 1000 == 0) {
    Serial.print("sc_status : ");
    Serial.println(sc_status);
    Serial.print("fc_status : ");
    Serial.println(fc_status);
    Serial.print("sc_on_time : ");
    Serial.println(sc_on_time);
    Serial.print("fc_on_time : ");
    Serial.println(fc_on_time);
    Serial.print("sc_off_time : ");
    Serial.println(sc_off_time);
    Serial.print("fc_off_time : ");
    Serial.println(fc_off_time);
    Serial.print("scf_on : ");
    Serial.println(scf_on);
    Serial.print("scf_of : ");
    Serial.println(scf_of);
    Serial.print("fcf_on : ");
    Serial.println(fcf_on);
    Serial.print("fcf_of : ");
    Serial.println(fcf_of);
    Serial.print("elapsed_time : ");
    Serial.println(elapsed_time);
    Serial.println("************************************************************************");
  }

  delay(30);

  WiFiClient client = server.available();
  if (!client) {
    delay(10);
    return;
  }
  Serial.println("Waiting for new client");
  while (!client.available()) {
    delay(20);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if (request.indexOf("/scon=-") != -1) {
    sc_on_time = sc_on_time - time_step;
    scf_on = 1;
    led_notify();
  }
  if (request.indexOf("/scon=+") != -1) {
    sc_on_time = sc_on_time + time_step;
    scf_on = 1;
    led_notify();
  }
  if (request.indexOf("/fcon=-") != -1) {
    fc_on_time = fc_on_time - time_step;
    fcf_on = 1;
    led_notify();
  }
  if (request.indexOf("/fcon=+") != -1) {
    fc_on_time = fc_on_time + time_step;
    fcf_on = 1;
    led_notify();
  }
  if (request.indexOf("/scoff=-") != -1) {
    sc_off_time = sc_off_time - time_step;
    scf_of = 1;
    led_notify();
  }
  if (request.indexOf("/scoff=+") != -1) {
    sc_off_time = sc_off_time + time_step;
    scf_of = 1;
    led_notify();
  }
  if (request.indexOf("/fcoff=-") != -1) {
    fc_off_time = fc_off_time - time_step;
    fcf_of = 1;
    led_notify();
  }
  if (request.indexOf("/fcoff=+") != -1) {
    fc_off_time = fc_off_time + time_step;
    fcf_of = 1;
    led_notify();
  }

  if (request.indexOf("/reset") != -1) {
    initial_time = current_time;
    led_notify();
  }



  /*------------------Creating html page---------------------*/

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<style>");
  client.println("table, th, td {");
  client.println("border: 1px solid black;");
  client.println("border-collapse: collapse;");
  client.println("padding-top: 10px;");
  client.println("padding-bottom: 20px;");
  client.println("padding-left: 30px;");
  client.println("padding-right: 40px;");
  client.println("}");
  client.println(".center {");
  client.println("text-align: center;");
  client.println("}");
  client.println("</style>");
  client.println("<title>Bike Charge Controller</title>");
  client.println("<meta http-equiv=\"refresh\" content=\"5;URL=''\">");
  client.println("</head>");
  client.println("<body class=\"center\">");
  client.println("<table style = \"background-color: #F5F5F5;\">");
  client.println("<tr style = \"background-color: #E8E8E8;\">");
  client.println("<th>#</th>");
  client.println("<th colspan=\"3\">Slow Charger</th>");
  client.println("<th colspan=\"3\">Fast Charger</th>");
  client.println("</tr>");
  client.println("<tr>");
  client.println("<td style = \"background-color: #E8E8E8;\">Status</td>");
  if (sc_status == 0) {
    client.println("<td style=\"background-color: orange;\" colspan=\"3\">OFF</td>");
  } else {
    client.println("<td style=\"background-color: greenyellow;\" colspan=\"3\"> ON</td>");
  }
  if (fc_status == 0) {
    client.println("<td style=\"background-color: orange;\" colspan=\"3\">OFF</td>");
  } else {
    client.println("<td style=\"background-color: greenyellow;\" colspan=\"3\"> ON</td>");
  }
  client.println("");
  client.println("</tr>");
  client.println("<tr>");
  client.println("<td style = \"background-color: #E8E8E8;\">ON in </td>");
  if (sc_status == 0) {
    client.println("<td> <a href=\"/scon=-\"><button>-</button></a> </td>");
  } else {
    client.println("<td> <a href=\"/scon=-\"><button disabled>-</button></a> </td>");
  }
  long otime = sc_on_time - elapsed_time;
  String st = String("<td> <time> -:- </time>  </td>");
  if (otime > 0) {
    st = String("<td> <time>") + seconds_to_time(otime) + String("</time>  </td>");
  }
  client.println(st);
  if (sc_status == 0) {
    client.println("<td> <a href=\"/scon=+\"><button>+</button></a> </td>");
  } else {
    client.println("<td> <a href=\"/scon=+\"><button disabled>+</button></a> </td>");
  }
  if (fc_status == 0) {
    client.println("<td> <a href=\"/fcon=-\"><button>-</button></a> </td>");
  } else {
    client.println("<td> <a href=\"/fcon=-\"><button disabled>-</button></a> </td>");
  }

  otime = fc_on_time - elapsed_time;
  st = String("<td> <time> -:- </time>  </td>");
  if (otime > 0) {
    st = String("<td> <time>") + seconds_to_time(otime) + String("</time>  </td>");
  }
  client.println(st);
  if (fc_status == 0) {
    client.println("<td> <a href=\"/fcon=+\"><button>+</button></a> </td>");
  } else {
    client.println("<td> <a href=\"/fcon=+\"><button disabled>+</button></a> </td>");
  }

  client.println("</tr>");
  client.println("<tr>");
  client.println("<td style = \"background-color: #E8E8E8;\">OFF in </td>");
  client.println("<td> <a href=\"/scoff=-\"><button>-</button></a> </td>");
  otime = sc_off_time - elapsed_time;
  st = String("<td> <time> -:- </time>  </td>");
  if (otime > 0) {
    st = String("<td> <time>") + seconds_to_time(otime) + String("</time>  </td>");
  }
  client.println(st);
  client.println("<td> <a href=\"/scoff=+\"><button>+</button></a> </td>");
  client.println("<td> <a href=\"/fcoff=-\"><button>-</button></a> </td>");
  otime = fc_off_time - elapsed_time;
  st = String("<td> <time> -:- </time>  </td>");
  if (otime > 0) {
    st = String("<td> <time>") + seconds_to_time(otime) + String("</time>  </td>");
  }
  client.println(st);
  client.println("<td> <a href=\"/fcoff=+\"><button>+</button></a> </td>");
  client.println("</tr>");
  client.println("</table>");
  client.println("<td> <a href=\"/reset\"><button>reset time</button></a> </td>");
  client.println("</body>");
  client.println("</html>");

}

String seconds_to_time(long elapsed_time) {
  long x, minutes, hours;
  x = elapsed_time / 1000;
  x /= 60;
  minutes = x % 60;
  x /= 60;
  hours = x % 24;
  String time = String(hours) + String(":") + String(minutes);
  return time;
}

void led_notify() {
  digitalWrite(LED, LOW);
  delay(20);
  digitalWrite(LED, HIGH);
}

void slow_charger_on() {
  digitalWrite(SC_AC, LOW);
  delay(2000);
  digitalWrite(SC_DC_P, LOW);
  digitalWrite(SC_DC_N, LOW);
}

void slow_charger_off() {
  digitalWrite(SC_DC_P, HIGH);
  digitalWrite(SC_DC_N, HIGH);
  delay(2000);
  digitalWrite(SC_AC, HIGH);
}

void fast_charger_on() {
  digitalWrite(FC_AC, LOW);
  delay(2000);
  digitalWrite(FC_DC_P, LOW);
  digitalWrite(FC_DC_N, LOW);
}

void fast_charger_off() {
  digitalWrite(FC_DC_P, HIGH);
  digitalWrite(FC_DC_N, HIGH);
  delay(2000);
  digitalWrite(FC_AC, HIGH);
}