ltl spec {
	[] (((codigo[0]==1) && (codigo[1]==2) && (codigo[2]==3) && (state == APAGADO)) -> <> (state == ENCENDIDO)) &&
	[] (((codigo[0]==1) && (codigo[1]==2) && (codigo[2]==3) && (state == ENCENDIDO)) -> <> (state == APAGADO))
}

#define timeout true
mtype = {APAGADO, ENCENDIDO}
bit button;
bit button2;
byte state;
byte codigo[3];
	codigo[0] = 0;
	codigo[1] = 0;
	codigo[2] = 0;
byte indice;
	indice = 0;

active proctype lampara_timeout(){
	state = APAGADO;
	do
	:: (state == APAGADO) -> atomic {
		if
		:: button -> button = 0; codigo[indice]=(codigo[indice] + 1) %10
		:: timeout -> indice = indice + 1
		fi
		if 
		:: ((indice == 2) && (codigo[0]==1) && (codigo[1]==2) && (codigo[2]==3)) -> state = ENCENDIDO; indice = 0; codigo[0] = 0; codigo[1] = 0; codigo[2] = 0
		:: ((indice == 2) && !((codigo[0]==1) && (codigo[1]==2) && (codigo[2]==3)) ) -> indice = 0; codigo[0] = 0; codigo[1] = 0; codigo[2] = 0
		fi
	}
	:: (state == ENCENDIDO) -> atomic {
		if
		:: button -> button = 0; codigo[indice]=(codigo[indice] + 1) %10
		:: timeout -> indice = indice + 1
		:: button2 -> button2 = 0
		fi
		if 
		:: ((indice == 2) && (codigo[0]==1) && (codigo[1]==2) && (codigo[2]==3)) -> state = APAGADO; indice = 0; codigo[0] = 0; codigo[1] = 0; codigo[2] = 0
		:: ((indice == 2) && !((codigo[0]==1) && (codigo[1]==2) && (codigo[2]==3)) ) -> indice = 0; codigo[0] = 0; codigo[1] = 0; codigo[2] = 0
		fi
	}
	od
}

active proctype excitacion () {
do
  	:: button = 1
  	:: (! button) -> skip
od
do
  	:: button2 = 1
	:: (! button2) -> skip
od
}