function  [sys,x0,str,ts] = sabs(t,x,u,flag,r1,r2,J1,J2,d1,d2,M10,M20,Mg,w,p,a,L,phi,u0,b1,b2,tau,xini)
% S - function sabs
switch flag
	case 0  % Initialization
    sys = [3,  % number of continuous states
           0,      % number of discrete states
           7,  	 % number of outputs
           1,      % number of inputs
           0,      % reserved must be zero
           0,      % direct feedthrough flag
           1];     % number of sample times
    x0  = xini;
    str = [];
    ts  = [0 0];   % sample time: [period, offset]

	case 1                                                % Derivatives
   % coefficients
		v1 = r1*x(1) ;
		v2 = r2*x(2) ;
		vr = r2*x(2)-r1*x(1) ;
		svr = sign(vr) ;
		s1 = tanh(x(1)) ;
		s2 = tanh(x(2)) ;
		M10 = s1*M10 ;
		M20 = s2*M20 ;
		sn = sin(phi) ;
		cn = cos(phi) ;
		c11 = r1*d1/J1 ;
		c12 = (M10+Mg)*r1/J1 ;
		c13 = -d1/J1 ;
		c14 = -M10/J1 ;
		c15 = r1/J1 ;
		c16 = -1/J1 ;
		c21 = -r2*d1/J2 ;
		c22 = -(M10+Mg)*r2/J2 ;
		c23 = -d2/J2 ;
		c24 = -M20/J2 ;
		c25 = -r2/J2 ;
		c31 = 1/tau ;
		ee = 1 ;
      rho = 1 ;
      nv = norm([v1 v2]) ;
      nve = nv/ee ;
		if nv<ee
			rho = (3-2*nve)*nve^2 ;
		end

   % slip
      if v2>=v1
         if (v1>0)&(v2>0)
            lam = (v2-v1)/(v2) ;
         elseif (v1<=0)&(v2>=0)
            lam = 1 ;
         elseif (v1<0)&(v2<=0)
            lam = (v1-v2)/(v1) ;
         end
      else
         if (v1>0)&(v2>0)
            lam = (v1-v2)/(v1) ;
         elseif (v1>=0)&(v2<=0)
            lam = 1 ;
         elseif (v1<=0)&(v2<0)
            lam = (v2-v1)/(v2) ;
         end
      end
      
      lam = lam*rho ;
		%krzywa tarcia      
      lp = lam^p ;
		mu = w(4)*lp/(a+lp) ;
		mu = mu + w(3)*lam^3+w(2)*lam^2+w(1)*lam;
      mu = svr*mu ; 
      mu = mu*rho ;

      % S(lam)
      S = (mu/L)/(sn-mu*cn) ;
      % hamulec
      
      if u>=u0
         b = b1*u+b2 ;
      else
         b = 0.0 ;
      end

      dx(1,1) = S*(c11*x(1)+c12)+c13*x(1)+c14+(c15*S+c16)*s1*x(3);
      dx(2,1) = S*(c21*x(1)+c22)+c23*x(2)+c24+c25*S*s1*x(3) ;
		dx(3,1) = c31*(b-x(3)) ;       
      sys = dx;
            

  	case 2                                                % Discrete state update
    sys = []; % do nothing

	case 3																	% Outputs
   % coefficients
		v1 = r1*x(1) ;
		v2 = r2*x(2) ;
		vr = r2*x(2)-r1*x(1) ;
		svr = sign(vr) ;
		s1 = tanh(x(1)) ;
		s2 = tanh(x(2)) ;
		M10 = s1*M10 ;
		M20 = s2*M20 ;
		sn = sin(phi) ;
		cn = cos(phi) ;
		c11 = r1*d1/J1 ;
		c12 = (M10+Mg)*r1/J1 ;
		c13 = -d1/J1 ;
		c14 = -M10/J1 ;
		c15 = r1/J1 ;
		c16 = -1/J1 ;
		c21 = -r2*d1/J2 ;
		c22 = -(M10+Mg)*r2/J2 ;
		c23 = -d2/J2 ;
		c24 = -M20/J2 ;
		c25 = -r2/J2 ;
		c31 = 1/tau ;
		ee = 1 ;
		rho = 1 ;
      nv = norm([v1 v2]) ;
      nve = nv/ee ;
		if nv<ee
			rho = (3-2*nve)*nve^2 ;
		end

      % slip
      if v2>=v1
         if (v1>0)&(v2>0)
            lam = (v2-v1)/(v2) ;
         elseif (v1<=0)&(v2>=0)
            lam = 1 ;
         elseif (v1<0)&(v2<=0)
            lam = (v1-v2)/(v1) ;
         end
      else
         if (v1>0)&(v2>0)
            lam = (v1-v2)/(v1) ;
         elseif (v1>=0)&(v2<=0)
            lam = 1 ;
         elseif (v1<=0)&(v2<0)
            lam = (v2-v1)/(v2) ;
         end
      end
      lam = lam*rho ;
		%friction curve      
      lp = lam^p ;
		mu = w(4)*lp/(a+lp) ;
		mu = mu + w(3)*lam^3+w(2)*lam^2+w(1)*lam;
      mu = svr*mu ; 
      mu = mu*rho ;
      Fn = (s1*x(3)+M10+Mg+d1*x(1))/(L*sn-L*mu*cn) ;
      Ff = mu*Fn ;
      sys = [x(1:3);lam;abs(mu);Fn;Ff];
  
  case 9                                                % Terminate
    sys = []; % do nothing

  otherwise
    error(['unhandled flag = ',num2str(flag)]);
end
