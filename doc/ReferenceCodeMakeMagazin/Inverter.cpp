#include "Arduino.h"
#include "Inverter.h"
#include "SerialCaptureLines.h"


void Inverter::printVariables() const {
    serialCapture.println("Inverter Variables:");
    serialCapture.print("Webdata SN: "); serialCapture.println(webdata_sn);
    //...
    serialCapture.print("Webdata Rate P: "); serialCapture.println(webdata_rate_p);
    //..
    serialCapture.print("Last Update Timestamp: "); serialCapture.println(lastUpdateTimestamp);
}

ParseStatus Inverter::updateData(const String& html) {
    serialCapture.println("Start updating data");
    ParseStatus result = extractVariables(html);
    return result;
}


// Beispiel getterfunktion um von außen auf die Werte zuzugreifen
float Inverter::getInverterPowerNow_W() const {
    return webdata_now_p;
}

ParseStatus Inverter::updateData(const String& html) {
    // Die updateData Funktionübergibt die Werte aus dem HTML an die Funktion extractVariables
    // dort werden die Daten aus dem HTML extrahiert und in die Klassenvariablen gespeichert
    ParseStatus result = extractVariables(html);
    inverterActive = true;
    return result;
}




// Auf der HTML Seite sind die Werte in diesem Fall serhr leicht auszuelesen:
/* Auszug ...
<script type="text/javascript">
var webdata_sn = "2200000000                             ";
...
var webdata_now_p = "550";
var webdata_today_e = "2.0";
var webdata_total_e = "308.1";
...
var cover_mid = "4100000000";
var cover_ver = "MW3_16U_5406_2.27";
...
*/


// Die Funktion extractVariables analysiert das HTML und speichert extrahierte Daten in Klassenvariablen.
// Es werden sowohl String- als auch Float-Werte extrahiert.
ParseStatus Inverter::extractVariables(const String& html) {
    if (html.isEmpty()) {
        serialCapture.println("HTML is empty"); // Überprüft, ob das HTML-Dokument leer ist
        return PARSE_HTML_ISSUES; // Rückgabe eines Fehlers, wenn HTML leer ist
    }
    int countParseErrors = 0; // Zähler für Parse-Fehler
    int countParseSuccess = 0; // Zähler für erfolgreiche Parsings

    // Extrahiert Float-Werte aus dem HTML basierend auf spezifischen Markern
    float temp_webdata_now_p = extractFloatValue(html, "webdata_now_p =");
    float temp_webdata_today_e = extractFloatValue(html, "webdata_today_e =");
    float temp_webdata_total_e = extractFloatValue(html, "webdata_total_e =");

    // Überprüft die Plausibilität der Gesamtenergie; wenn 0 oder kleiner, liegt ein Parsing-Problem vor
    if (temp_webdata_total_e <= 0.0) {
        serialCapture.println("webdata_total_e is <= 0.0, indicating parsing issues");
        return PARSE_PARSING_ISSUES;    
    } 
    
    // Extrahiert und validiert String-Variablen aus dem HTML
    String temp_webdata_sn = extractAndValidateString(html, "webdata_sn =", countParseSuccess );
    // Weitere String-Variablen können hier ähnlich extrahiert werden
    String temp_status_c = extractAndValidateString(html, "status_c =", countParseSuccess);

    // Wenn keine Parse-Fehler auftraten, werden die temporären Werte in die Klassenvariablen übernommen
    if (countParseErrors == 0) {
        serialCapture.println("Updating class member variables with extracted values...");
        webdata_now_p = temp_webdata_now_p;
        // Weitere Zuweisungen zu Klassenvariablen hier...
        status_c = temp_status_c; // ...

        lastSuccessfullTimestamp = millis(); // Aktualisiert den Zeitstempel des letzten erfolgreichen Lesens
        lastUpdateTimestamp = lastSuccessfullTimestamp;
        lastReadSuccess = true; // Markiert das Lesen als erfolgreich
        return PARSE_OK;
    } else {
        // Bei Parse-Fehlern werden Fehlerinformationen aktualisiert, aber keine Daten übernommen
        lastUpdateTimestamp = millis();
        lastReadSuccess = false;
        return PARSE_PARSING_ISSUES;
    }
}


// Generische Funktion, die nach der richtigen Stelle im HTML sucht. 
String Inverter::extractValue(const String& html, const String& variableName) const {
    // Es wird geprüft wo die Markierung im HTML ist
    int startIndex = html.indexOf(variableName);
    if (startIndex != -1) {
        startIndex += variableName.length();
        int endIndex = html.indexOf(";", startIndex);
        if (endIndex != -1) {
            String value = html.substring(startIndex, endIndex);
            value.replace("\"", ""); 
            value.trim();
            return value;
        }
    }
    return "";
}

// Der zurückgegebenen Wert wird in einen Float umgewandelt
float Inverter::extractFloatValue(const String& html, const String& variableName) const {
    String valueStr = extractValue(html, variableName);
    if (valueStr.length() > 0) {
        return valueStr.toFloat();
    }
    return defaultValue;
}

// Der zurückgegebenen Wert wird in einen String umgewandelt und geprüft ob er nicht leer ist 
String Inverter::extractAndValidateString(const String& html, const String& key, int& countParseSuccess) {
    String value = extractValue(html, key);
    if (!value.isEmpty()) {
        countParseSuccess++;
    }
    return value;
}


