#ifndef WebPages_h
#define WebPages_h

const char Preferences_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>ESP Configuration</title>
</head>
<body>
    <h2>ESP Configuration</h2>
    <form action="/update" method="POST">
        <div>
            <label for="homeSSID">Home WiFi SSID:</label>
            <input type="text" id="homeSSID" name="homeSSID" value="{{homeSSID}}" required>
        </div>
        <div>
            <label for="homeKey">Home WiFi Password:</label>
            <input type="password" id="homeKey" name="homeKey" value="{{homeKey}}" required>
        </div>
        <!-- Repeat for other settings -->
        <div>
            <input type="submit" value="Update Settings">
        </div>
    </form>
</body>
</html>
)rawliteral";

// You can define more pages here in the same manner
// const char AnotherPage_HTML[] PROGMEM = R"rawliteral( ... )rawliteral";

#endif /* WebPages_h */
