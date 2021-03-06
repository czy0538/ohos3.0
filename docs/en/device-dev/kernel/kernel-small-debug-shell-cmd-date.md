# date<a name="EN-US_TOPIC_0000001134006248"></a>

-   [Command Function](#section56472016338)
-   [Syntax](#section16635112512316)
-   [Parameters](#section15896030039)
-   [Usage](#section116361036636)
-   [Example](#section021711411237)
-   [Output](#section17950184414312)

## Command Function<a name="section56472016338"></a>

This command is used to query and set the system date and time.

## Syntax<a name="section16635112512316"></a>

date

date --help

date +\[_Format_\]

date -s_ _\[_YY/MM/DD_\]

date_ _-s_ _\[_hh:mm:ss_\]__

date -r \[_Filename_\]

## Parameters<a name="section15896030039"></a>

**Table  1**  Parameter description

<a name="table3825mcpsimp"></a>
<table><thead align="left"><tr id="row3831mcpsimp"><th class="cellrowborder" valign="top" width="21%" id="mcps1.2.4.1.1"><p id="p3833mcpsimp"><a name="p3833mcpsimp"></a><a name="p3833mcpsimp"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.4.1.2"><p id="p3835mcpsimp"><a name="p3835mcpsimp"></a><a name="p3835mcpsimp"></a>Description</p>
</th>
<th class="cellrowborder" valign="top" width="28.999999999999996%" id="mcps1.2.4.1.3"><p id="p3837mcpsimp"><a name="p3837mcpsimp"></a><a name="p3837mcpsimp"></a>Value Range</p>
</th>
</tr>
</thead>
<tbody><tr id="row3838mcpsimp"><td class="cellrowborder" valign="top" width="21%" headers="mcps1.2.4.1.1 "><p id="p3840mcpsimp"><a name="p3840mcpsimp"></a><a name="p3840mcpsimp"></a>--help</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.4.1.2 "><p id="p3842mcpsimp"><a name="p3842mcpsimp"></a><a name="p3842mcpsimp"></a>Displays the help information.</p>
</td>
<td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.4.1.3 "><p id="p3844mcpsimp"><a name="p3844mcpsimp"></a><a name="p3844mcpsimp"></a>N/A</p>
</td>
</tr>
<tr id="row3845mcpsimp"><td class="cellrowborder" valign="top" width="21%" headers="mcps1.2.4.1.1 "><p id="p3847mcpsimp"><a name="p3847mcpsimp"></a><a name="p3847mcpsimp"></a>+Format</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.4.1.2 "><p id="p3849mcpsimp"><a name="p3849mcpsimp"></a><a name="p3849mcpsimp"></a>Prints the date and time based on <strong id="b3850mcpsimp"><a name="b3850mcpsimp"></a><a name="b3850mcpsimp"></a>Format</strong>.</p>
</td>
<td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.4.1.3 "><p id="p3852mcpsimp"><a name="p3852mcpsimp"></a><a name="p3852mcpsimp"></a>Placeholders listed in <strong id="b14613228303338"><a name="b14613228303338"></a><a name="b14613228303338"></a>--help</strong>.</p>
</td>
</tr>
<tr id="row3853mcpsimp"><td class="cellrowborder" valign="top" width="21%" headers="mcps1.2.4.1.1 "><p id="p3855mcpsimp"><a name="p3855mcpsimp"></a><a name="p3855mcpsimp"></a>-s YY/MM/DD</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.4.1.2 "><p id="p3857mcpsimp"><a name="p3857mcpsimp"></a><a name="p3857mcpsimp"></a>Sets the system date and separates the year, month, and day by slashes (/).</p>
</td>
<td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.4.1.3 "><p id="p3859mcpsimp"><a name="p3859mcpsimp"></a><a name="p3859mcpsimp"></a>&gt;= 1970/01/01</p>
</td>
</tr>
<tr id="row3860mcpsimp"><td class="cellrowborder" valign="top" width="21%" headers="mcps1.2.4.1.1 "><p id="p3862mcpsimp"><a name="p3862mcpsimp"></a><a name="p3862mcpsimp"></a>-s hh:mm:ss</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.4.1.2 "><p id="p3864mcpsimp"><a name="p3864mcpsimp"></a><a name="p3864mcpsimp"></a>Sets the system time and separates the hour, minute, and second by colons (:).</p>
</td>
<td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.4.1.3 "><p id="p3866mcpsimp"><a name="p3866mcpsimp"></a><a name="p3866mcpsimp"></a>N/A</p>
</td>
</tr>
<tr id="row3867mcpsimp"><td class="cellrowborder" valign="top" width="21%" headers="mcps1.2.4.1.1 "><p id="p3869mcpsimp"><a name="p3869mcpsimp"></a><a name="p3869mcpsimp"></a>-r Filename</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.4.1.2 "><p id="p3871mcpsimp"><a name="p3871mcpsimp"></a><a name="p3871mcpsimp"></a>Queries the modification time of the <strong id="b12095159793338"><a name="b12095159793338"></a><a name="b12095159793338"></a>Filename</strong> file.</p>
</td>
<td class="cellrowborder" valign="top" width="28.999999999999996%" headers="mcps1.2.4.1.3 "><p id="p3873mcpsimp"><a name="p3873mcpsimp"></a><a name="p3873mcpsimp"></a>N/A</p>
</td>
</tr>
</tbody>
</table>

## Usage<a name="section116361036636"></a>

-   If the  **date**  parameter is not specified, the current system date and time are displayed by default.
-   The  **--help**,  **+Format**,  **-s**, and  **-r**  parameters are mutually exclusive.

## Example<a name="section021711411237"></a>

Example:

Run  **date +%Y--%m--%d**.

## Output<a name="section17950184414312"></a>

**Figure  1**  System date printed based on the specified format<a name="fig152576304517"></a>  
![](figure/system-date-printed-based-on-the-specified-format.png "system-date-printed-based-on-the-specified-format")

