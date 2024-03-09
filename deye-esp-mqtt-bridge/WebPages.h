#ifndef WebPages_h
#define WebPages_h

const char ConfigPage_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>ESP Dashboard</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #1e1e1e; color: #c5c5c5; margin: 0; padding: 0; }
        .menu { background-color: #333; overflow: hidden; }
        .menu a { float: left; display: block; color: white; text-align: center; padding: 14px 20px; text-decoration: none; }
        .menu a:hover { background-color: #ddd; color: black; }
        .content { padding: 20px; }
        .footer { margin-top: 20px; padding: 20px; background-color: #252526; text-align: center; }
        .footer a { color: #569cd6; text-decoration: none; }
    </style>
</head>
<body>

<div class="menu">
    <a href="/">Home</a>
    <a href="/config">Config</a>
    <a href="/wiki">Wiki</a>
</div>

<div class="content">
    <h2>Welcome to the ESP CONFIG</h2>

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
        body { font-family: Arial, sans-serif; background-color: #1e1e1e; color: #c5c5c5; margin: 0; padding: 0; }
        .menu { background-color: #333; overflow: hidden; }
        .menu a { float: left; display: block; color: white; text-align: center; padding: 14px 20px; text-decoration: none; }
        .menu a:hover { background-color: #ddd; color: black; }
        .content { padding: 20px; }
        .footer { margin-top: 20px; padding: 20px; background-color: #252526; text-align: center; }
        .footer a { color: #569cd6; text-decoration: none; }
    </style>
</head>
<body>

<div class="menu">
    <a href="/">Home</a>
    <a href="/config">Config</a>
    <a href="/wiki">Wiki</a>
</div>

<div class="footer">
    <a href="https://github.com/marxram/deye-esp-mqtt-bridge" target="_blank">Project GitHub</a>
</div>

<div class="content">
    <h2>Welcome HOME</h2>
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
        body { font-family: Arial, sans-serif; background-color: #1e1e1e; color: #c5c5c5; margin: 0; padding: 0; }
        .menu { background-color: #333; overflow: hidden; }
        .menu a { float: left; display: block; color: white; text-align: center; padding: 14px 20px; text-decoration: none; }
        .menu a:hover { background-color: #ddd; color: black; }
        .content { padding: 20px; }
        .footer { margin-top: 20px; padding: 20px; background-color: #252526; text-align: center; }
        .footer a { color: #569cd6; text-decoration: none; }
    </style>
</head>
<body>

<div class="menu">
    <a href="/">Home</a>
    <a href="/config">Config</a>
    <a href="/wiki">Wiki</a>
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
