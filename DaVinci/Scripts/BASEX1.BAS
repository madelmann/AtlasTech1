PRINT "This program demostrates all commands."

FOR X = 1 TO 2
	PRINT X, X/2; X, X*X
NEXT

GOSUB 300
PRINT "hello"

REM Kommentarzeile
CALL testsub


PRINT 12-4/2
PRINT 100
200 A = 100/2
IF A>10 THEN PRINT "this is ok"
PRINT A
PRINT A+34
END
300 PRINT "this is a subroutine"
    RETURN

SUB testsub
	PRINT "Hello from 'testsub'!"
RETURN
	
