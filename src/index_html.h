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
		<td>Backend user</td>
		<td>: </td>
		<td>%BACKUSER%</td>
	</tr>
    <tr>
		<td>FW Version</td>
		<td>: </td>
		<td>%FW_VERSION%</td>
		<td></td>
		<td></td>
		<td>Backend pass</td>
		<td>: </td>
		<td>%BACKPASSDSPLY%</td>
	</tr>
    <tr>
		<td>SSID name</td>
		<td>: </td>
		<td>%SSID%</td>
		<td></td>
		<td></td>
		<td>Backend port</td>
		<td>: </td>
		<td>%BACKPORT%</td>
	</tr>
	<tr>
		<td>SSID pass</td>
		<td>: </td>
		<td>%PASSWRDDSPLY%</td>
		<td></td>
		<td></td>
		<td>Device Time</td>
		<td>: </td>
		<td>%DEVTIME%</td>
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
  <h3>connection setting</h3>
  <form action = "/get"> 
  <table border="0">
    <tr>
		<td>APN</td>
		<td>: <input type="text" name="apn" value=%APN%></td>
	</tr>
    <tr>
		<td>SSID</td>
		<td>: <input type="text" name="ssid" value=%SSID%></td>
	</tr>
    <tr>
		<td>SSID Password</td>
		<td>: <input type="password" name="ssid_password" id="ssid_password" value=%PASSWRD%></td>
	</tr>
    <tr>
		<td>Backend Server</td>
		<td>: <input type="text" name="backend_server" value=%BACKSERVER%></td>
	</tr>
    <tr>
		<td>Backend Port</td>
		<td>: <input type="text" name="backend_port" value=%BACKPORT%></td>
	</tr>
    <tr>
		<td>Backend Username</td>
		<td>: <input type="text" name="backend_username" value=%BACKUSER%></td>
	</tr>
    <tr>
		<td>Backend Password </td>
		<td>: <input type="password" id="backend_password" name="backend_password" value=%BACKPASS%></td>
	</tr>
  </table>
	<br>
	<input type="submit" value="Save Config">
</form> 

<h3>Lamp setting</h3>
<form action = "/lamp">
<table border="0">
	<tr>
		<td>Mode </td>
		<td>: <select id="mode" name="mode">
  			<option value="0">Manual</option>
  			<option value="1">Auto</option>
  			<option value="2">Night Mode</option>
			</select></td>
	</tr>
	<tr>
		<td>On Schedule </td>
		<td>: <input type="time" name="time_on">
	</tr>
	<tr>
		<td>Off Schedule </td>
		<td>: <input type="time" name="time_off">
	</tr>
	<tr>
		<td>Night Schedule </td>
		<td>: <input type="time" name="time_night">
	</tr>
	<tr>
		<td>Manual Control</td>
		<td>: <select id="control" name="control">
  			<option value="0">Off</option>
  			<option value="1">On</option>
			</select></td>
	</tr>
	<tr>
		<td>Lamp Power </td>
		<td>: <input type="number" name="lamp_power" min="1" max="250">
	</tr>
</table>
<br>
<input type="submit" value="save config">
</form>
<br>
<form action="/timesync">
  <label for="timesync">Timesync (date and time):</label>
  <input type="datetime-local" id="timesync" name="timesync">
  <input type="submit" value="sync">
</form>
<p>
	<a href="/reboot"><button class="button">Reboot</button> &emsp;
	<a href="/resetwh"><button class="button">Reset WH</button> &emsp;
</p>  
</body>
</html>)rawliteral";