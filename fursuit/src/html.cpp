#include "html.h"
const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE HTML>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        html {
            font-family: Arial;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        }

        h2 {
            font-size: 3.0rem;
        }

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
    <link rel="stylesheet" href="/wheel.css">
</head>

<body>
    <h2>ESP32 for fursuit</h2>
    <h3>Primary animation</h3>
    <div class="grid" id="primary">

    </div>
    <h3>Secondary animation</h3>
    <div class="grid" id="secondary">

    </div>
    <div style="display: flex; flex-wrap: nowrap;">
        <div>

            <h3>Color 0</h3>
            <div id="color0">

            </div>
        </div>
        <div>

            <h3>Color 1</h3>
            <div id="color1">

            </div>
        </div>
    </div>
    <div style="display: flex; flex-wrap: nowrap;">
        <div>

            <h3>Left Ear mode</h3>
            <div id="learmode">

            </div>
        </div>
        <div>

            <h3>Right ear mode</h3>
            <div id="rearmode">

            </div>
        </div>
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
            if (!confirm("Are you sure you want to reset the suit config?")) return;
            fetch(ip + "/reset")
        }

        function Init(urlStart) {
            ip = urlStart;
            UpdateLEDPatterns();
            UpdateEarModes();
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
            for (const [key, value] of Object.entries(config)) {
                if (["primary", "secondary"].includes(key)) continue;
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
        var earModes = [];

        function UpdateLEDPatterns() {
            fetch(ip + "/ledpatterns").then((response) => {
                return response.json();
            }).then((leds) => {
                ledPatterns = leds
                UpdateLEDPatternUI();
            })
        }

        function UpdateEarModes() {
            fetch(ip + "/earmodes").then((response) => {
                return response.json();
            }).then((ears) => {
                earModes = ears
                UpdateEarModeUI();
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

        function UpdateEarModeUI() {
            var leftHtml = ``;
            var rightHtml = ``;
            earModes.forEach((animation) => {
                leftHtml += `<button class="${animation.id == config.leftEarMode ? "enabled" : ""}" onclick="SetLeftEarMode(${animation.id})">${animation.name}</button>`
                rightHtml += `<button class="${animation.id == config.rightEarMode ? "enabled" : ""}" onclick="SetRightEarMode(${animation.id})">${animation.name}</button>`
            })
            document.getElementById("rearmode").innerHTML = rightHtml;
            document.getElementById("learmode").innerHTML = leftHtml;
        }

        function SetLeftEarMode(id) {
            config.leftEarMode = id;
            Send();
            UpdateEarModeUI();
        }

        function SetRightEarMode(id) {
            config.rightEarMode = id;
            Send();
            UpdateEarModeUI();
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
        lastSend = 0

        function Send() {
            if (Date.now() - lastSend < 50) return; // make sure to not spam the esp with web requests
            lastSend = Date.now();
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
            if (document.getElementById("setuppassword").value.length < 8) return alert("Password must be at least 8 characters")
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

        function SetColor0(color, update = true) {
            console.log(color);
            console.log(HSVtoRGB(color.hsv[0], color.hsv[1], color.hsv[2]));
            var c = HSVtoRGB(color.hsv[0], color.hsv[1], color.hsv[2]);
            var hex = ((c.r << 16) + (c.g << 8) + c.b).toString(16)
            if (update) {
                config.color0 = parseInt(hex, 16);
                Send();
            }
        }

        function SetColor1(color, update = true) {
            console.log(color);
            console.log(HSVtoRGB(color.hsv[0], color.hsv[1], color.hsv[2]));
            var c = HSVtoRGB(color.hsv[0], color.hsv[1], color.hsv[2]);
            var hex = ((c.r << 16) + (c.g << 8) + c.b).toString(16)
            if (update) {
                config.color1 = parseInt(hex, 16);
                Send();
            }
        }

        function AddWheel() {
            picker0 = new ReinventedColorWheel({
                appendTo: document.getElementById('color0'),
                wheelDiameter: 250,
                wheelThickness: 40,
                handleDiameter: 35,
                onChange: SetColor0
            })
            picker1 = new ReinventedColorWheel({
                appendTo: document.getElementById('color1'),
                wheelDiameter: 250,
                wheelThickness: 40,
                handleDiameter: 35,
                onChange: SetColor1
            })
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
    <script src="/wheel.js" onload="AddWheel()"></script>
</body>

</html>
    )rawliteral";

const char wheelCss[] PROGMEM = R"rawliteral(
.reinvented-color-wheel,.reinvented-color-wheel--hue-handle,.reinvented-color-wheel--hue-wheel,.reinvented-color-wheel--sv-handle,.reinvented-color-wheel--sv-space{touch-action:manipulation;touch-action:none;-webkit-touch-callout:none;-webkit-tap-highlight-color:transparent;-webkit-user-select:none;-moz-user-select:none;-ms-user-select:none;user-select:none}.reinvented-color-wheel{position:relative;display:inline-block;line-height:0;border-radius:50%}.reinvented-color-wheel--hue-wheel{border-radius:50%}.reinvented-color-wheel--sv-space{position:absolute;left:0;top:0;right:0;bottom:0;margin:auto}.reinvented-color-wheel--hue-handle,.reinvented-color-wheel--sv-handle{position:absolute;box-sizing:border-box;pointer-events:none;border-radius:50%;border:2px solid #fff;box-shadow:inset 0 0 0 1px #000}
)rawliteral";

const char wheelJs[] PROGMEM = R"rawliteral(
var ReinventedColorWheel=function(){"use strict"
var e=function(e){var t=e[0],n=e[1]/100,h=e[2]/100
return 0===h?[0,0,0]:[t,2*(n*=(h*=2)<=1?h:2-h)/(h+n)*100,(h+n)/2*100]}
var t,n,h,i=function(e){var t,n,h=e[0],i=e[1]/100,s=e[2]/100
return t=i*s,[h,100*(t=(t/=(n=(2-i)*s)<=1?n:2-n)||0),100*(n/=2)]},s="PointerEvent"in window
!s&&"ontouchend"in window?(t=function(e,t){return e.addEventListener("touchstart",function(n){(h=1===n.touches.length&&e)&&(n.preventDefault(),t(n.targetTouches[0]))})},n=function(e,t){return e.addEventListener("touchmove",function(n){h===e&&(n.preventDefault(),t(n.targetTouches[0]))})}):(t=function(e,t){return e.addEventListener(s?"pointerdown":"mousedown",function(n){0===n.button&&(h=e,t(n))})},n=function(e,t){return addEventListener(s?"pointermove":"mousemove",function(n){h===e&&t(n)})},addEventListener(s?"pointerup":"mouseup",function(){h=void 0}))
var o={hsv:[0,100,100],hsl:[0,100,50],wheelDiameter:200,wheelThickness:20,handleDiameter:16,wheelReflectsSaturation:!0,onChange:function(){}}
function l(e,t){return e?[r(e[0])?(n=e[0],h=360,i=(n+.5|0)%h,i<0?i+h:i):t[0],r(e[1])?a(e[1]):t[1],r(e[2])?a(e[2]):t[2]]:t
var n,h,i}function a(e){return e<0?0:e>100?100:e}function r(e){return"number"==typeof e&&isFinite(e)}function d(e,t){var n=document.createElement(e)
return n.className=t,n}return function(){function s(e){var i=this
this.options=e,this.wheelDiameter=this._option("wheelDiameter"),this.wheelThickness=this._option("wheelThickness"),this.handleDiameter=this._option("handleDiameter"),this.onChange=this._option("onChange"),this.wheelReflectsSaturation=this._option("wheelReflectsSaturation"),this.rootElement=this.options.appendTo.appendChild(d("div","reinvented-color-wheel")),this.hueWheelElement=this.rootElement.appendChild(d("canvas","reinvented-color-wheel--hue-wheel")),this.hueWheelContext=this.hueWheelElement.getContext("2d"),this.hueHandleElement=this.rootElement.appendChild(d("div","reinvented-color-wheel--hue-handle")),this.svSpaceElement=this.rootElement.appendChild(d("canvas","reinvented-color-wheel--sv-space")),this.svSpaceContext=this.svSpaceElement.getContext("2d"),this.svHandleElement=this.rootElement.appendChild(d("div","reinvented-color-wheel--sv-handle")),this._redrawHueWheel=function(){i._redrawHueWheelRequested=!1
var e=i.wheelDiameter,t=e/2,n=t-i.wheelThickness/2,h=Math.PI/180,s=i.wheelReflectsSaturation?","+i.hsl[1]+"%,"+i.hsl[2]+"%)":",100%,50%)",o=i.hueWheelContext
o.clearRect(0,0,e,e),o.lineWidth=i.wheelThickness
for(var l=0;l<360;l++)o.beginPath(),o.arc(t,t,n,(l-90.7)*h,(l-89.3)*h),o.strokeStyle="hsl("+l+s,o.stroke()},this._onMoveHueHandle=function(e){var t=i.hueWheelElement.getBoundingClientRect(),n=i.wheelDiameter/2,h=e.clientX-t.left-n,s=e.clientY-t.top-n,o=Math.atan2(s,h)
i.setHSV(180*o/Math.PI+90)},this._onMoveSvHandle=function(e){var t=i.svSpaceElement.getBoundingClientRect(),n=100*(e.clientX-t.left)/t.width,h=100*(t.bottom-e.clientY)/t.height
i.setHSV(i.hsv[0],n,h)},this.hueWheelContext.imageSmoothingEnabled=!1,this.svSpaceContext.imageSmoothingEnabled=!1,!e.hsv&&e.hsl?this.hsv=s.hsl2hsv(this.hsl=l(e.hsl,o.hsl)):this.hsl=s.hsv2hsl(this.hsv=l(e.hsv,o.hsv)),t(this.hueWheelElement,function(e){var t=i.hueWheelElement.getBoundingClientRect()
i.hueWheelContext.getImageData(e.clientX-t.left,e.clientY-t.top,1,1).data[3]?i._onMoveHueHandle(e):h=void 0}),n(this.hueWheelElement,this._onMoveHueHandle),t(this.svSpaceElement,this._onMoveSvHandle),n(this.svSpaceElement,this._onMoveSvHandle),this.redraw()}return s.prototype.setHSV=function(){var e=this.hsv,t=this.hsv=l(arguments,e),n=e[0]-t[0],h=e[1]-t[1]||e[2]-t[2]
n&&(this.hsl[0]=this.hsv[0],this._redrawHueHandle(),this._updateSvBackground()),h&&(this.hsl=s.hsv2hsl(t),this._redrawSvHandle(),this.wheelReflectsSaturation&&!this._redrawHueWheelRequested&&(requestAnimationFrame(this._redrawHueWheel),this._redrawHueWheelRequested=!0)),(n||h)&&this.onChange(this)},s.prototype.setHSL=function(){this.setHSV.apply(this,s.hsl2hsv(l(arguments,this.hsl)))},s.prototype.redraw=function(){this.hueWheelElement.width=this.hueWheelElement.height=this.wheelDiameter,this.svSpaceElement.width=this.svSpaceElement.height=(this.wheelDiameter-2*this.wheelThickness)*Math.sqrt(2)/2
var e=this.hueHandleElement.style,t=this.svHandleElement.style
e.width=e.height=t.width=t.height=this.handleDiameter+"px",e.marginLeft=e.marginTop=t.marginLeft=t.marginTop=-this.handleDiameter/2+"px",this._redrawHueWheel(),this._redrawHueHandle(),this._redrawSvSpace(),this._redrawSvHandle()},s.prototype._redrawSvSpace=function(){this._updateSvBackground()
var e=this.svSpaceElement.width,t=this.svSpaceContext,n=t.createLinearGradient(0,0,e,0),h=t.createLinearGradient(0,0,0,e)
n.addColorStop(0,"rgba(255,255,255,1)"),n.addColorStop(1,"rgba(255,255,255,0)"),h.addColorStop(0,"rgba(0,0,0,0)"),h.addColorStop(1,"rgba(0,0,0,1)"),t.fillStyle=n,t.fillRect(0,0,e,e),t.fillStyle=h,t.fillRect(0,0,e,e)},s.prototype._updateSvBackground=function(){this.svSpaceElement.style.backgroundColor="hsl("+this.hsv[0]+",100%,50%)"},s.prototype._redrawHueHandle=function(){var e=this.wheelDiameter/2,t=e-this.wheelThickness/2,n=(this.hsv[0]-90)*Math.PI/180,h=this.hueHandleElement.style
h.left=t*Math.cos(n)+e+"px",h.top=t*Math.sin(n)+e+"px"},s.prototype._redrawSvHandle=function(){var e=this.svSpaceElement,t=this.svHandleElement.style
t.left=e.offsetLeft+e.offsetWidth*this.hsv[1]/100+"px",t.top=e.offsetTop+e.offsetHeight*(1-this.hsv[2]/100)+"px"},s.prototype._option=function(e){var t=this.options[e]
return void 0!==t?t:o[e]},s.default=s,s.defaultOptions=o,s.hsv2hsl=i,s.hsl2hsv=e,s}()}())rawliteral";