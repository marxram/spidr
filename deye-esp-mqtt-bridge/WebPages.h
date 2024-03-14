#ifndef WebPages_h
#define WebPages_h

const char ConfigPage_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>ESP Dashboard</title>
    <style>
       body { font-family: Arial, sans-serif; background-color: #1e1e1e; color: #c5c5c5; margin: 0; padding: 0; display: flex; flex-direction: column; align-items: center; }
        .content, .content-form { padding: 20px; max-width: 800px; width: 100%; box-sizing: border-box; }
        .content-form form { display: flex; flex-direction: column; }
        fieldset { padding: 10px; margin-bottom: 20px; }
        .footer { margin-top: 20px; padding: 20px; background-color: #252526; text-align: center; width: 100%; }
        .footer a { color: #2f848d;; text-decoration: none; }
        label { display: inline-block; width: 180px; margin-bottom: 10px; }
        input[type="text"], input[type="number"] { width: calc(100% - 190px); padding: 5px; }
        input[type="submit"] { width: auto; padding: 10px 20px; margin-top: 10px; }
        .menu {background-color: #333; overflow: hidden; width: 100%; display: flex; justify-content: center; align-items: center;}
        .menu a {display: block; color: white; text-align: center; padding: 14px 20px; text-decoration: none;} 
        .menu a:hover {background-color: #ddd; color: black;} 
        .menu a.active {background-color: #2f848d;; color: white; border-bottom: 2px solid white;}
        .meter{color: #6588F2;}
	pre {
        width: 50%;             /* Width of the <pre> element as a percentage of its container's width */
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
    </style>
</head>
<body>

<div class="content">
    <h2>Inverter Status</h2>
    <div id="sensor-readings">
        <p class="meter">Power: <span id="power">{{power}}</span> W</p>
        <p class="meter">Energy Today: <span id="energyToday">{{energyToday}}</span> kWh</p>
        <p class="meter">Energy Total: <span id="energyTotal">{{energyTotal}}</span> kWh</p>
    </div>
</div>

<div class="menu">
    <a href="/">Home</a>
    <a href="/config" class="active">Config</a>
    <a href="/wiki">Wiki</a>
</div>

<div class="content-form">
    <h2>Welcome to the ESP CONFIG</h2>

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

<div class="footer">
    <a href="https://github.com/marxram/deye-esp-mqtt-bridge" target="_blank">Project GitHub</a>
</div>

</body>
</html>
)rawliteral";



const char HomePage_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>ESP Dashboard</title>
    <style>
       body { font-family: Arial, sans-serif; background-color: #1e1e1e; color: #c5c5c5; margin: 0; padding: 0; display: flex; flex-direction: column; align-items: center; }
        .content, .content-form { padding: 20px; max-width: 800px; width: 100%; box-sizing: border-box; }
        .content-form form { display: flex; flex-direction: column; }
        fieldset { padding: 10px; margin-bottom: 20px; }
        .footer { margin-top: 20px; padding: 20px; background-color: #252526; text-align: center; width: 100%; }
        .footer a { color: #2f848d;; text-decoration: none; }
        label { display: inline-block; width: 180px; margin-bottom: 10px; }
        input[type="text"], input[type="number"] { width: calc(100% - 190px); padding: 5px; }
        input[type="submit"] { width: auto; padding: 10px 20px; margin-top: 10px; }
        .menu {background-color: #333; overflow: hidden; width: 100%; display: flex; justify-content: center; align-items: center;}
        .menu a {display: block; color: white; text-align: center; padding: 14px 20px; text-decoration: none;} 
        .menu a:hover {background-color: #ddd; color: black;} 
        .menu a.active {background-color: #2f848d;; color: white; border-bottom: 2px solid white;}
        .meter{color: #6588F2;}
	pre {
        width: 50%;             /* Width of the <pre> element as a percentage of its container's width */
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
    </style>
</head>
<body>

    <div class="content">
        <h2>Inverter Status</h2>
        <div id="sensor-readings">
            <p class="meter">Power: <span id="power">{{power}}</span> W</p>
            <p class="meter">Energy Today: <span id="energyToday">{{energyToday}}</span> kWh</p>
            <p class="meter">Energy Total: <span id="energyTotal">{{energyTotal}}</span> kWh</p>
        </div>
    </div>
    <div class="menu">
        <a href="/" class="active">Home</a>
        <a href="/config">Config</a>
        <a href="/wiki">Wiki</a>
    </div>
    
    
    <pre id="serialOutput">{{serial_data}}</pre> <!-- This is where the serial data will go -->
    <script>
    function refreshSerialData() {
    location.reload(); // This will reload the entire page
    }

    // Call the function to reload the page every 5 seconds
    setInterval(refreshSerialData, 20000);
</script>
    
    <div class="footer">
        <a href="https://github.com/marxram/deye-esp-mqtt-bridge" target="_blank">Project GitHub</a>
    </div>
</body>
</html>
)rawliteral";


const char WikiPage_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>ESP Dashboard</title>
    <style>
       body { font-family: Arial, sans-serif; background-color: #1e1e1e; color: #c5c5c5; margin: 0; padding: 0; display: flex; flex-direction: column; align-items: center; }
        .content, .content-form { padding: 20px; max-width: 800px; width: 100%; box-sizing: border-box; }
        .content-form form { display: flex; flex-direction: column; }
        fieldset { padding: 10px; margin-bottom: 20px; }
        .footer { margin-top: 20px; padding: 20px; background-color: #252526; text-align: center; width: 100%; }
        .footer a { color: #2f848d;; text-decoration: none; }
        label { display: inline-block; width: 180px; margin-bottom: 10px; }
        input[type="text"], input[type="number"] { width: calc(100% - 190px); padding: 5px; }
        input[type="submit"] { width: auto; padding: 10px 20px; margin-top: 10px; }
        .menu {background-color: #333; overflow: hidden; width: 100%; display: flex; justify-content: center; align-items: center;}
        .menu a {display: block; color: white; text-align: center; padding: 14px 20px; text-decoration: none;} 
        .menu a:hover {background-color: #ddd; color: black;} 
        .menu a.active {background-color: #2f848d;; color: white; border-bottom: 2px solid white;}
        .meter{color: #6588F2;}
    </style>
</head>
<body>

<div class="content">
    <h2>Inverter Status</h2>
    <div id="sensor-readings">
        <p class="meter">Power: <span id="power">{{power}}</span> W</p>
        <p class="meter">Energy Today: <span id="energyToday">{{energyToday}}</span> kWh</p>
        <p class="meter">Energy Total: <span id="energyTotal">{{energyTotal}}</span> kWh</p>
    </div>
</div>


<div class="menu">
    <a href="/">Home</a>
    <a href="/config">Config</a>
   <a href="/wiki" class="active">Wiki</a>
</div>




<div class="footer">
    <a href="https://github.com/marxram/deye-esp-mqtt-bridge" target="_blank">Project GitHub</a>
</div>

<div class="content">
    <h2>Welcome to the WIKI</h2>
</div>

</body>
</html>
)rawliteral";



 
#endif /* WebPages_h */
