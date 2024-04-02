PRINT "This program is a scripting sequence test."

SYSTEMBEGIN
	GAME.SCRIPTSEQUENCE 1
	TELL Welcome adventurer in the ancient lands of DaVinci!
	TELL 
	TELL Open lands full of treasures and evil demons ly at your feets.
	CAMERA.SETPOSITION -1275.0 140.0 -1230.0
	CAMERA.SETVIEW -1300.0 134.0 -1266.0
	REGION.LOCK
SYSTEMEND

SLEEP 5000

SYSTEM "GAME.SCRIPTSEQUENCE 0"
	
