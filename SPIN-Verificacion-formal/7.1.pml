ltl spec {
	[] (((state == APAGADO_0) && button) -> <> (state == APAGADO_1)) &&
	[] (((state == APAGADO_1) && button) -> <> (state == APAGADO_2)) &&
	[] ((state == APAGADO_1) -> <> (state == APAGADO_0)) &&
	[] (((state == APAGADO_2) && button) -> <> (state == APAGADO_3)) &&
	[] ((state == APAGADO_2) -> <> (state == APAGADO_0)) &&
	[] (((state == APAGADO_3) && button) -> <> (state == APAGADO_0)) &&
	[] ((state == APAGADO_3) -> <> (state == ENCENDIDO_0)) &&
	[] (((state == ENCENDIDO_0) && button) -> <> (state == ENCENDIDO_1)) &&
	[] (((state == ENCENDIDO_1) && button) -> <> (state == ENCENDIDO_2)) &&
	[] ((state == ENCENDIDO_1) -> <> (state == ENCENDIDO_0)) &&
	[] (((state == ENCENDIDO_2) && button) -> <> (state == APAGADO_3)) &&
	[] ((state == APAGADO_2) -> <> (state == ENCENDIDO_3)) &&
	[] (((state == ENCENDIDO_3) && button) -> <> (state == ENCENDIDO_0)) &&
	[] ((state == ENCENDIDO_3) -> <> (state == APAGADO_0))
}

#define timeout true
mtype = {APAGADO_0, ENCENDIDO_0, APAGADO_1, ENCENDIDO_1, APAGADO_2, ENCENDIDO_2, APAGADO_3, ENCENDIDO_3}
bit button;
bit button2;
byte state;

active proctype lampara_timeout(){
	state = APAGADO_0;
	do
	:: (state == APAGADO_0) -> atomic {
		if
		:: button -> state = APAGADO_1; button = 0
		fi
	}
	:: (state == APAGADO_1) -> atomic {
		if
		:: button -> state = APAGADO_2; button = 0
		:: timeout -> state = APAGADO_0
		fi
	}
	:: (state == APAGADO_2) -> atomic {
		if
		:: button -> state = APAGADO_3; button = 0
		:: timeout -> state = APAGADO_0
		fi
	}
	:: (state == APAGADO_3) -> atomic {
		if
		:: button -> state = APAGADO_0; button = 0
		:: timeout -> state = ENCENDIDO_0
		fi
	}
	:: (state == ENCENDIDO_0) -> atomic {
		if
		:: button -> state = ENCENDIDO_1; button = 0
		:: button2 -> button2 = 0
		fi
	}
	:: (state == ENCENDIDO_1) -> atomic {
		if
		:: button -> state = ENCENDIDO_2; button = 0
		:: timeout -> state = ENCENDIDO_0
		fi
	}
	:: (state == ENCENDIDO_2) -> atomic {
		if
		:: button -> state = ENCENDIDO_3; button = 0
		:: timeout -> state = ENCENDIDO_0
		fi
	}
	:: (state == ENCENDIDO_3) -> atomic {
		if
		:: button -> state = ENCENDIDO_0; button = 0
		:: timeout -> state = APAGADO_0
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