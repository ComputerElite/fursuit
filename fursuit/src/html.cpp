#include "html.h"
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    button {
        background-color: #195B6A;
        border: none;
        color: white;
        text-decoration: none;
        margin: 2px;
        padding: 10px;
        cursor: pointer;
    }
    .enabled {
        background-color: #4CAF50;
    }
    .grid {
        display: grid;
        grid-template-columns: repeat(3, 1fr);
        grid-gap: 10px;
    }

  </style>
</head>
<body>
  <h2>ESP32 for fursuit</h2>
  <h3>Primary animation</h3>
  <div class="grid" id="primary">

  </div>
  <h3>Secondary animation</h3>
  <div class="grid" id="secondary">

  </div>
  <h3>Other config options</h3>
  <div id="otherConfig"></div>
  <h3>WiFi</h3>
  <label>SSID<input type="text" id="ssid"></label><br>
  <label>Password<input type="password" id="password"></label><br>
  <label>AP SSID<input type="text" id="setupssid"></label><br>
  <label>AP Password<input type="password" id="setuppassword"></label><br>
  <i id="wifiStatus"></i><br>
  <button onclick="sendWiFi()">Send WiFi</button>
  <h3>Danger!!!</h3>
  <button onclick="Reset()">Reset</button>
  
<script>
    const ssid = document.getElementById("ssid");
    const password = document.getElementById("password");

    function Lerp(a, b, t) {
        return a + (b - a) * t;
    }

    function Reset() {
        if(!confirm("Are you sure you want to reset the suit config?")) return;
        fetch(ip + "/reset")
    }

    function Init(urlStart) {
        ip = urlStart;
        UpdateLEDPatterns();
        GetConfig();
        UpdateWifi();
    }

    function UpdateUI() {
        UpdateLEDPatternUI();
        GenerateUI();
    }

    function SetProperty(name, value) {
        config[name] = value
        Send();
        UpdateUI();
    }
    
    function GenerateUI() {
        var html = ""
        for(const [key, value] of Object.entries(config)) {
            if(["primary", "secondary"].includes(key)) continue;
            html += `
                <div>
                    <label for="${key}">${key}</label>
                    ${typeof value === "number" ? 
                `<input onchange="SetProperty('${key}', parseFloat(this.value))" type="number" id="${key}" value="${value}">` 
                : `<input onchange="SetProperty('${key}', ${!value})" type="checkbox" id="${key}" ${value ? `checked` : ``}>`}
                    
                </div>
            `
        }
        document.getElementById("otherConfig").innerHTML = html
    }

    Init('')

    var ledPatterns = [];

    function UpdateLEDPatterns() {
        fetch(ip + "/ledpatterns").then((response) => {
            return response.json();
        }).then((leds) => {
            ledPatterns = leds
            UpdateLEDPatternUI();
        })
    }

    function UpdateLEDPatternUI() {
        var primaryHtml = ``;
            var secondaryHtml = ``;
            ledPatterns.forEach((animation) => {
                primaryHtml += `<button class="${animation.id == config.primary ? "enabled" : ""}" onclick="SetPrimary(${animation.id})">${animation.name}</button>`
                secondaryHtml += `<button class="${animation.id == config.secondary ? "enabled" : ""}" onclick="SetSecondary(${animation.id})">${animation.name}</button>`
            })
            document.getElementById("primary").innerHTML = primaryHtml;
            document.getElementById("secondary").innerHTML = secondaryHtml;
    }

    function SetPrimary(id) {
        config.primary = id;
        Send();
        UpdateLEDPatternUI();
    }

    function SetSecondary(id) {
        config.secondary = id;
        Send();
        UpdateLEDPatternUI();
    }


    var config = {}
    function GetConfig() {
        fetch(ip + "/config").then((response) => {
            return response.json();
        }).then((data) => {
            config = data;
            UpdateUI();
            console.log(data)
        })
    }
    function Send() {
        fetch(ip + "/config", {
            method: "POST",
            body: JSON.stringify(config)
        })
    }

    function UpdateWifi() {
        fetch(ip + "/wifi")
            .then((response) => {
                return response.json();
            })
            .then((data) => {
                ssid.value = data.ssid;
                password.value = data.password;
                document.getElementById("setupssid").value = data.setupSSID;
                document.getElementById("setuppassword").value = data.setupPassword;
                document.getElementById("wifiStatus").innerText = data.status;
            })
    
    }

    function sendWiFi() {
        if(document.getElementById("setuppassword").value.length < 8) return alert("Password must be at least 8 characters")
        fetch(ip + `/wifi`, {
            method: "POST",
            body: JSON.stringify({
                ssid: ssid.value,
                password: password.value,
                setupSSID: document.getElementById("setupssid").value,
                setupPassword: document.getElementById("setuppassword").value
            })
        })
        alert("WiFi sent. ESP will try to connect to it. Keep your eye out for a new network device. If it fails the setup network will open again.")
    }

    function RGBtoHSV(rgb) {
            var r = (rgb >> 16) & 255;
            var g = (rgb >> 8) & 255;
            var b = rgb & 255;

            r = r / 255; // Normalize r to the range of 0-1
            g = g / 255; // Normalize g to the range of 0-1
            b = b / 255; // Normalize b to the range of 0-1

            var max = Math.max(r, g, b);
            var min = Math.min(r, g, b);
            var delta = max - min;

            var h, s, v;

            // Calculate hue (h)
            if (delta === 0) {
                h = 0; // No difference, so hue is undefined (can be set to any value)
            } else if (max === r) {
                h = ((g - b) / delta) % 6;
            } else if (max === g) {
                h = (b - r) / delta + 2;
            } else {
                h = (r - g) / delta + 4;
            }

            h = (h * 60 + 360) % 360; // Convert h to the range of 0-360

            // Calculate saturation (s)
            s = max === 0 ? 0 : delta / max;

            // Calculate value (v)
            v = max;

            // Convert saturation and value to the range of 0-100
            s = Math.round(s * 100);
            v = Math.round(v * 100);

            return { h: h, s: s, v: v };
        }


        function HSVtoRGB(h, s, v) {
            var r, g, b;

            h = h / 360; // Normalize h to the range of 0-1
            s = s / 100; // Normalize s to the range of 0-1
            v = v / 100; // Normalize v to the range of 0-1

            var i = Math.floor(h * 6);
            var f = h * 6 - i;
            var p = v * (1 - s);
            var q = v * (1 - f * s);
            var t = v * (1 - (1 - f) * s);

            switch (i % 6) {
                case 0:
                    r = v;
                    g = t;
                    b = p;
                    break;
                case 1:
                    r = q;
                    g = v;
                    b = p;
                    break;
                case 2:
                    r = p;
                    g = v;
                    b = t;
                    break;
                case 3:
                    r = p;
                    g = q;
                    b = v;
                    break;
                case 4:
                    r = t;
                    g = p;
                    b = v;
                    break;
                case 5:
                    r = v;
                    g = p;
                    b = q;
                    break;
            }

            // Convert the values to the range of 0-255
            r = Math.round(r * 255);
            g = Math.round(g * 255);
            b = Math.round(b * 255);

            return { r: r, g: g, b: b };
        }
</script>
</body>

</html>
    )rawliteral";