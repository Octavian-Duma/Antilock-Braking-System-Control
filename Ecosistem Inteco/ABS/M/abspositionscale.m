function ret = ABSPositionScale( Pos )

Pos = min(3, max(1, Pos));
cl_abs = cabs;
ret = get(cl_abs, 'ScaleCoeff');
ret = ret ( Pos );