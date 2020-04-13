#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>Slighter</title></head>
<body>
  <h1 style="color:blue;font-family:arial">SLIGHTER CONFIG PAGE</h1>
  <h2>Device Information</h2>
  <table>
    <tr>
		<td>PJU code</td>
		<td>: </td>
		<td>%PJU_CODE%</td>
		<td></td>
		<td></td>
		<td>Backend server</td>
		<td>: </td>
		<td>%BACKSERVER%</td>
	</tr>
	<tr>
		<td>Model</td>
		<td>: </td>
		<td>%MODEL%</td>
		<td></td>
		<td></td>
		<td>Backend pass</td>
		<td>: </td>
		<td>%BACKPASS%</td>
	</tr>
    <tr>
		<td>FW Version</td>
		<td>: </td>
		<td>%FW_VERSION%</td>
		<td></td>
		<td></td>
		<td>Backend port</td>
		<td>: </td>
		<td>%BACKPORT%</td>
	</tr>
    <tr>
		<td>SSID name</td>
		<td>: </td>
		<td>%SSID%</td>
		<td></td>
		<td></td>
	</tr>
	<tr>
		<td>SSID pass</td>
		<td>: </td>
		<td>%PASSWRD%</td>
		<td></td>
		<td></td>
	</tr>
	<tr>
		<td>APN</td>
		<td>: </td>
		<td>%APN%</td>
		<td></td>
		<td></td>
	</tr>
  </table>
  <h2>Device configuration</h2>
  <form action = "/get"> 
  <table border="0">
    <tr>
		<td>APN</td>
		<td>: <input type="text" name="apn" value="xl"></td>
	</tr>
    <tr>
		<td>SSID</td>
		<td>: <input type="text" name="ssid" value="eyro"></td>
	</tr>
    <tr>
		<td>SSID Password</td>
		<td>: <input type="text" name="ssid_password" value="eyro1234"></td>
	</tr>
    <tr>
		<td>Backend Server</td>
		<td>: <input type="text" name="backend_server" value="158.140.167.173"></td>
	</tr>
    <tr>
		<td>Backend Port</td>
		<td>: <input type="text" name="backend_port" value="1884"></td>
	</tr>
    <tr>
		<td>Backend Username</td>
		<td>: <input type="text" name="backend_username" value="eyroMQTT"></td>
	</tr>
    <tr>
		<td>Backend Password </td>
		<td>: <input type="text" name="backend_password" value="eyroMQTT1234"></td>
	</tr>
  </table>
  <br>
  <input type="submit" value="Save Config">
  <form action = "/reset"><input type="reset"></form> 
  </form> 
</body>
</html>)rawliteral";