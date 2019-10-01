Get-Content "C:\Users\Aaron\Documents\College\Fall 2019\Embedded Systems\Lab 2\StoplightCommand.html" | foreach-object {
$curline = $_
$curline = $curline -replace "\\", "\\"
$curline = $curline -replace "`"", "\`""
##Not sure we need to escape question marks. Uncomment the line below if you have issues.
#$curline = $curline -replace "\?", "\?"
$newline = "client.println(`"" + $curline + "`");"
$newline | Out-File "C:\Users\Aaron\Documents\College\Fall 2019\Embedded Systems\Lab 2\To_Arduino.txt" -Append
}