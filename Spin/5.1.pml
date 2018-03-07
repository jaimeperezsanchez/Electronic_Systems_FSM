ltl spec {
	[] (((state == LED_OFF) && button) -> <> (state == LED_ON)) &&
	[] ((state == LED_ON) -> <> (state == LED_OFF))
}

#define timeout true

mtype = {LED_OFF, LED_ON}
bit button;
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
		:: timeout -> state = LED_OFF
		fi
	}
	od
}

active proctype excitacion () {
do
::if
  :: button = 1
  :: (! button) -> skip
  fi ;
  printf ("button = %d, state = %d\n", button, state)
od
}