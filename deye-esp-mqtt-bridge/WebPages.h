#ifndef WebPages_h
#define WebPages_h


const char MENU_HTML[] PROGMEM = R"rawliteral(
<div class="menu">
    <a href="/">Home</a>
    <a href="/config">Config</a>
    <a href="/configoptions">Config Options</a>
    <a href="/serial">Serial</a>
    <a href="/wiki">Wiki</a>
</div>
)rawliteral";


const char STYLES_HTML[] PROGMEM = R"rawliteral(
    body { font-family: Arial, sans-serif; background-color: #1e1e1e; color: #c5c5c5; margin: 0; padding: 0; display: flex; flex-direction: column; align-items: center; }
        .content, .content-form { padding: 20px; max-width: 1024px; width: 100%; box-sizing: border-box; }
        .content-form form { display: flex; flex-direction: column; }
        fieldset { padding: 10px; margin-bottom: 20px; }
        .footer { margin-top: 20px; padding: 20px; background-color: #252526; text-align: center; width: 100%; }
        .footer a { color: #2f848d;; text-decoration: none; }
        label { display: inline-block; width: 220px; margin-bottom: 10px; }
        input[type="text"], input[type="number"] { width: calc(100% - 240px); padding: 5px; }
        input[type="submit"] { width: auto; padding: 10px 20px; margin-top: 10px; }
        .menu {background-color: #333; overflow: hidden; width: 100%; display: flex; justify-content: center; align-items: center;}
        .menu a {display: block; color: white; text-align: center; padding: 14px 20px; text-decoration: none;} 
        .menu a:hover {background-color: #ddd; color: black;} 
        .menu a.active {background-color: #2f848d;; color: white; border-bottom: 2px solid white;}
        .meter {color: #c5c5c5;}
        .values-container { display: flex; flex-wrap: wrap; justify-content: space-between; }
        .meter-value { flex-basis: 15%; margin-bottom: 10px; }
        .meter-value span { color: #6588F2; font-size: xx-large; }
	pre {
        width: 65%;             /* Width of the <pre> element as a percentage of its container's width */
		height: 50vh; 			/* 50% of the viewport height */
		overflow-y: auto;
		max-height: 20%;      /* Maximum height before scrollbars appear */
        overflow-y: auto;       /* Allows vertical scrolling if content overflows */
        margin: 0 auto;         /* Centers the <pre> element horizontally if its width is less than the container's width */
        background-color: #173e42; /* Background color */
        color: #bdbcb7;         /* Text color */
        font-family: 'Courier New', monospace; /* Font style */
		font-size: smaller;
        white-space: pre-wrap;  /* Preserves white spaces and also wraps text */
        padding: 10px;          /* Inner space between the content and the border of the <pre> element */
        box-sizing: border-box; /* Includes padding and border in the element's total width and height */
    }
)rawliteral";

const char FOOTER_HTML[] PROGMEM = R"rawliteral(
<div class="footer">
    <a href="https://github.com/marxram/deye-esp-mqtt-bridge" target="_blank">Visit E|S|P|I|D|E|R on Github</a>
</div>
)rawliteral";


const char INVERTER_HTML[] PROGMEM = R"rawliteral(
    <h2>Inverter Status</h2>
    <div id="sensor-readings" class="values-container">
        <div class="meter-value">
            <p class="meter"><label>Power</label> <span id="power">{{power}}</span> W</p>
        </div>
        <div class="meter-value">
            <p class="meter"><label>Energy Today</label> <span id="energyToday">{{energyToday}}</span> kWh</p>
        </div>
        <div class="meter-value">
            <p class="meter"><label>Energy Total</label> <span id="energyTotal">{{energyTotal}}</span> kWh</p>
        </div>
    </div>
)rawliteral";


const char ConfigPage_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>{{PAGEHEAD}}</title>
    <style>
       {{STYLES}}
    </style>
</head>
<body>
<h1>{{PAGEHEAD}}</h1>

{{MENU}}

{{INVERTER_STATUS}}


<div class="content-form">
   <h2>{{HEADLINE}}</h2>

    <form action="/update" method="POST">
       <form action="/update" method="POST">
        <fieldset>
            <legend>WiFi Settings</legend>
            <!-- Home WiFi -->
            <div>
                <label for="homeSSID">Home WiFi SSID:</label>
                <input type="text" id="homeSSID" name="homeSSID" value="{{homeSSID}}" required>
            </div>
            <div>
                <label for="homeKey">Home WiFi Password:</label>
                <input type="text" id="homeKey" name="homeKey" value="{{homeKey}}" required>
            </div>
            <!-- Inverter WiFi -->
            <div>
                <label for="inverterSSID">Inverter WiFi SSID:</label>
                <input type="text" id="inverterSSID" name="inverterSSID" value="{{inverterSSID}}" required>
            </div>
            <div>
                <label for="inverterKey">Inverter WiFi Password:</label>
                <input type="text" id="inverterKey" name="inverterKey" value="{{inverterKey}}" required>
            </div>
        </fieldset>
        <fieldset>
            <legend>MQTT Broker Settings</legend>
            <div>
                <label for="mqttBrokerHost">MQTT Broker Host:</label>
                <input type="text" id="mqttBrokerHost" name="mqttBrokerHost" value="{{mqttBrokerHost}}" required>
            </div>
            <div>
                <label for="mqttBrokerPort">MQTT Broker Port:</label>
                <input type="number" id="mqttBrokerPort" name="mqttBrokerPort" value="{{mqttBrokerPort}}" required>
            </div>
            <div>
                <label for="mqttBrokerUser">MQTT Broker User:</label>
                <input type="text" id="mqttBrokerUser" name="mqttBrokerUser" value="{{mqttBrokerUser}}">
            </div>
            <div>
                <label for="mqttBrokerPwd">MQTT Broker Password:</label>
                <input type="text" id="mqttBrokerPwd" name="mqttBrokerPwd" value="{{mqttBrokerPwd}}">
            </div>
            <div>
                <label for="mqttBrokerMainTopic">MQTT Broker Main Topic:</label>
                <input type="text" id="mqttBrokerMainTopic" name="mqttBrokerMainTopic" value="{{mqttBrokerMainTopic}}">
            </div>
        </fieldset>
        <fieldset>
            <legend>Web Access Credentials</legend>
            <!-- Inverter Web Access -->
            <div>
                <label for="inverterWebUser">Inverter Web User:</label>
                <input type="text" id="inverterWebUser" name="inverterWebUser" value="{{inverterWebUser}}" required>
            </div>
            <div>
                <label for="inverterWebPwd">Inverter Web Password:</label>
                <input type="text" id="inverterWebPwd" name="inverterWebPwd" value="{{inverterWebPwd}}" required>
            </div>
        </fieldset>
        <fieldset>
        <legend>Relais Web Access (optional - unused)</legend>
            <!-- Relais Web Access -->
            <div>
                <label for="relaisWebUser">Relais Web User:</label>
                <input type="text" id="relaisWebUser" name="relaisWebUser" value="{{relaisWebUser}}" required>
            </div>
            <div>
                <label for="relaisWebPwd">Relais Web Password:</label>
                <input type="text" id="relaisWebPwd" name="relaisWebPwd" value="{{relaisWebPwd}}" required>
            </div>
            <!-- Relais WiFi -->
            <div>
                <label for="relaisSSID">Relais WiFi SSID:</label>
                <input type="text" id="relaisSSID" name="relaisSSID" value="{{relaisSSID}}" required>
            </div>
            <div>
                <label for="relaisKey">Relais WiFi Password:</label>
                <input type="text" id="relaisKey" name="relaisKey" value="{{relaisKey}}" required>
            </div>
        </fieldset>
        <div>
            <input type="submit" value="Update Settings">
        </div>
    </form>

</div>

{{FOOTER}}

</body>
</html>
)rawliteral";

const char ConfigPageOptions_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>{{PAGEHEAD}}</title>
    <style>
       {{STYLES}}
    </style>
</head>
<body>

<h1>{{PAGEHEAD}}</h1>

{{MENU}}

{{INVERTER_STATUS}}
    
<div class="content-form">
    <h2>{{HEADLINE}}</h2>

        <form action="/updateoptions" method="POST">
        <fieldset>
            <legend>NTP Configuration</legend>
            <div>
                <label for="ntpA">NTP Server A:</label>
                <input type="text" id="ntpA" name="ntpA" value="{{ntpA}}">
            </div>
            <div>
                <label for="ntpB">NTP Server B:</label>
                <input type="text" id="ntpB" name="ntpB"  value="{{ntpB}}">
            </div>
            <div>
                <label for="ntpActive">NTP Active:</label>
                <input type="checkbox" id="ntpActive" name="ntpActive" value="{{ntpActive}}">
            </div>
            <div>
                <label for="ntpGmtOff">GMT Offset:</label>
                <input type="number" id="ntpGmtOff" name="ntpGmtOff" value="{{ntpGmtOff}}">
            </div>
            <div>
                <label for="ntpDstOff">DST Offset:</label>
                <input type="number" id="ntpDstOff" name="ntpDstOff" value="{{ntpDstOff}}">
            </div>
        </fieldset>

        <fieldset>
            <legend>Timing Behavior</legend>
            <div>
                <label for="homeNetMS">Home Network Stay (ms):</label>
                <input type="number" id="homeNetMS" name="homeNetMS" required value="{{homeNetMS}}">
            </div>
            <div>
                <label for="homeFirstBootMS">First Boot Home Stay (ms):</label>
                <input type="number" id="homeFirstBootMS" name="homeFirstBootMS" required value="{{homeFirstBootMS}}">
            </div>
            
            <div>
                <label for="wifiWaitS">WiFi Connect Wait (s):</label>
                <input type="number" id="wifiWaitS" name="wifiWaitS" required value="{{wifiWaitS}}">
            </div>
            <div>
                <label for="invOffTimeoutS">Inverter Offline Timeout (s):</label>
                <input type="number" id="invOffTimeoutS" name="invOffTimeoutS" required value="{{invOffTimeoutS}}">
            </div>
        </fieldset>

        <fieldset>
            <legend>Create your own - WiFi Access Point</legend>
            <div>
                <label for="apModeMS">AP Mode Duration (ms):</label>
                <input type="number" id="apModeMS" name="apModeMS" required value="{{apModeMS}}"> 
            </div>
            <div>
            <label for="apSSID">Accesspoint SSID</label>
                <input type="text" id="apSSID" name="apSSID" value="{{apSSID}}">
            </div>
            <div>
            <label for="apKey">Accesspoint Passphrase</label>
                <input type="text" id="apKey" name="apKey" value="{{apKey}}">
            </div>
        </fieldset>
        <div>
            <input type="submit" value="Save Configuration">
        </div>
    </form>

</div>

{{FOOTER}}

</body>
</html>
)rawliteral";




const char HomePage_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>{{PAGEHEAD}}</title>
    <style>
       {{STYLES}}
    </style>
</head>
<body>
<h1>{{PAGEHEAD}}</h1>

{{MENU}}

{{INVERTER_STATUS}}
    
<div class="content">
    <h2>{{HEADLINE}}</h2>
</div>

<div class="content-form">
    <h2>Solar Inverter Data</h2>

    <fieldset>
        <legend>Inverter Details</legend>
        <div>
            <label>Serial Number:</label>
            <span>{{webdata_sn}}</span>
        </div>
        <!-- Unused Values
        <div>
            <label>Main Software Version:</label>
            <span>{{webdata_msvn}}</span>
        </div>
        <div>
            <label>Secondary Software Version:</label>
            <span>{{webdata_ssvn}}</span>
        </div>
        <div>
            <label>PV Type:</label>
            <span>{{webdata_pv_type}}</span>
        </div>
        <div>
            <label>Rate Power:</label>
            <span>{{webdata_rate_p}}</span>
        </div>
        -->
        <div>
            <label>Current Power:</label>
            <span>{{webdata_now_p}} kW</span>
        </div>
        <div>
            <label>Energy Today:</label>
            <span>{{webdata_today_e}} kWh</span>
        </div>
        <div>
            <label>Total Energy:</label>
            <span>{{webdata_total_e}} kWh</span>
        </div>
        <!-- Unused Values
        <div>
            <label>Alarm Status:</label>
            <span>{{webdata_alarm}}</span>
        </div>
        -->
        <div>
            <label>Last Update Time:</label>
            <span>{{webdata_utime}}</span>
        </div>
    </fieldset>

    <fieldset>
        <legend>WiFi Module Information</legend>
        <div>
            <label>Module ID</label>
            <span>{{cover_mid}}</span>
        </div>
        <div>
            <label>Module Software Version</label>
            <span>{{cover_ver}}</span>
        </div>
        <div>
            <label>Current Mode:</label>
            <span>{{cover_wmode}}</span>
        </div>
        <div>
            <label>AP SSID:</label>
            <span>{{cover_ap_ssid}}</span>
        </div>
        <div>
            <label>AP IP:</label>
            <span>{{cover_ap_ip}}</span>
        </div>
        <div>
            <label>AP MAC:</label>
            <span>{{cover_ap_mac}}</span>
        </div>
        <div>
            <label>STA SSID:</label>
            <span>{{cover_sta_ssid}}</span>
        </div>
        <div>
            <label>STA RSSI:</label>
            <span>{{cover_sta_rssi}}</span>
        </div>
        <div>
            <label>STA IP:</label>
            <span>{{cover_sta_ip}}</span>
        </div>
        <div>
            <label>STA MAC:</label>
            <span>{{cover_sta_mac}}</span>
        </div>
    </fieldset>

    <fieldset>
        <legend>Remote Server Connectivity</legend>
        <div>
            <label>Status A:</label>
            <span>{{status_a}}</span>
        </div>
        <div>
            <label>Status B:</label>
            <span>{{status_b}}</span>
        </div>
        <!--
        <div>
            <label>Status C:</label>
            <span>{{status_c}}</span>
        </div>
        -->
    </fieldset>
</div>

    
{{FOOTER}}
</body>
</html>
)rawliteral";


const char Serial_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>{{PAGEHEAD}}</title>
    <style>
       {{STYLES}}
    </style>
</head>
<body>
<h1>{{PAGEHEAD}}</h1>

{{MENU}}

{{INVERTER_STATUS}}

<div class="content">
    <h2>{{HEADLINE}}</h2>
</div>
    
    <pre id="serialOutput">{{serial_data}}</pre> <!-- This is where the serial data will go -->
    <script>
    function refreshSerialData() {
    location.reload(); // This will reload the entire page
    }

    // Call the function to reload the page every 5 seconds
    setInterval(refreshSerialData, 30000);
</script>


{{FOOTER}}
</body>
</html>
)rawliteral";




const char WikiPage_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>{{PAGEHEAD}}</title>
    <style>
       {{STYLES}}
    </style>
</head>
<body>
<h1>{{PAGEHEAD}}</h1>

{{MENU}}

{{INVERTER_STATUS}}

<div class="content">
    <h2>{{HEADLINE}}</h2>
</div>

{{FOOTER}}

</body>
</html>
)rawliteral";



 
#endif /* WebPages_h */
