#ifndef WebPages_h
#define WebPages_h

// Modularisierte Inhalte der Webseiten. Hier das MENU
const char MENU_HTML[] PROGMEM = R"rawliteral(
<div class="menu">
 // [INHALTE DES MENUS HIER]
</div>
)rawliteral";

// Die Styles sind global und können zentral gemanaged werden. 
// Sie müssen nachher in alle HTML Seiten eingebunden werden.
const char STYLES_HTML[] PROGMEM = R"rawliteral(
 // [Alle STYLE INFORMATIONEN HIER]
)rawliteral";

// Der Footer ist ebenfalls global und kann zentral gemanaged werden.
const char FOOTER_HTML[] PROGMEM = R"rawliteral(
<div class="footer">
    <a href="https://github.com/marxram/spidr" target="_blank">Visit S|P|I|D|R on Github</a>
</div>
)rawliteral";

// Zeile um die Inverter Daten anzuzeigen 
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

// Gekürztes Beispiel der Config Page
const char ConfigPage_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en"><head><meta charset="UTF-8">
    <title>{{PAGEHEAD}}</title>
    <style>{{STYLES}}</style>
</head>
<body> 
<h1>{{PAGEHEAD}}</h1>
{{MENU}}
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
            // Alle weiteren Felder hier
        </fieldset> 
        <div> <input type="submit" value="Update Settings"> </div>
    </form>
</div>
{{FOOTER}}
</body>
</html>
)rawliteral";