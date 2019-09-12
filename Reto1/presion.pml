ltl temp {
	( [] (((temp_fuera < th_out) && (temp_in<th_in1||carga<th1_carga_carga)) -> <> (state == P0)) ) &&
	( [] (((temp_fuera>th_out) && ((temp_in>th_in1 && temp_in<th_in2) || (carga>th1_carga_carga && carga<th2_carga))) -> <> (state == P1)) ) &&
	( [] (((temp_fuera>th_out) && (temp_in>th_in2&&carga>th2_carga)) -> <> (state == P2)) ) 
}

mtype = {P0, P1, P2}

short temp_fuera;
short temp_in;
byte carga;
byte th_in1 = 20;
byte th_in2 = 40;
byte th1_carga = 40;
byte th2_carga = 70;
byte th_out = 20;
byte state;

active proctype control ()
{
	state = P0;
	do
	:: (state == P0) -> atomic{
		if
		:: (((temp_fuera>th_out) && ((temp_in>th_in1 && temp_in<th_in2) || (carga>th1_carga_carga && carga<th2_carga)))) -> state = P1
		:: (((temp_fuera>th_out) && (temp_in>th_in2||carga>th2_carga))) -> state = P2
		:: (((temp_fuera < th_out) && (temp_in<th_in1||carga<th1_carga_carga))) -> skip
		fi;
	}
	:: (state == P1) -> atomic{
		if
		:: (((temp_fuera < th_out) && (temp_in<th_in1||carga<th1_carga_carga))) -> state = P0
		:: (((temp_fuera>th_out) && (temp_in>th_in2||carga>th2_carga))) -> state = P2
		:: (((temp_fuera>th_out) && ((temp_in>th_in1 && temp_in<th_in2) || (carga>th1_carga_carga && carga<th2_carga)))) -> skip
		fi;
	}
	:: (state == P2) -> atomic{
		if
		:: (((temp_fuera < th_out) && (temp_in<th_in1||carga<th1_carga_carga))) -> state = P0
		:: (((temp_fuera>th_out) && ((temp_in>th_in1 && temp_in<th_in2) || (carga>th1_carga_carga && carga<th2_carga)))) -> state = P1
		:: (((temp_fuera>th_out) && (temp_in>th_in2||carga>th2_carga))) -> skip
		fi;
	}
	od;
}

active proctype excitacion () {
do
::if
	:: ((temp_fuera >= -10) && (temp_fuera < 100)) -> temp_fuera++	
	:: (temp_fuera >= 100) -> temp_fuera = 50
	:: ((temp_in >= -10) && (temp_in < 100)) -> temp_in++	
	:: (temp_in >= 100) -> temp_in = 50
	:: ((carga >= 0) && (carga < 100)) -> carga++	
	:: (carga >= 100) -> carga = 50
	:: skip
  fi;
  printf ("temp_fuera = %d, temp_in = %d, carga = %d, th1_carga_carga = %d, th2_carga = %d, th_out = %d, th_in1 = %d, th_in2 = %d, estado = %e\n\n", temp_fuera, temp_in, carga, th1_carga_carga, th2_carga, th_out, th_in1, th_in2, state)
od;
}
