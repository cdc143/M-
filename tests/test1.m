%Check precedence of operations
begin
z:=0; %Must init z before using it
y:=2;
x:=3;
z:=x+y*z; %Answer should be 3, not 0
write x;
end
