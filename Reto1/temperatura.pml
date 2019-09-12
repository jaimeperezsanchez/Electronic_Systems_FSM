ltl temp {
	( [] (((t1 >= th1) && (t2 >= th2)) -> <> (state == O_2)) ) &&
	( [] (((t1 < th1) && (t2 >= th2)) -> <> (state == O_1_D)) ) &&
	( [] (((t1 >= th1) && (t2 < th2)) -> <> (state == O_1_I)) ) &&
	( [] (((t1 < th1) && (t2 < th2)) -> <> (state == C_2)) )
}

mtype = {O_2, O_1_D, O_1_I, C_2}
int t1;	
int t2;	
byte th1=80;
byte th2=80;
byte state;

active proctype control ()
{
	state = C_2;
	do
	:: (state == O_2) -> atomic{
		if
		:: ((t1 < th1) && (t2 >= th2)) -> state = O_1_D
		:: ((t1 >= th1) && (t2 < th2)) -> state = O_1_I
		:: ((t1 < th1) && (t2 < th2)) -> state = C_2
		:: ((t1 >= th1) && (t2 >= th2)) -> skip
		fi
	}
	:: (state == O_1_D) -> atomic{
		if
		:: ((t1 >= th1) && (t2 >= th2)) -> state = O_2
		:: ((t1 >= th1) && (t2 < th2)) -> state = O_1_I
		:: ((t1 < th1) && (t2 < th2)) -> state = C_2
		:: ((t1 < th1) && (t2 >= th2)) -> skip
		fi
	}
	:: (state == O_1_I) -> atomic{
		if
		:: ((t1 >= th1) && (t2 >= th2)) -> state = O_2
		:: ((t1 < th1) && (t2 >= th2)) -> state = O_1_D
		:: ((t1 < th1) && (t2 < th2)) -> state = C_2
		:: ((t1 >= th1) && (t2 < th2)) -> skip
		fi
	}
	:: (state == C_2) -> atomic{
		if
		:: ((t1 >= th1) && (t2 >= th2)) -> state = O_2
		:: ((t1 < th1) && (t2 >= th2)) -> state = O_1_D
		:: ((t1 >= th1) && (t2 < th2)) -> state = O_1_I
		:: ((t1 < th1) && (t2 < th2)) -> skip
		fi
	}
	od
}

active proctype excitacion () {
do
::if
	:: ((t1 >= -10) && (t1 < 100)) -> t1++	
	:: (t1 >= 100) -> t1 = 50
	:: ((t2 >= -10) && (t2 < 100)) -> t2++	
	:: (t2 >= 100) -> t2 = 50
	:: skip
  fi;
  printf ("t1 = %d, t2 = %d, th1 = %d, th2 = %d, estado = %e\n\n", t1, t2, th1, th2, state)
od;
}

