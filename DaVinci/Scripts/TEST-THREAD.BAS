PRINT "This program is a threading test."

FOR Y = 1 TO 5
	FOR X = 1 TO 5
		SYSTEM "TELL this is executed from a basic script and is running in a thread."
		SLEEP 500
	NEXT
NEXT
	
