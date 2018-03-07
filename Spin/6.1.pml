ltl spec {
	[] (((state == LED_OFF) && button) -> <> (state == LED_ON)) &&
	[] (((state == LED_ON) && button) -> <> (state == LED_OFF)) 
}

mtype = {LED_OFF, LED_ON}
bit button;
bit button2;
byte state;

active proctype lampara_timeout(){
	state = LED_OFF;
	do
	:: (state == LED_OFF) -> atomic {
		if
		:: button -> state = LED_ON; button = 0		
		fi
	}
	:: (state == LED_ON) -> atomic {
		if
		:: button -> state = LED_OFF; button = 0
		:: button2 -> button2 = 0
		fi
	}
	od
}

active proctype excitacion () {
do
::if
  	:: button = 1
  	:: (! button) -> skip
  fi
od
do
::if
  	:: button2 = 1
	:: (! button2) -> skip
  fi 
od
}